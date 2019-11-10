#include "xdbc/ifxdefs.h"
#include "port/file.h"

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------
//------------------------------------------------------
IfxParameter::IfxParameter()
{
  __mi_index=-1;

  __mi_length=0;
  __mi_type=0;
  __mi_precision=0;
  __mi_scale=0;

  __mi_iomode=0;
}

IfxParameter::IfxParameter(const IfxParameter& rv)
{
  __mi_index=rv.__mi_index;

  __mi_length=rv.__mi_length;
  __mi_type=rv.__mi_type;
  __mi_precision=rv.__mi_precision;
  __mi_scale=rv.__mi_scale;

  __ms_name=rv.__ms_name;
  __ms_schema_name=rv.__ms_schema_name;
  __ms_type_name=rv.__ms_type_name;

  __mi_iomode=rv.__mi_iomode;

}

IfxParameter& IfxParameter::operator=(const IfxParameter& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index=rv.__mi_index;

  __mi_length=rv.__mi_length;
  __mi_type=rv.__mi_type;
  __mi_precision=rv.__mi_precision;
  __mi_scale=rv.__mi_scale;

  __ms_name=rv.__ms_name;
  __ms_schema_name=rv.__ms_schema_name;
  __ms_type_name=rv.__ms_type_name;

  __mi_iomode=rv.__mi_iomode;

  return *this;
}

void IfxParameter::dump(StringBuffer& __sb)
{
  __sb<<"----------------------------------"<<File::lineSeparator;
  __sb<<"__mi_index:"<<__mi_index<<File::lineSeparator;


  __sb<<"__mi_length:"<<__mi_length<<File::lineSeparator;
  __sb<<"__mi_type:"<<__mi_type<<File::lineSeparator;
  __sb<<"__mi_precision:"<<__mi_precision<<File::lineSeparator;
  __sb<<"__mi_scale:"<<__mi_scale<<File::lineSeparator;

  __sb<<"__mi_iomode:"<<__mi_iomode<<File::lineSeparator;

  __sb<<"__ms_name:"<<__ms_name<<File::lineSeparator;
  __sb<<"__ms_schema_name:"<<__ms_schema_name<<File::lineSeparator;
  __sb<<"__ms_type_name:"<<__ms_type_name<<File::lineSeparator;

}

//---------------------------------------------------
// IfxColumn Class 
//---------------------------------------------------

IfxColumn::IfxColumn()
{
  __mi_index=-1;

  __mi_length=0;
  __mi_type=0;
  __mi_xtype=0;
  __mi_precision=0;
  __mi_scale=0;
  
  __mb_char_semantics=false;

  __mi_nullable=1;
}

IfxColumn::IfxColumn(const IfxColumn& rv)
{
  __mi_index=rv.__mi_index;

  __mi_length=rv.__mi_length;
  __mi_type=rv.__mi_type;
  __mi_xtype=rv.__mi_xtype;
  __mi_precision=rv.__mi_precision;
  __mi_scale=rv.__mi_scale;

  __ms_name=rv.__ms_name;
  __ms_schema_name=rv.__ms_schema_name;
  __ms_type_name=rv.__ms_type_name;  

  __mb_char_semantics=rv.__mb_char_semantics;

  __mi_nullable=rv.__mi_nullable;
  __ms_table_name=rv.__ms_table_name;
}

IfxColumn& IfxColumn::operator=(const IfxColumn& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index=rv.__mi_index;

  __mi_length=rv.__mi_length;
  __mi_type=rv.__mi_type;
  __mi_xtype=rv.__mi_xtype;
  __mi_precision=rv.__mi_precision;
  __mi_scale=rv.__mi_scale;

  __ms_name=rv.__ms_name;
  __ms_schema_name=rv.__ms_schema_name;
  __ms_type_name=rv.__ms_type_name;

  __mb_char_semantics=rv.__mb_char_semantics;

  __mi_nullable=rv.__mi_nullable;
  __ms_table_name=rv.__ms_table_name;

  return *this;
}

void IfxColumn::dump(StringBuffer& __sb)
{
  __sb<<"----------------------------------"<<File::lineSeparator;
  __sb<<"__mi_index:"<<__mi_index<<File::lineSeparator;


  __sb<<"__mi_length:"<<__mi_length<<File::lineSeparator;
  __sb<<"__mi_type:"<<__mi_type<<File::lineSeparator;
  __sb<<"__mi_xtype:"<<__mi_xtype<<File::lineSeparator;
  __sb<<"__mi_precision:"<<__mi_precision<<File::lineSeparator;
  __sb<<"__mi_scale:"<<__mi_scale<<File::lineSeparator;

  __sb<<"__mb_char_semantics:"<<__mb_char_semantics<<File::lineSeparator;
 
  __sb<<"__ms_name:"<<__ms_name<<File::lineSeparator;
  __sb<<"__ms_schema_name:"<<__ms_schema_name<<File::lineSeparator;
  __sb<<"__ms_table_name:"<<__ms_table_name<<File::lineSeparator;
  __sb<<"__ms_type_name:"<<__ms_type_name<<File::lineSeparator;

}

