#include "xdbc/xtypes.h"

#include "port/libnumber.h"
#include "port/utilities.h"
#include "port/ioc_sbuf.h" 

__OPENCODE_BEGIN_NAMESPACE

const  int Types::BIT            =  XDBC_TYPE_BIT;
const  int Types::TINYINT        =  XDBC_TYPE_TINYINT;
const  int Types::SMALLINT       =  XDBC_TYPE_SMALLINT;
const  int Types::INTEGER        =  XDBC_TYPE_INTEGER;
const  int Types::BIGINT         =  XDBC_TYPE_BIGINT;
const  int Types::FLOAT          =  XDBC_TYPE_FLOAT;
const  int Types::REAL           =  XDBC_TYPE_REAL;
const  int Types::DOUBLE         =  XDBC_TYPE_DOUBLE;
const  int Types::NUMERIC        =  XDBC_TYPE_NUMERIC;
const  int Types::DECIMAL        =  XDBC_TYPE_DECIMAL;
const  int Types::CHAR           =  XDBC_TYPE_CHAR;
const  int Types::VARCHAR        =  XDBC_TYPE_VARCHAR;
const  int Types::LONGVARCHAR    =  XDBC_TYPE_LONGVARCHAR;
const  int Types::DATE           =  XDBC_TYPE_DATE;
const  int Types::TIME           =  XDBC_TYPE_TIME;
const  int Types::TIMESTAMP      =  XDBC_TYPE_TIMESTAMP;
const  int Types::INTERVAL       =  XDBC_TYPE_INTERVAL;
const  int Types::BINARY         =  XDBC_TYPE_BINARY;
const  int Types::VARBINARY      =  XDBC_TYPE_VARBINARY;
const  int Types::LONGVARBINARY  =  XDBC_TYPE_LONGVARBINARY;
const  int Types::xNULL          =  XDBC_TYPE_NULL;
const  int Types::OTHER          =  XDBC_TYPE_OTHER;
const  int Types::DISTINCT       =  XDBC_TYPE_DISTINCT;
const  int Types::STRUCT         =  XDBC_TYPE_STRUCT;
const  int Types::ARRAY          =  XDBC_TYPE_ARRAY;
const  int Types::BLOB           =  XDBC_TYPE_BLOB;
const  int Types::CLOB           =  XDBC_TYPE_CLOB;
const  int Types::REF            =  XDBC_TYPE_REF;
const  int Types::DATALINK       =  XDBC_TYPE_DATALINK;
const  int Types::BOOLEAN        =  XDBC_TYPE_BOOLEAN;

const char* __xdbc_getname(int xdbcTypes)
{ 
  switch(xdbcTypes)
  {
    case XDBC_TYPE_CHAR:
    {
      return "CHAR";
    }
    break;  
    case XDBC_TYPE_VARCHAR:
    {
      return "VARCHAR";
    }
    break;  
    case XDBC_TYPE_LONGVARCHAR:
    {
      return "LONGVARCHAR";
    }
    break;  
    
    case XDBC_TYPE_BINARY:
    {
      return "BINARY";
    }
    break;  
    case XDBC_TYPE_VARBINARY:
    {
      return "VARBINAR";
    }
    break;  
    case XDBC_TYPE_LONGVARBINARY:
    {
      return "LONGVARBINARY";
    }
    break;   

    case XDBC_TYPE_TINYINT:
    {
      return "TINYINT";
    }
    break;  
    case XDBC_TYPE_SMALLINT:
    {
      return "SMALLINT";
    }
    break;  
    case XDBC_TYPE_INTEGER:
    {
      return "INTEGER";
    }
    break;          
    case XDBC_TYPE_BIGINT:
    {
      return "BIGINT";
    }
    break; 

    case XDBC_TYPE_REAL:
    {
      return "REAL";
    }
    break;  
    case XDBC_TYPE_FLOAT:
    {
      return "FLOAT";
    }
    break;  
    case XDBC_TYPE_DOUBLE:
    {
      return "DOUBLE";
    }
    break;

    case XDBC_TYPE_NUMERIC:
    {
      return "NUMERIC";
    }
    break;  
    case XDBC_TYPE_DECIMAL:
    {
      return "DECIMAL";
    }
    break;                               

    case XDBC_TYPE_DATE:
    {
      return "DATE";
    }
    break;  
    case XDBC_TYPE_TIME:
    {
      return "TIME";
    }
    break;  
    case XDBC_TYPE_TIMESTAMP:
    {
      return "TIMESTAMP";
    }
    break;          
    case XDBC_TYPE_INTERVAL:
    {
      return "INTERVAL";
    }
    break;
  }
  
  return "UNKNOWN";  
}

