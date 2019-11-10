#include "xdbc/ifxConversion.h"
#include "xdbc/ifxConnection.h"
#include "xdbc/xtypes.h"
#include "xdbc/ifxTypes.h"
#include "xdbc/XDBCUtilities.h" 

#include "port/ioc_sbuf.h"
#include "port/utilities.h"
#include "port/date.h"
#include "port/ioc_smart.h"
#include "port/libnumber.h"
#include "port/utilities.h"
#include "port/libmath.h" 

#include <sqlca.h>
#include <sqlhdr.h>

__OPENCODE_BEGIN_NAMESPACE

#define IFXCONVERSION_CLASS_NAME "IfxConversion"

IfxConversion::IfxConversion()
{
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

int   IfxConversion::DATABASE_TO_XDBC(int sqlt)
{
  return IfxTypes::getInstance().Ifx_2_XDBC(sqlt);
}

int   IfxConversion::DATABASE_TO_XDBC(const IfxColumn& column)
{
  return IfxTypes::getInstance().Ifx_2_XDBC(column);
}

int   IfxConversion::XDBC_TO_DATABASE(int xtype)
{
  return IfxTypes::getInstance().XDBC_2_Ifx(xtype);;
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------

bool IfxConversion::parseDateTime(struct dtime* p_dtime,XDBC_TIMESTAMP& ts)
{
  vector<int> fields = Date::parseFields(IfxConversion::toString(p_dtime),1970,1,1);

  vector<int> fvalues(7);
  fvalues[0] = 1970;
  fvalues[1] = 1;
  fvalues[2] = 1;

  fvalues[3] = 0;
  fvalues[4] = 0;
  fvalues[5] = 0;
  fvalues[6] = 0;

  fixDateTimeInterval(fields,p_dtime->dt_qual,fvalues);
  
  ts.year = fields[0];
  ts.month = fields[1];
  ts.day = fields[2];
  
  ts.hour = fields[3];
  ts.minute = fields[4];
  ts.second = fields[5];
  
  ts.fraction = fields[6];
  
  return true;
}

void  IfxConversion::makeDateTime(const XDBC_TIMESTAMP& ts,struct dtime* p_dtime)
{
  char buf[40] ={0};

  vector<int> fields(7);
  fields[0] = ts.year;
  fields[1] = ts.month;
  fields[2] = ts.day;
  
  fields[3] = ts.hour;
  fields[4] = ts.minute;
  fields[5] = ts.second;
  
  fields[6] = ts.fraction;

  formatDateTimeInterval(fields,p_dtime->dt_qual,buf);

  if(strlen(buf) == 0)
  {
    sprintf(buf,"%d-%d-%d %d:%d:%d.%3d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
  }

  dtcvasc(buf,p_dtime);
}

void  IfxConversion::makeDateTime(const XDBC_DATE& date,struct dtime* p_dtime)
{
  char buf[40] ={0};

  vector<int> fields(7);
  
  fields[0] = date.year;
  fields[1] = date.month;
  fields[2] = date.day;
  
  fields[3] = 0;
  fields[4] = 0;
  fields[5] = 0;
  
  fields[6] = 0;

  formatDateTimeInterval(fields,p_dtime->dt_qual,buf);

  if(strlen(buf) == 0)
  {
    sprintf(buf,"%d-%d-%d %d:%d:%d.%3d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
  }

  dtcvasc(buf,p_dtime);
}

void  IfxConversion::makeDateTime(const XDBC_TIME& date,struct dtime* p_dtime)
{
  char buf[40] ={0};

  vector<int> fields(7);
  fields[0] = 1970;
  fields[1] = 1;
  fields[2] = 1;

  fields[3] = date.hour;
  fields[4] = date.minute;
  fields[5] = date.second;
  fields[6] = 0;

  formatDateTimeInterval(fields,p_dtime->dt_qual,buf);

  if(strlen(buf) == 0)
  {
    sprintf(buf,"%d-%d-%d %d:%d:%d.%3d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
  }

  dtcvasc(buf,p_dtime);
}

bool  IfxConversion::parseInterval(struct intrvl* p_interval,XDBC_INTERVAL& interval)
{
  vector<int> fields = Date::parseFields(IfxConversion::toString(p_interval),0,0,0);

  vector<int> fvalues(7);
  fvalues[0] = 0;
  fvalues[1] = 0;
  fvalues[2] = 0;

  fvalues[3] = 0;
  fvalues[4] = 0;
  fvalues[5] = 0;
  fvalues[6] = 0;

  fixDateTimeInterval(fields,p_interval->in_qual,fvalues);

  int start_field = TU_START(p_interval->in_qual);

  if( start_field >= TU_DAY)
  { 
    interval.interval_type = XDBC_IS_DAY_TO_SECOND;
    interval.interval_sign = XDBC_FALSE;
    if(fields[2] < 0 || fields[3] < 0 || fields[4] < 0 || fields[5] < 0 || fields[6] < 0)
    {
      interval.interval_sign = XDBC_TRUE;
    }
    
    interval.intval.day_second.day = Math::abs(fields[2]);      
    interval.intval.day_second.hour = Math::abs(fields[3]);    
    interval.intval.day_second.minute = Math::abs(fields[4]);   
    interval.intval.day_second.second = Math::abs(fields[5]);   
    interval.intval.day_second.fraction = Math::abs(fields[6]);  
  }
  else
  {
    interval.interval_type = XDBC_IS_YEAR_TO_MONTH;
    interval.interval_sign = XDBC_FALSE;
    if(fields[0] < 0 || fields[1] < 0)
    {
      interval.interval_sign = XDBC_TRUE;
    }  
    interval.intval.year_month.year = Math::abs(fields[0]);
    interval.intval.year_month.month = Math::abs(fields[1]);
  }

  return true;
}

void   IfxConversion::makeInterval(const XDBC_INTERVAL& interval,struct intrvl* p_interval)
{
  char buf[40] ={0};

  vector<int> fields(7);
  
  if(  (interval.interval_type == XDBC_IS_YEAR)
     ||(interval.interval_type == XDBC_IS_MONTH)
     ||(interval.interval_type == XDBC_IS_YEAR_TO_MONTH) )
  {
    if(interval.interval_sign == XDBC_TRUE)
    {
      fields[0] = -(interval.intval.year_month.year);
      fields[1] = -(interval.intval.year_month.month);  
    }   
    else
    {
      fields[0] = interval.intval.year_month.year;
      fields[1] = interval.intval.year_month.month;       
    }  
  }
  else
  {
    if(interval.interval_sign == XDBC_TRUE)
    {
      fields[2] = -(interval.intval.day_second.day);  
      fields[3] = -(interval.intval.day_second.hour);  
      fields[4] = -(interval.intval.day_second.minute);  
      fields[5] = -(interval.intval.day_second.second);  
      fields[6] = -(interval.intval.day_second.fraction); 
    }
    else
    {   
      fields[2] = interval.intval.day_second.day;  
      fields[3] = interval.intval.day_second.hour;  
      fields[4] = interval.intval.day_second.minute;  
      fields[5] = interval.intval.day_second.second;  
      fields[6] = interval.intval.day_second.fraction;   
    } 
  }    


  
  formatDateTimeInterval(fields,p_interval->in_qual,buf);

  if(strlen(buf) == 0)
  {
    sprintf(buf, "%d %d:%d:%d.%3d",fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
  }

  incvasc(buf,p_interval);
}

void IfxConversion::int8ToString(struct ifx_int8* p_ec_int8,string& strLong)
{
  char p_buf[128 + 1];
  memset(p_buf,0,128 + 1);
  ifx_int8toasc(p_ec_int8,p_buf,128);

  char* trip_pos = strchr(p_buf,' ');
  if(trip_pos != 0)
  {
    strLong.assign(p_buf,(trip_pos - p_buf));
  }
  else
  {
    strLong.assign(p_buf);
  }
}

void  IfxConversion::int8ToLong(struct ifx_int8* p_ec_int8,XLONG& intLong)
{
  struct decimal ec_decimal;
  ifx_int8todec(p_ec_int8,&ec_decimal);
  decimalToLong(&ec_decimal,intLong);
}

void  IfxConversion::longToint8(const XLONG& intLong,struct ifx_int8* p_ec_int8)
{
  struct decimal ec_decimal;
  longToDecimal(intLong,&ec_decimal);
  ifx_int8cvdec(&ec_decimal,p_ec_int8);
}

void   IfxConversion::decimalToLong(struct decimal* p_ec_decimal,XLONG& intLong)
{
  int4 int4_temp = 0;
  dectolong(p_ec_decimal, &int4_temp);
  intLong = int4_temp;
}

void   IfxConversion::decimalToString(struct decimal* p_ec_decimal,string& strLong)
{
  char p_buf[128 + 1];
  memset(p_buf,0,128 + 1);
  dectoasc(p_ec_decimal,p_buf,128,-1);

  char* trip_pos = strchr(p_buf,' ');
  if(trip_pos != 0)
  {
    strLong.assign(p_buf,(trip_pos - p_buf));
  }
  else
  {
    strLong.assign(p_buf);
  }
}

void   IfxConversion::longToDecimal(const XLONG& intLong,struct decimal* p_ec_decimal)
{
  int4 int4_temp = (int4)intLong;
  deccvlong(int4_temp,p_ec_decimal); 
}


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------

void IfxConversion::parseResultSetMetaData(struct sqlda*  __mp_ec_sqlda_columns,vector<IfxColumn>& __vector_columns)
{
  if(__mp_ec_sqlda_columns == 0)
  {
    return;
  }

  int numcols = __mp_ec_sqlda_columns->sqld;
  __vector_columns.resize(numcols + 1);
  
  for(int i = 1;i <= numcols;++i)
  {
    struct sqlvar_struct* p_ec_column = &(__mp_ec_sqlda_columns->sqlvar[i]);

    __vector_columns[i].__mi_index = i;

    __vector_columns[i].__mi_type = p_ec_column->sqltype;
    __vector_columns[i].__mi_xtype = p_ec_column->sqlxid;
    __vector_columns[i].__mi_length = p_ec_column->sqllen;

    if(   p_ec_column->sqltype == SQLCHAR 
       || p_ec_column->sqltype == SQLNCHAR 
       || p_ec_column->sqltype == SQLVCHAR
       || p_ec_column->sqltype == SQLNVCHAR)
    {
      p_ec_column->sqllen = rtypmsize(p_ec_column->sqltype,p_ec_column->sqllen); 
    }

    if(p_ec_column->sqlname != 0)
    {
      __vector_columns[i].__ms_name = p_ec_column->sqlname;
    }

    if(p_ec_column->sqlownername != 0)
    {
      __vector_columns[i].__ms_schema_name = p_ec_column->sqlownername;
    }

    if(p_ec_column->sqltypename != 0)
    {
      __vector_columns[i].__ms_type_name = p_ec_column->sqltypename;
      __vector_columns[i].__ms_type_name = StringsKit::toUpperCase(__vector_columns[i].__ms_type_name);
    }
    else
    {
      __vector_columns[i].__ms_type_name = IfxTypes::getInstance().getTypeName(p_ec_column->sqltype,false);
    }
    
  }//~end for(int i = 1;i <= numcols;++i)
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
void IfxConversion::prepareColumnData(__XDBC_HOOK_DECLARE_MORE  struct sqlda*  __mp_ec_sqlda_columns,
                              const vector<IfxColumn>& __vector_columns,
                              vector<IfxDefine>& __vector_defines,
                              char*& p_data_buf,int& __mi_buf_size,
                              IfxResultSetCollection* __cachedCollection,
                              bool __adoptIt)  __XDBC_THROW_DECLARE(SQLException)
{
  int numcols = __mp_ec_sqlda_columns->sqld;
  
  if(!__adoptIt)
  {
    memset(__cachedCollection->__p_rowbuf,0,__cachedCollection->__rowbuf_size);
  }
    
  if(__adoptIt)
  {
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  
    //-------------------------------------------------------------------------
    // 1. allocate row data buffer 
    //-------------------------------------------------------------------------
    __vector_defines.resize(numcols + 1);
    int offset=0;
    
    for(int i = 1;i <= numcols;++i)
    {
      IfxDefine& ifx_define = __vector_defines[i];
      ifx_define.__mi_index=__vector_columns[i].__mi_index;
  
      int col_type = __vector_columns[i].__mi_type & SQLTYPE;
      int col_length = __vector_columns[i].__mi_length;
    
      ifx_define.int_buf_offset = offset;
      ifx_define.__mi_buf_size = 0;
      
      switch (col_type)
      {
        case SQLCHAR:
        case SQLNCHAR:
        case SQLVCHAR:
        case SQLNVCHAR:
        {
          ifx_define.__mi_buf_size = col_length;
          break;
        }
//--------------------------------------------------------------------------
//   BEGIN - NOT USE BUFFER
//--------------------------------------------------------------------------        
        case SQLTEXT:
        case SQLBYTES:
        {
          ifx_define.ifx_value.ec_locator.loc_loctype = LOCMEMORY;     /* set loctype for in memory */
          ifx_define.ifx_value.ec_locator.loc_bufsize = -1;            /* let db get buffer */
          ifx_define.ifx_value.ec_locator.loc_oflags = 0;              /* clear loc_oflags */
          ifx_define.ifx_value.ec_locator.loc_mflags = 0;
  
          break;
        } 
//--------------------------------------------------------------------------
//   END - NOT USE BUFFER
//--------------------------------------------------------------------------        
      }//~end switch (p_ec_column->sqltype & SQLTYPE)
  
      ifx_define.__mi_define_size = ifx_define.__mi_buf_size;
      ifx_define.__mi_define_ind  = 0;
      ifx_define.__mi_define_code = 0;
  
      int __addr_alignment = MemoryKit::roundUp(ifx_define.__mi_buf_size,sizeof(void*));
      offset += __addr_alignment;
      __mi_buf_size += __addr_alignment;
  
    }//~end for(int i = 1;i <= numcols;++i)
  
    p_data_buf = new char[__mi_buf_size];
    memset(p_data_buf,0,__mi_buf_size);
    
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  }
  
  //-------------------------------------------------------------------------
  // 2.define data by position
  //-------------------------------------------------------------------------
  for(int i = 1;i <= numcols;++i)
  {
    int col_type = __vector_columns[i].__mi_type & SQLTYPE;

    char* buf_start = p_data_buf + __vector_defines[i].int_buf_offset;
    int buf_size = __vector_defines[i].__mi_buf_size;

    //cout<<i<<"--"<<(void*)buf_start<<" || "<<buf_size<<endl;
    struct sqlvar_struct* p_ec_column = &(__mp_ec_sqlda_columns->sqlvar[i]);
    
    p_ec_column->sqlind = (int2*)&(__vector_defines[i].__mi_define_ind);
    p_ec_column->sqlilen = sizeof(__vector_defines[i].__mi_define_ind);
    p_ec_column->sqlitype = SQLSMINT;
    
    p_ec_column->sqldata = buf_start;
    
    switch (col_type)
    {
//--------------------------------------------------------------------------
//   BEGIN - NOT USE BUFFER
//--------------------------------------------------------------------------
      case SQLSMINT:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_SMINT);
        break;
      }
      
      case SQLINT:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_INT);
        break;
      } 
      
      case SQLSERIAL:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_SERIAL);
        break;
      } 

      case SQLSMFLOAT:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_FLOAT);
        break;
      }

      case SQLFLOAT:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_DOUBLE);
        break;
      }
                       
      case SQLINT8:
      case SQLSERIAL8:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_int8);
        break;
      }

      case SQLDECIMAL:
      case SQLMONEY:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_decimal);
        break;
      }

      case SQLUDTFIXED: //SQLUDTFIXED(BOLB,CLOB)
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_lo_ts);
        break;
      }

      case SQLTEXT:
      case SQLBYTES:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_locator);
        break;
      }

      case SQLDATE:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_INT);
        break;
      }
      
      case SQLDTIME:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_dtime);
        break;
      }

      case SQLINTERVAL:
      {
        p_ec_column->sqldata = (char*)&(__vector_defines[i].ifx_value.ec_interval);
        break;
      }
