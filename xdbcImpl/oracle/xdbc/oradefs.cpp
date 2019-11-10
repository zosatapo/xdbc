#include "xdbc/oradefs.h"
#include "xdbc/oracleConversion.h"
#include "port/file.h"

USING_OPENCODE_STL_NS
__OPENCODE_BEGIN_NAMESPACE

//------------------------------------------------------
//------------------------------------------------------
OracleParameter::OracleParameter()
{
  __mi_index=-1;

  __mi_length=0;
  __mi_type=0;
  __mi_precision=0;
  __mi_scale=0;

  __mi_iomode=0;
}

OracleParameter::OracleParameter(const OracleParameter& rv)
{
  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;

  __mi_iomode = rv.__mi_iomode;

}

OracleParameter& OracleParameter::operator=(const OracleParameter& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;

  __mi_iomode = rv.__mi_iomode;

  return *this;
}


void OracleParameter::dump(StringBuffer& __sb)
{
  __sb << "-------------------------------------------" << File::lineSeparator;
  __sb << "__mi_index       : " << __mi_index << File::lineSeparator;
  __sb << "__ms_name        : " << __ms_name << File::lineSeparator;  
  __sb << "__mi_type        : " << __mi_type << File::lineSeparator;
  __sb << "__ms_type_name   : " << __ms_type_name << File::lineSeparator;
  __sb << "__mi_length      : " << __mi_length << File::lineSeparator;
   
  __sb << "__mi_precision   : " << __mi_precision << File::lineSeparator;
  __sb << "__mi_scale       : " << __mi_scale << File::lineSeparator;

  __sb << "__ms_schema_name : " << __ms_schema_name << File::lineSeparator;


  __sb<<  "__mi_iomode      : " <<__mi_iomode<<File::lineSeparator;
}

//---------------------------------------------------
// OracleColumn Class 
//---------------------------------------------------

OracleColumn::OracleColumn()
{
  __mi_index=-1;

  __mi_length=0;
  __mi_type=0;
  __mi_precision=0;
  __mi_scale=0;
  
  __mb_char_semantics=false;

  __mi_nullable=0;
}

OracleColumn::OracleColumn(const OracleColumn& rv)
{
  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;  

  __mb_char_semantics = rv.__mb_char_semantics;

  __mi_nullable = rv.__mi_nullable;
  __ms_table_name = rv.__ms_table_name;
}

OracleColumn& OracleColumn::operator=(const OracleColumn& rv)
{
  if(&rv == this)
  {
    return *this;
  }


  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;

  __mb_char_semantics = rv.__mb_char_semantics;

  __mi_nullable = rv.__mi_nullable;
  __ms_table_name = rv.__ms_table_name;

  return *this;
}

void OracleColumn::dump(StringBuffer& __sb)
{
  __sb << "-------------------------------------------" << File::lineSeparator;
  __sb << "__mi_index          : " << __mi_index << File::lineSeparator;

  __sb << "__ms_name           : " << __ms_name << File::lineSeparator;
  __sb << "__mi_type           : " << __mi_type << File::lineSeparator;
  __sb << "__ms_type_name      : " << __ms_type_name << File::lineSeparator;
  __sb << "__mi_length         : " << __mi_length << File::lineSeparator;
  __sb << "__mi_precision      : " << __mi_precision << File::lineSeparator; 
  __sb << "__mi_scale          : " << __mi_scale << File::lineSeparator;

  __sb << "__mb_char_semantics : " << __mb_char_semantics << File::lineSeparator;  
  __sb << "__ms_schema_name    : " << __ms_schema_name << File::lineSeparator;
  __sb << "__ms_table_name     : " << __ms_table_name << File::lineSeparator;
}

//---------------------------------------------------
//---------------------------------------------------

OracleValue::OracleValue()
{
  ora_DATETIME = 0;
  ora_INTERVAL = 0;

  ora_LOB = 0;
  ora_ROWID = 0;
}

OracleValue::OracleValue(const OracleValue& rv)
{
  ora_DATETIME = rv.ora_DATETIME;
  ora_INTERVAL = rv.ora_INTERVAL;
  
  ora_LOB = rv.ora_LOB;
  ora_ROWID = rv.ora_ROWID;
  
  ora_DATE = rv.ora_DATE; 
  ora_NUMBER = rv.ora_NUMBER;

  ora_UBIGINT = rv.ora_UBIGINT;
}

OracleValue& OracleValue::operator=(const OracleValue& rv)
{
  if(&rv == this)
  {
    return *this;
  }
  
  ora_DATETIME = rv.ora_DATETIME;
  ora_INTERVAL = rv.ora_INTERVAL;
  
  ora_LOB = rv.ora_LOB;
  ora_ROWID = rv.ora_ROWID;
  
  ora_DATE = rv.ora_DATE; 
  ora_NUMBER = rv.ora_NUMBER;
  ora_UBIGINT = rv.ora_UBIGINT;

  return *this;
}

//---------------------------------------------------
//---------------------------------------------------
OracleBind::OracleBind()
{
  __mi_index=-1;

  __mi_bind_size=0;
  __mi_bind_ind=0;
  __mi_bind_code=0;

  __mi_bind_type=0;
  __mb_has_bound=false;

  oci_bndp = 0;
}