string XDBC_NUMERIC::stringValue(const struct __xdbc_NUMERIC* __x_numeric)
{
  UXLONG __temp_divisor = 1;
  UXLONG __temp = 0;
  XDBC_NUMERIC::parseNumeric(__x_numeric,&__temp,&__temp_divisor);
  
  StringBuffer __sb;
  string __temp_str  = Number::toString(__temp);  
  int insertionPoint = (int)(__temp_str.size() - __x_numeric->scale);
  
  if (insertionPoint == 0) 
  {  
    /* Point goes right before intVal */
    if(__x_numeric->sign != 1)
    {
      __sb << "-0.";
    }
    else
    {
      __sb << "0.";
    } 
    __sb << __temp_str;   
  } 
  else if (insertionPoint > 0) 
  { 
    /* Point goes inside intVal */
    if(__x_numeric->sign != 1)
    {
      __sb << "-";
    }
    
    __sb << __temp_str.substr(0,insertionPoint);
    __sb << ".";
    __sb << __temp_str.substr(insertionPoint + 1,__x_numeric->scale);
  }
  else 
  { 
    /* We must insert zeros between point and intVal */
    if(__x_numeric->sign != 1)
    {
      __sb << "-0.";
    }
    else
    {
      __sb << "0.";
    }
    
    for (int i=0; i<-insertionPoint; i++)
    {
      __sb << "0";
    }        
    __sb << __temp_str;   
  }

  return __sb.str();
}
  
double XDBC_NUMERIC::doubleValue(const struct __xdbc_NUMERIC* __x_numeric)
{
  UXLONG __temp_divisor = 1;
  UXLONG __temp = 0;
  XDBC_NUMERIC::parseNumeric(__x_numeric,&__temp,&__temp_divisor);
    
  if(__x_numeric->sign != 1)
  {
    return -((double)__temp/__temp_divisor);
  }
  else
  {
    return ((double)__temp/__temp_divisor);
  }  
}

XLONG  XDBC_NUMERIC::longValue(const struct __xdbc_NUMERIC* __x_numeric)
{
  UXLONG __temp_divisor = 1;
  UXLONG __temp = 0;
  XDBC_NUMERIC::parseNumeric(__x_numeric,&__temp,&__temp_divisor);
    
  if(__x_numeric->sign != 1)
  {
    return -(__temp/__temp_divisor);
  }
  else
  {
    return (__temp/__temp_divisor);
  }
}

bool XDBC_NUMERIC::valueOf(struct __xdbc_NUMERIC* __x_numeric,const string& value)
{
  return false;
}

bool XDBC_NUMERIC::valueOf(struct __xdbc_NUMERIC* __x_numeric,XLONG value)
{
  return false;
}

bool XDBC_NUMERIC::parseNumeric(const struct __xdbc_NUMERIC* __x_numeric,UXLONG* __numval,UXLONG* __divisor)
{
  char __temp_val[XDBC_MAX_NUMERIC_LEN + 1];
  __temp_val[XDBC_MAX_NUMERIC_LEN] = '\0';
  memcpy(__temp_val,__x_numeric->val,XDBC_MAX_NUMERIC_LEN);
 
  if(__numval != 0)
  {
    *__numval = 0;
    if(__temp_val[0] != 0)
    {
      *__numval = (UXLONG)Number::parseLong(MemoryKit::decode16(__temp_val,XDBC_MAX_NUMERIC_LEN),16);
    }  
  }  
  
  if(__divisor!= 0)
  {
    *__divisor = 1;
    if(__x_numeric->scale > 0)
    {
      for (int i = 0;i < __x_numeric->scale; i++)
      {
        *__divisor = (*__divisor) * 10;
      }
    }  
  }

  return true;
}
  
__OPENCODE_END_NAMESPACE