//--------------------------------------------------------------------------
//   END - NOT USE BUFFER
//--------------------------------------------------------------------------

    }//~end switch (p_ec_column->sqltype & SQLTYPE)

  }//~end for(int i=0;i<numcols;++i)  
}


void  IfxConversion::prepareVariableData(__XDBC_HOOK_DECLARE_MORE IfxConnection* __mp_conn,struct sqlda*  __mp_ec_sqlda_vars,const vector<IfxColumn>& __vector_vars,vector<IfxBind>& __vector_binds,char*& p_data_buf,int& __mi_buf_size)  __XDBC_THROW_DECLARE(SQLException)
{
  int numvars = __mp_ec_sqlda_vars->sqld;
  __vector_binds.resize(numvars + 1);
  
  //-------------------------------------------------------------------------
  // 1. allocate row data buffer 
  //-------------------------------------------------------------------------
  int offset = 0;
  for(int i = 1;i <= numvars;++i)
  {
    IfxBind& ifx_bind = __vector_binds[i];
    ifx_bind.int_parameter_index=__vector_vars[i].__mi_index;

    int col_type = __vector_vars[i].__mi_type & SQLTYPE;
    int col_length = __vector_vars[i].__mi_length;

    ifx_bind.int_buf_offset = offset;
    ifx_bind.__mi_buf_size = 0;
    
    switch (col_type)
    {
      case SQLCHAR:
      case SQLVCHAR:
      case SQLNCHAR:
      case SQLNVCHAR:
      {
         /* Allow for the trailing null character in C*/
        ifx_bind.__mi_buf_size = col_length + 1; 
        break;
      }

//--------------------------------------------------------------------------
//   BEGIN - NOT USE BUFFER
//--------------------------------------------------------------------------
      case SQLUDTFIXED: //SQLUDTFIXED(BOLB,CLOB)
      {
        int error = 0;
 
        ifx_lo_def_create_spec(&(ifx_bind.ifx_value.p_ec_create_spec));
        IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONVERSION_CLASS_NAME);
        __XDBC_HOOK_CHECK(return);
        
        if(   (!__vector_vars[i].__ms_table_name.empty() )
           || (!__vector_vars[i].__ms_name.empty() ))
        {
          string col_name;
          string database;
          string server;

          __XDBC_ATTRIBUTES::iterator __pos = __mp_conn->__mo_attributes.find("informix.database");
          if(__pos != __mp_conn->__mo_attributes.end())
          {
            database = __pos->second;
          }
        
          __pos = __mp_conn->__mo_attributes.find("informix.server");
          if(__pos != __mp_conn->__mo_attributes.end())
          {
            server = __pos->second;
          }  

          col_name.append(database);
          col_name.append("@");
          col_name.append(server);
          col_name.append(":");
          col_name.append(__vector_vars[i].__ms_table_name);
          col_name.append(".");
          col_name.append(__vector_vars[i].__ms_name);
        
          //printf("=========col_name:[%s]\n",col_name.c_str()); 
          ifx_lo_col_info((char*)col_name.c_str(), ifx_bind.ifx_value.p_ec_create_spec);
          IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONVERSION_CLASS_NAME);
          __XDBC_HOOK_CHECK(return);
        }
        else
        {
          if(DriverManager::isTracingOn)
          {
            StringBuffer __sb;
            __sb.append("prepareVariableData|illegal argument. tablename=");
            __sb.append(__vector_vars[i].__ms_table_name);
            __sb.append(", columnname=");
            __sb.append(__vector_vars[i].__ms_name);
            
            __XDBC_FORCED_TRACE(IFXCONVERSION_CLASS_NAME,__sb.str());
          }
        }

        ifx_bind.ifx_value.int_ec_lofd = ifx_lo_create(ifx_bind.ifx_value.p_ec_create_spec, LO_WRONLY ,&(ifx_bind.ifx_value.ec_lo_ts), &error);
        IfxConversion::checkException(__XDBC_HOOK_INVOKE_MORE SQLCODE,SQLSTATE,IFXCONVERSION_CLASS_NAME);
        __XDBC_HOOK_CHECK(return);
        break;
      }

      case SQLTEXT:
      case SQLBYTES:
      {
        ifx_bind.ifx_value.ec_locator.loc_loctype = LOCMEMORY; 

        break;
      }

      case SQLDTIME:
      {
        ifx_bind.ifx_value.ec_dtime.dt_qual = __vector_vars[i].__mi_length;
        break;
      }

      case SQLINTERVAL:
      {
        ifx_bind.ifx_value.ec_interval.in_qual = __vector_vars[i].__mi_length;
        break;
      }
//--------------------------------------------------------------------------
//   END - NOT USE BUFFER
//--------------------------------------------------------------------------
    }//~end switch (p_ec_var->sqltype & SQLTYPE)

    ifx_bind.__mi_bind_size = ifx_bind.__mi_buf_size;
    ifx_bind.__mi_bind_ind = 0;
    ifx_bind.__mi_bind_code = 0;

    offset += ifx_bind.__mi_buf_size;
    __mi_buf_size += ifx_bind.__mi_buf_size;

  }//~end for(int i = 1;i <= numvars;++i)

  p_data_buf = new char[__mi_buf_size];
  memset(p_data_buf,0,__mi_buf_size);

  //-------------------------------------------------------------------------
  // 2.bind data by position
  //-------------------------------------------------------------------------
  for(int i = 1;i <= numvars;++i)
  {    
    int col_type = __vector_vars[i].__mi_type & SQLTYPE;

    char* buf_start = p_data_buf+__vector_binds[i].int_buf_offset;
    int buf_size = __vector_binds[i].__mi_buf_size;

    struct sqlvar_struct* p_ec_var = &(__mp_ec_sqlda_vars->sqlvar[i]);

    p_ec_var->sqlind = (int2*)&(__vector_binds[i].__mi_bind_ind);
    p_ec_var->sqlilen = sizeof(__vector_binds[i].__mi_bind_ind);
    p_ec_var->sqlitype = SQLSMINT;

    p_ec_var->sqllen  = buf_size;
    p_ec_var->sqldata = buf_start;
    
    switch (col_type)
    {
//--------------------------------------------------------------------------
//   BEGIN - NOT USE BUFFER
//--------------------------------------------------------------------------
      case SQLSMINT:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_SMINT);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_SMINT);
        break;
      }
      
      case SQLINT:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_INT);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_INT);
        break;
      } 

      case SQLSERIAL:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_SERIAL);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_SERIAL);
        break;
      } 
      
      case SQLSMFLOAT:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_FLOAT);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_FLOAT);
        break;
      }

      case SQLFLOAT:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_DOUBLE);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_DOUBLE);
        break;
      }

      case SQLSERIAL8:
      case SQLINT8:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_int8);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_int8);
        break;
      }
            
      case SQLDECIMAL:
      case SQLMONEY:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_decimal);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_decimal);
        break;
      }

      case SQLUDTFIXED: //SQLUDTFIXED(BOLB,CLOB)
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_lo_ts);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_lo_ts);
        break;
      }

      case SQLTEXT:
      case SQLBYTES:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_locator);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_locator);

        break;
      }
      case SQLDATE:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_INT);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_INT);
        break;
      }
      
      case SQLDTIME:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_dtime);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_dtime);
        break;
      }

      case SQLINTERVAL:
      {
        p_ec_var->sqldata = (char*)&(__vector_binds[i].ifx_value.ec_interval);
        p_ec_var->sqllen =  sizeof(__vector_binds[i].ifx_value.ec_interval);
        break;
      }