OracleBind::OracleBind(const OracleBind& rv)
{
  __mi_index = rv.__mi_index;

  string_value = rv.string_value;
  ora_value = rv.ora_value; 
  oci_bndp = rv.oci_bndp;

  __mi_bind_size = rv.__mi_bind_size;
  __mi_bind_ind = rv.__mi_bind_ind;
  __mi_bind_code = rv.__mi_bind_code;

  __mi_bind_type = rv.__mi_bind_type;
  __mb_has_bound = rv.__mb_has_bound;
}

OracleBind& OracleBind::operator=(const OracleBind& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index = rv.__mi_index;

  string_value = rv.string_value;
  ora_value = rv.ora_value;
  oci_bndp = rv.oci_bndp;

  __mi_bind_size = rv.__mi_bind_size;
  __mi_bind_ind = rv.__mi_bind_ind;
  __mi_bind_code = rv.__mi_bind_code;

  __mi_bind_type = rv.__mi_bind_type;
  __mb_has_bound = rv.__mb_has_bound;

  return *this;
}

void OracleBind::cleanup(const OracleColumn& column)
{
}

void OracleBind::cleanup(const OracleParameter& param)
{
  if(ora_value.ora_LOB != 0)
  {
    OCIDescriptorFree(ora_value.ora_LOB,OracleConversion::SQLT_TO_DTYPE(param.__mi_type));
    ora_value.ora_LOB = 0;
  }

  if(ora_value.ora_DATETIME != 0)
  {
    OCIDescriptorFree(ora_value.ora_DATETIME,OracleConversion::SQLT_TO_DTYPE(param.__mi_type));
    ora_value.ora_DATETIME = 0;
  }

  if(ora_value.ora_INTERVAL != 0)
  {
    OCIDescriptorFree(ora_value.ora_INTERVAL,(ub4) OracleConversion::SQLT_TO_DTYPE(param.__mi_type));
    ora_value.ora_INTERVAL = 0;
  }

  if(ora_value.ora_ROWID != 0)
  {
    OCIDescriptorFree(ora_value.ora_ROWID,(ub4) OracleConversion::SQLT_TO_DTYPE(param.__mi_type));
    ora_value.ora_ROWID = 0;
  }

}

//-----------------------------------------------------
//-----------------------------------------------------
OracleDefine::OracleDefine()
{
  __mi_index=-1;

  __mi_define_size=0;
  __mi_define_ind=0;
  __mi_define_code=0;
  __mi_define_type=0;

  int_buf_offset=0;
  __mi_buf_size=0;

  oci_defnp = 0;
}

OracleDefine::OracleDefine(const OracleDefine& rv)
{
  __mi_index = rv.__mi_index;

  __mi_define_size = rv.__mi_define_size;
  __mi_define_ind = rv.__mi_define_ind;
  __mi_define_code = rv.__mi_define_code;
  __mi_define_type = rv.__mi_define_type;
  
  ora_value = rv.ora_value;
  oci_defnp = rv.oci_defnp;

  int_buf_offset = rv.int_buf_offset;
  __mi_buf_size = rv.__mi_buf_size;
}

OracleDefine& OracleDefine::operator=(const OracleDefine& rv)
{
  if(&rv == this)
  {
    return *this;
  }

  __mi_index = rv.__mi_index;

  __mi_define_size = rv.__mi_define_size;
  __mi_define_ind = rv.__mi_define_ind;
  __mi_define_code = rv.__mi_define_code;
  __mi_define_type = rv.__mi_define_type;
  
  ora_value = rv.ora_value;
  oci_defnp = rv.oci_defnp;

  int_buf_offset = rv.int_buf_offset;
  __mi_buf_size = rv.__mi_buf_size;

  return *this;
}


void OracleDefine::cleanup(const OracleColumn& column)
{
  sword data_type=column.__mi_type;
  
  if(ora_value.ora_LOB != 0)
  {
    OCIDescriptorFree(ora_value.ora_LOB,(ub4) OracleConversion::SQLT_TO_DTYPE(data_type));
    ora_value.ora_LOB = 0;
  }
  
  if(ora_value.ora_DATETIME != 0)
  {
    OCIDescriptorFree(ora_value.ora_DATETIME,(ub4) OracleConversion::SQLT_TO_DTYPE(data_type));
    ora_value.ora_DATETIME = 0;
  }
  
  if(ora_value.ora_INTERVAL != 0)
  {
    OCIDescriptorFree(ora_value.ora_INTERVAL,(ub4) OracleConversion::SQLT_TO_DTYPE(data_type));
    ora_value.ora_INTERVAL = 0;
  }
  
  if(ora_value.ora_ROWID != 0)
  {
    OCIDescriptorFree(ora_value.ora_ROWID,(ub4) OracleConversion::SQLT_TO_DTYPE(data_type));
    ora_value.ora_ROWID = 0;
  }

}

void OracleDefine::cleanup(const OracleParameter& param)
{
}

__OPENCODE_END_NAMESPACE