//---------------------------------------------------
//---------------------------------------------------
IfxValue::IfxValue()
{
  memset(&ec_decimal,0,sizeof(ec_decimal));
  memset(&ec_locator,0,sizeof(ec_locator));  
  memset(&ec_dtime,0,sizeof(ec_dtime));
  memset(&ec_interval,0,sizeof(ec_interval));
  memset(&ec_int8,0,sizeof(ec_int8));

  memset(&ec_lo_ts,0,sizeof(ec_lo_ts));
  p_ec_create_spec = 0;
  int_ec_lofd = -1;
  
  ec_SERIAL = 0;
}

IfxValue::IfxValue(const IfxValue& rv)
{
  ec_decimal = rv.ec_decimal;  
  ec_locator = rv.ec_locator;
  ec_dtime = rv.ec_dtime;
  ec_interval = rv.ec_interval;
  
  ec_lo_ts = rv.ec_lo_ts;
  p_ec_create_spec = rv.p_ec_create_spec;
  int_ec_lofd = rv.int_ec_lofd;

  ec_SERIAL = rv.ec_SERIAL;
}

IfxValue& IfxValue::operator=(const IfxValue& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  ec_decimal = rv.ec_decimal;  
  ec_locator = rv.ec_locator;
  ec_dtime = rv.ec_dtime;
  ec_interval = rv.ec_interval;
  
  ec_lo_ts = rv.ec_lo_ts;
  p_ec_create_spec = rv.p_ec_create_spec;
  int_ec_lofd = rv.int_ec_lofd;

  ec_SERIAL = rv.ec_SERIAL;

  return *this;
}

//---------------------------------------------------
//---------------------------------------------------
IfxBind::IfxBind()
{
  int_parameter_index=-1;

  __mi_bind_size=0;
  __mi_bind_ind=0;
  __mi_bind_code=0;

  __mi_bind_type=0;
  __mb_has_bound=false;

  int_buf_offset=0;
  __mi_buf_size=0;
}

IfxBind::IfxBind(const IfxBind& rv)
{
  int_parameter_index=rv.int_parameter_index;

  __mi_bind_size=rv.__mi_bind_size;
  __mi_bind_ind=rv.__mi_bind_ind;
  __mi_bind_code=rv.__mi_bind_code;

  __mi_bind_type=rv.__mi_bind_type;
  __mb_has_bound=rv.__mb_has_bound;

  int_buf_offset=rv.int_buf_offset;
  __mi_buf_size=rv.__mi_buf_size;
 
  ifx_value = rv.ifx_value; 
}

IfxBind& IfxBind::operator=(const IfxBind& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  int_parameter_index=rv.int_parameter_index;

  __mi_bind_size=rv.__mi_bind_size;
  __mi_bind_ind=rv.__mi_bind_ind;
  __mi_bind_code=rv.__mi_bind_code;

  __mi_bind_type=rv.__mi_bind_type;
  __mb_has_bound=rv.__mb_has_bound;

  int_buf_offset=rv.int_buf_offset;
  __mi_buf_size=rv.__mi_buf_size;

  ifx_value = rv.ifx_value;

  return *this;
}


void IfxBind::cleanup(const IfxColumn& column)
{
  if(ifx_value.int_ec_lofd != -1)
  {
    ifx_lo_close(ifx_value.int_ec_lofd);
    ifx_value.int_ec_lofd = -1;
  }
  
  if(ifx_value.p_ec_create_spec != 0 )
  {
    ifx_lo_spec_free(ifx_value.p_ec_create_spec);
    ifx_value.p_ec_create_spec = 0;
  }
}

void IfxBind::cleanup(const IfxParameter& param)
{
  if(ifx_value.int_ec_lofd != -1)
  {
    ifx_lo_close(ifx_value.int_ec_lofd);
    ifx_value.int_ec_lofd = -1;
  }

  if(ifx_value.p_ec_create_spec != 0 )
  {
    ifx_lo_spec_free(ifx_value.p_ec_create_spec);
    ifx_value.p_ec_create_spec = 0;
  }
}

//---------------------------------------------------
//---------------------------------------------------

IfxDefine::IfxDefine()
{
  __mi_index=-1;

  __mi_define_size=0;
  __mi_define_ind=0;
  __mi_define_code=0;

  int_buf_offset=0;
  __mi_buf_size=0;
}

IfxDefine::IfxDefine(const IfxDefine& rv)
{
  __mi_index=rv.__mi_index;

  __mi_define_size=rv.__mi_define_size;
  __mi_define_ind=rv.__mi_define_ind;
  __mi_define_code=rv.__mi_define_code;

  int_buf_offset=rv.int_buf_offset;
  __mi_buf_size=rv.__mi_buf_size;

  ifx_value = rv.ifx_value;
}

IfxDefine& IfxDefine::operator=(const IfxDefine& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index=rv.__mi_index;

  __mi_define_size=rv.__mi_define_size;
  __mi_define_ind=rv.__mi_define_ind;
  __mi_define_code=rv.__mi_define_code;

  int_buf_offset=rv.int_buf_offset;
  __mi_buf_size=rv.__mi_buf_size;

  ifx_value = rv.ifx_value;

  return *this;
}

void IfxDefine::cleanup(const IfxColumn& column)
{
}

void IfxDefine::cleanup(const IfxParameter& param)
{
}

__OPENCODE_END_NAMESPACE