//--------------------------------------------------------------------------
//   END - NOT USE BUFFER
//--------------------------------------------------------------------------
    }//~end switch (p_ec_var->sqltype & SQLTYPE)

  }//~end for(int i = 1;i <= numvars;++i)
}

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
bool IfxConversion::checkConversion(__XDBC_HOOK_DECLARE_MORE const string& className,char* rowBuffer,IfxDefine& ifx_define,int inType,int outType,char* outBuffer,const string& columnName) __XDBC_THROW_DECLARE(SQLException)
{

  if(inType == outType)
  {
    return false;
  }

  char* buf_start = rowBuffer + ifx_define.int_buf_offset;
  int   buf_size  = ifx_define.__mi_define_size;

  switch(inType)
  {
    case SQLSMINT:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;
        *__p_string = Number::toString(ifx_define.ifx_value.ec_SMINT);

        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        *__p_int = (int)(ifx_define.ifx_value.ec_SMINT);

        return true;
      }

      if( (outType == SQLINT8) || (outType == SQLSERIAL8))
      {
        XLONG* __p_int8 = (XLONG*)outBuffer;
        *__p_int8 = (XLONG)(ifx_define.ifx_value.ec_SMINT);

        return true;
      }

      if( (outType == SQLDECIMAL) || (outType == SQLFLOAT))
      {
        double* __p_double = (double*)outBuffer;
        *__p_double = (double)(ifx_define.ifx_value.ec_SMINT);
        
        return true;
      }
    }
    break;
