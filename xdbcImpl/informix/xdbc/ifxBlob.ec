#include "xdbc/ifxBlob.h"
#include "xdbc/ifxConnection.h"

__OPENCODE_BEGIN_NAMESPACE

//-------------------------------------------------------------------------

IfxBlob::IfxBlob(IfxConnection* ora_conn):Blob()
{
  __mp_conn=ora_conn;
}

IfxBlob::IfxBlob(IfxConnection* ora_conn,struct ifx_lo_ts* p_ec_lo_ts)
{
  __mp_conn=ora_conn;

  if(p_ec_lo_ts != 0)
  {
    ifx_int8_t size ;
    ifx_int8_t offset ;
    ifx_lo_stat_t *stats =0;

    int isize = 0;
    int error = 0;
    int lofd  = -1;

    lofd =  ifx_lo_open(p_ec_lo_ts, LO_RDONLY, &error);
    ifx_lo_stat(lofd, &stats);

    if((ifx_lo_stat_size(stats, &size)) < 0)
    {
      isize = 0;
    }
    else if(ifx_int8toint(&size, &isize) !=  0)
    {
      isize = 0;
    }

    ifx_lo_stat_free(stats);

    char* p_buf = new char[isize + 1];
    memset(p_buf,0,isize + 1);

    ifx_int8cvint(0, &offset);
    ifx_lo_readwithseek(lofd, p_buf, isize, &offset, LO_SEEK_CUR, &error);

    lob_stream.assign(p_buf,isize);

    delete []p_buf;
    p_buf = 0;

    ifx_lo_close(lofd);
  }
}

IfxBlob::IfxBlob(IfxConnection* ora_conn,struct tag_loc_t* p_ec_locator):Blob()
{
  __mp_conn=ora_conn;
  
  if(p_ec_locator != 0 )
  {
    lob_stream.insert(0,p_ec_locator->loc_buffer,p_ec_locator->loc_size);
  }
}

IfxBlob::~IfxBlob()
{
}

int IfxBlob::length(__XDBC_HOOK_DECLARE_ONCE) const  __XDBC_THROW_DECLARE(SQLException)
{
  return (int)lob_stream.length();
}

int IfxBlob::write(__XDBC_HOOK_DECLARE_MORE int pos, const char* str, int offset, int len)  __XDBC_THROW_DECLARE(SQLException)
{
  lob_stream.insert(pos-1,str+offset,len);

  return len;
}

int IfxBlob::read(__XDBC_HOOK_DECLARE_MORE int pos, char* str,int offset,int length) const  __XDBC_THROW_DECLARE(SQLException)
{
  int count = this->length(__XDBC_HOOK_INVOKE_ONCE) - pos + 1;
  __XDBC_HOOK_CHECK(return -1);
  
  if(count < 0)
  {
    return 0;
  }

  const char* start = lob_stream.data() + pos - 1;

  count = __M_min(count,length);
  memcpy(str+offset,start,count);

  return count;
}

void IfxBlob::truncate(__XDBC_HOOK_DECLARE_MORE int len)  __XDBC_THROW_DECLARE(SQLException)
{
  lob_stream.resize(len);
}

void IfxBlob::swap(IfxBlob& blob)
{
  IfxConnection* p_tmp_conn=__mp_conn;

  __mp_conn=blob.__mp_conn;

  blob.__mp_conn=p_tmp_conn;

  lob_stream.swap(blob.lob_stream);
}

__OPENCODE_END_NAMESPACE