//----------------------------------
    case SQLINT:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;
        *__p_string = Number::toString((int)ifx_define.ifx_value.ec_INT);
        
        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;
        *__p_short  = (short)(ifx_define.ifx_value.ec_INT);
        
        return true;
      }

      if( (outType == SQLINT8) || (outType == SQLSERIAL8))
      {
        XLONG* __p_int8 = (XLONG*)outBuffer;
        *__p_int8 = (XLONG)(ifx_define.ifx_value.ec_INT);

        return true;
      }

      if( (outType == SQLDECIMAL) || (outType == SQLFLOAT))
      {
        double* __p_double = (double*)outBuffer;
        *__p_double = (double)(ifx_define.ifx_value.ec_INT);
        
        return true;
      }
    }

    break;
//----------------------------------
    case SQLSERIAL:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;
        *__p_string = Number::toString(ifx_define.ifx_value.ec_SERIAL);

        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;        
        *__p_short  = (short)(ifx_define.ifx_value.ec_SERIAL);
        
        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        *__p_int  = (int)(ifx_define.ifx_value.ec_SERIAL);
        
        return true;
      }

      if(outType == SQLDECIMAL || outType == SQLFLOAT)
      {
        double* __p_double = (double*)outBuffer;
        *__p_double = (double)(ifx_define.ifx_value.ec_SERIAL);
        return true;
      }
    }

    break;
//----------------------------------
    case SQLINT8:
    case SQLSERIAL8:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;
        int8ToString(&(ifx_define.ifx_value.ec_int8),*__p_string);

        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;
        int int_temp = 0;
        ifx_int8toint(&(ifx_define.ifx_value.ec_int8), &int_temp);
        *__p_short = (short)int_temp;
        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        ifx_int8toint(&(ifx_define.ifx_value.ec_int8), __p_int);
        return true;
      }

      if(outType == SQLDECIMAL || outType == SQLFLOAT)
      {
        double* __p_double = (double*)outBuffer;
        ifx_int8todbl(&(ifx_define.ifx_value.ec_int8), __p_double);
        return true;
      }
    }
    break;
//----------------------------------
    case SQLSMFLOAT:
    {
      float float_temp = 0;

      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;
        *__p_string = Number::toString(ifx_define.ifx_value.ec_FLOAT);

        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;
        *__p_short  = (short)(ifx_define.ifx_value.ec_FLOAT);
        
        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        *__p_int  = (int)(ifx_define.ifx_value.ec_FLOAT);
        return true;
      }

      if( (outType == SQLINT8) || (outType == SQLSERIAL8))
      {
        XLONG* __p_int8 = (XLONG*)outBuffer;
        *__p_int8 = (XLONG)(ifx_define.ifx_value.ec_FLOAT);

        return true;
      }

      if(outType == SQLDECIMAL || outType == SQLFLOAT)
      {
        double* __p_double = (double*)outBuffer;
        *__p_double = (double)(ifx_define.ifx_value.ec_FLOAT);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLFLOAT:
    {
      double dbl_temp = 0;

      if(outType == SQLVCHAR)
      {
        string* __p_string = (string*)outBuffer;     
        *__p_string = Number::toString(ifx_define.ifx_value.ec_DOUBLE);

        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;
        *__p_short  = (short)(ifx_define.ifx_value.ec_DOUBLE);
        
        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        *__p_int  = (int)(ifx_define.ifx_value.ec_DOUBLE);
        
        return true;
      }

      if( (outType == SQLINT8) || (outType == SQLSERIAL8))
      {
        XLONG* __p_int8 = (XLONG*)outBuffer;
        *__p_int8 = (XLONG)(ifx_define.ifx_value.ec_DOUBLE);

        return true;
      }

      if(outType == SQLDECIMAL)
      {
        double* __p_double = (double*)outBuffer;
        *__p_double = (double)(ifx_define.ifx_value.ec_DOUBLE);
        
        return true;
      }
    }
    break;

//----------------------------------
    case SQLDECIMAL:
    case SQLMONEY: 
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string=(string*)outBuffer;
        decimalToString(&(ifx_define.ifx_value.ec_decimal),*__p_string);
        
        return true;
      }

      if(outType == SQLSMINT)
      {
        short* __p_short = (short*)outBuffer;
        int int_temp = 0;
        dectoint(&(ifx_define.ifx_value.ec_decimal), &int_temp);
        *__p_short = (short)int_temp;
        
        return true;
      }

      if(outType == SQLINT)
      {
        int* __p_int = (int*)outBuffer;
        dectoint(&(ifx_define.ifx_value.ec_decimal), __p_int);
        
        return true;
      }

      if( (outType == SQLINT8) || (outType == SQLSERIAL8))
      {
        XLONG* __p_int8 = (XLONG*)outBuffer;
        decimalToLong(&(ifx_define.ifx_value.ec_decimal),*__p_int8);
        
        return true;
      }

      if(outType == SQLFLOAT)
      {
        double* __p_double = (double*)outBuffer;
        dectodbl(&(ifx_define.ifx_value.ec_decimal), __p_double);
        
        return true;
      }

      if(outType == SQLDECIMAL || outType == SQLMONEY)
      {
        double* __p_double = (double*)outBuffer;
        dectodbl(&(ifx_define.ifx_value.ec_decimal), __p_double);
        
        return true;
      }

    }
    break;

//----------------------------------
    case SQLCHAR: 
    case SQLVCHAR:
    case SQLNCHAR:
    case SQLNVCHAR:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string=(string*)outBuffer;

        if(*(buf_start + buf_size - 1) == '\0')
        {
          __p_string->assign(buf_start);
        }
        else
        {
          __p_string->assign(buf_start,buf_size);
        }

        return true;
      }
    }
    break;

//----------------------------------
    case SQLUDTFIXED:
    {
      ifx_int8_t size ;
      ifx_int8_t offset ;
      ifx_lo_stat_t *stats =0;

      int isize = 0;
      int error = 0;
      int lofd  = -1;

      if(outType == SQLVCHAR)
      {
        string* __p_string=(string*)outBuffer;

        lofd =  ifx_lo_open(&(ifx_define.ifx_value.ec_lo_ts), LO_RDONLY, &error);
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

        if(isize > 0)
        {
          string str_x(isize,'\0');
          ifx_int8cvint(0, &offset);
          ifx_lo_readwithseek(lofd, (char*)str_x.data(), isize, &offset, LO_SEEK_CUR, &error);
          __p_string->swap(str_x);
        }
       
        ifx_lo_close(lofd);
        return true;
      }
    }
    break;

//----------------------------------
    case SQLTEXT:
    case SQLBYTES:
    {
      if(outType == SQLVCHAR)
      {
        string* __p_string=(string*)outBuffer;
        struct tag_loc_t* p_ec_locator = &(ifx_define.ifx_value.ec_locator); 
        __p_string->assign(p_ec_locator->loc_buffer,p_ec_locator->loc_size);  
        return true;
      }      
    }
    break;
//----------------------------------
   case SQLDATE:
   {
     if(outType == SQLVCHAR)
     {
       string* __p_string = (string*)outBuffer;
       
       short mdy_array[3];
       rjulmdy(ifx_define.ifx_value.ec_INT, mdy_array);
       
       XDBC_DATE __sql_date;
       __sql_date.year = mdy_array[2];
       __sql_date.month = mdy_array[0];
       __sql_date.day = mdy_array[1];
       *__p_string = XDBCUtilities::toString(__sql_date);
       
       return true;
     }
     else if(outType == SQLDTIME)
     {
       XDBC_TIMESTAMP* __p_timestamp = (XDBC_TIMESTAMP*)outBuffer;

       short mdy_array[3];
       rjulmdy(ifx_define.ifx_value.ec_INT, mdy_array);
       
       __p_timestamp->year = mdy_array[2];
       __p_timestamp->month = mdy_array[0];
       __p_timestamp->day = mdy_array[1];

       return true;
     }
 
   }
   break;

//----------------------------------
   case SQLDTIME:
   {
     if(outType == SQLVCHAR)
     {
       string* __p_string = (string*)outBuffer;
       *__p_string = IfxConversion::toString(&(ifx_define.ifx_value.ec_dtime)); 
       
       return true;
     }      

     if(outType == SQLDATE)
     {
       XDBC_DATE* __p_date = (XDBC_DATE*)outBuffer;
       
       XDBC_TIMESTAMP __sql_timestamp;
       IfxConversion::parseDateTime(&(ifx_define.ifx_value.ec_dtime),__sql_timestamp);
       
       __p_date->year  = __sql_timestamp.year;
       __p_date->month = __sql_timestamp.month;
       __p_date->day   = __sql_timestamp.day;

       return true;
     }
   }
   break;
//----------------------------------
   case SQLINTERVAL:
   {
     if(outType == SQLVCHAR)
     {
       string* __p_string=(string*)outBuffer;
       *__p_string = IfxConversion::toString(&(ifx_define.ifx_value.ec_interval));
       
       return true;
     }
   }
   break;

//----------------------------------
   default:
   {
     break;
   }
  }

  StringBuffer __sb("[");
  __sb.append(className);
  __sb.append("] Conversion not supported. columnName:");
  __sb.append("(");
  __sb.append(columnName);
  __sb.append("),sourceSQLType:");
  __sb.append(IfxTypes::getInstance().getTypeName(inType,false));
  __sb.append("(");
  __sb.append(inType);
  __sb.append("),targetSQLType:");
  __sb.append(IfxTypes::getInstance().getTypeName(outType,false));
  __sb.append("(");
  __sb.append(outType);
  __sb.append(")");
  __XDBC_HOOK_RETURN(return false,__sb.str());
}

void IfxConversion::checkException(__XDBC_HOOK_DECLARE_MORE int sql_code,const string& sql_state,const string& className) __XDBC_THROW_DECLARE(SQLException)
{
  if(sql_code < 0)
  {
    char errFormat[1024] = {0};
    int msg_len = 0;

    rgetlmsg(sql_code, errFormat, sizeof(errFormat), &msg_len);
    errFormat[1023]='\0';

    StringBuffer __sb;
    __sb.append("[");
    __sb.append(className);
    __sb.append("] ");
    
    char __errMsg[1024] = {0};
    sprintf(__errMsg,errFormat,sqlca.sqlerrm);
    __sb.append(__errMsg);
    __XDBC_HOOK_RETURN(return,__sb.str(),sql_state,sql_code);
  }
}

string IfxConversion::toString(struct dtime* p_dtime)
{
  char buf[40] ={0};
  dttoasc(p_dtime, buf);

  return string(buf);
}

string IfxConversion::toString(struct intrvl* p_interval)
{
  char buf[40] ={0};
  intoasc(p_interval, buf);
  return string(buf);
}


void  IfxConversion::formatDateTimeInterval(const vector<int>& fields,int qual,char* buf)
{
  int start_field = TU_START(qual);
  int end_field = TU_END(qual);

  switch(start_field)
  {
    case TU_YEAR:
    {
      if(end_field == TU_YEAR)
      {
        sprintf(buf, "%d",fields[0]);
      }
      else if (end_field == TU_MONTH)
      {
        sprintf(buf, "%d-%d",fields[0],fields[1]);
      }
      else if (end_field == TU_DAY)
      {
        sprintf(buf, "%d-%d-%d",fields[0],fields[1],fields[2]);
      }
      else if(end_field == TU_HOUR)
      {
        sprintf(buf, "%d-%d-%d %d",fields[0],fields[1],fields[2],fields[3]);
      }
      else if(end_field == TU_MINUTE)
      {
        sprintf(buf, "%d-%d-%d %d:%d",fields[0],fields[1],fields[2],fields[3],fields[4]);
      }
      else if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d-%d-%d %d:%d:%d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d-%d-%d %d:%d:%d.%01d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d-%d-%d %d:%d:%d.%02d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d-%d-%d %d:%d:%d.%03d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d-%d-%d %d:%d:%d.%04d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d-%d-%d %d:%d:%d.%05d",fields[0],fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000);
        }
      }      
    }
    break;

    case TU_MONTH:
    {
      if (end_field == TU_MONTH)
      {
        sprintf(buf, "%d",fields[1]);
      }
      else if (end_field == TU_DAY)
      {
        sprintf(buf, "%%d-%d",fields[1],fields[2]);
      }
      else if(end_field == TU_HOUR)
      {
        sprintf(buf, "%d-%d %d",fields[1],fields[2],fields[3]);
      }
      else if(end_field == TU_MINUTE)
      {
        sprintf(buf, "%d-%d %d:%d",fields[1],fields[2],fields[3],fields[4]);
      }
      else if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d-%d %d:%d:%d",fields[1],fields[2],fields[3],fields[4],fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d-%d %d:%d:%d.%01d",fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d-%d %d:%d:%d.%02d",fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d-%d %d:%d:%d.%03d",fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d-%d %d:%d:%d.%04d",fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d-%d %d:%d:%d.%05d",fields[1],fields[2],fields[3],fields[4],fields[5],fields[6]/10000);
        }
      }      
    }
    break;

    case TU_DAY:
    {
      if(end_field == TU_DAY)
      {
        sprintf(buf, "%d",fields[2]);
      }
      else if(end_field == TU_HOUR)
      {
        sprintf(buf, "%d %d",fields[2],fields[3]);
      }
      else if(end_field == TU_MINUTE)
      {
        sprintf(buf, "%d %d:%d",fields[2],fields[3],fields[4]);
      }
      else if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d %d:%d:%d",fields[2],fields[3],fields[4],fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d %d:%d:%d.%01d",fields[2],fields[3],fields[4],fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d %d:%d:%d.%02d",fields[2],fields[3],fields[4],fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d %d:%d:%d.%03d",fields[2],fields[3],fields[4],fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d %d:%d:%d.%04d",fields[2],fields[3],fields[4],fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d %d:%d:%d.%05d",fields[2],fields[3],fields[4],fields[5],fields[6]/10000);
        }
      }
    }
    break;

    case TU_HOUR:
    {
      if(end_field == TU_HOUR)
      {
        sprintf(buf, "%d",fields[3]);
      }
      else if(end_field == TU_MINUTE)
      {
        sprintf(buf, "%d:%d",fields[3],fields[4]);
      }
      else if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d:%d:%d",fields[3],fields[4],fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d:%d:%d.%01d",fields[3],fields[4],fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d:%d:%d.%02d",fields[3],fields[4],fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d:%d:%d.%03d",fields[3],fields[4],fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d:%d:%d.%04d",fields[3],fields[4],fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d:%d:%d.%05d",fields[3],fields[4],fields[5],fields[6]/10000);
        }
      }
    }
    break;

    case TU_MINUTE:
    {
      if(end_field == TU_MINUTE)
      {
        sprintf(buf, "%d",fields[4]);
      }
      else if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d:%d",fields[4],fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d:%d.%01d",fields[4],fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d:%d.%02d",fields[4],fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d:%d.%03d",fields[4],fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d:%d.%04d",fields[4],fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d:%d.%05d",fields[4],fields[5],fields[6]/10000);
        }
      }
    }
    break;

    case TU_SECOND:
    {
      if(end_field == TU_SECOND)
      {
        sprintf(buf, "%d",fields[5]);
      }
      else
      {
        if(end_field == TU_F1)
        {
          sprintf(buf, "%d.%01d",fields[5],fields[6]/100000000);
        }
        else if(end_field == TU_F2)
        {
          sprintf(buf, "%d.%02d",fields[5],fields[6]/10000000);
        }
        else if(end_field == TU_F3)
        {
          sprintf(buf, "%d.%03d",fields[5],fields[6]/1000000);
        }
        else if(end_field == TU_F4)
        {
          sprintf(buf, "%d.%04d",fields[5],fields[6]/100000);
        }
        else if(end_field == TU_F5)
        {
          sprintf(buf, "%d.%05d",fields[5],fields[6]/10000);
        }
      }
    }
    break;

    case TU_FRAC:
    {
      if(end_field == TU_F1)
      {
        sprintf(buf, ".%01d",fields[6]/100000000);
      }
      else if(end_field == TU_F2)
      {
        sprintf(buf, ".%02d",fields[6]/10000000);
      }
      else if(end_field == TU_F3)
      {
        sprintf(buf, ".%03d",fields[6]/1000000);
      }
      else if(end_field == TU_F4)
      {
        sprintf(buf, ".%04d",fields[6]/100000);
      }
      else if(end_field == TU_F5)
      {
        sprintf(buf, ".%05d",fields[6]/10000);
      }
    }
    break;

    default:
    {
    }
    break;

  }//~end switch
}

void  IfxConversion::fixDateTimeInterval(vector<int>& fields,int qual,const vector<int>& fvalues)
{
  int start_field = TU_START(qual);
  int end_field = TU_END(qual);

  switch(start_field)
  {
    case TU_YEAR:
    {
      if(end_field == TU_YEAR)
      {
        fields[0] = fields[2];
        fields[2] = fvalues[2];
      }
      else if (end_field == TU_MONTH)
      {
        fields[0] = fields[1];
        fields[1] = fields[2];
        fields[2] = fvalues[2]; 
      }
    }
    break;

    case TU_MONTH:
    {
      if (end_field == TU_MONTH)
      {
        fields[1] = fields[2];
        fields[2] = fvalues[2];
      }
    }
    break;
    
    case TU_HOUR:
    {
      if(end_field == TU_HOUR)
      {
        fields[3] = fields[2];
        fields[2] = fvalues[2];
      }
    }
    break;

    case TU_MINUTE:
    {
      if(end_field == TU_MINUTE)
      {
        fields[4] = fields[2];
        fields[2] = fvalues[2];
      }
      else if(end_field == TU_SECOND)
      {
        fields[5] = fields[4];
        fields[4] = fields[3];
        fields[3] = fvalues[3];
      }      
    }
    break;

    case TU_SECOND:
    {
      if(end_field == TU_SECOND)
      {
        fields[5] = fields[2]; 
        fields[2] = fvalues[2];
      }
    }
    break;

    case TU_FRAC:
    {
      fields[6] = fields[2];
      fields[2] = fvalues[2];
    }
    break;

    default:
    {
    }
    break;
  }
}

__OPENCODE_END_NAMESPACE
