#include "xdbc/oracleConversion.h"
#include "xdbc/oracleConnection.h"

#include "xdbc/xtypes.h"
#include "xdbc/oracleTypes.h"

#include "port/ioc_sbuf.h"
#include "port/ioc_smart.h"
#include "port/file.h"
#include "port/utilities.h"
#include "port/libmath.h"

__OPENCODE_BEGIN_NAMESPACE

#define __OracleConversion_CLASS_NAME  "OracleConversion"

OracleConversion::OracleConversion()
{
}

bool   OracleConversion::parseDate(const char* p_datex,XDBC_DATE& date)
{
  const unsigned char* p_date = (const unsigned char*)p_datex;

  int century = *p_date;
  century -= 100;
  ++p_date;

  int year= *p_date;
  year -= 100;
  ++p_date;

  year += (century*100);
  date.year = year;
 
  date.month = *p_date;
  ++p_date;

  date.day = *p_date;

  return true;
}

bool   OracleConversion::parseTime(const char* p_timex,XDBC_TIME& time)
{
  const unsigned char* p_time = (const unsigned char*)p_timex;

  //century
  ++p_time;

  //year
  ++p_time;

  //month
  ++p_time;

  //day
  ++p_time;

  time.hour = *p_time -1;
  ++p_time;
  
  time.minute = *p_time -1;
  ++p_time;

  time.second = *p_time - 1;

  return true;
}

bool  OracleConversion::parseTimestamp(const char* p_timestampx,XDBC_TIMESTAMP& timestamp)
{
  const unsigned char* p_timestamp = (const unsigned char*)p_timestampx;

  int century=*p_timestamp;
  century -= 100;
  ++p_timestamp;

  int year = *p_timestamp;
  year -= 100;
  ++p_timestamp;

  year += (century*100);
  timestamp.year = year;
  
  timestamp.month = *p_timestamp;
  ++p_timestamp;

  timestamp.day = *p_timestamp;
  ++p_timestamp;

  timestamp.hour = *p_timestamp - 1;
  ++p_timestamp;

  timestamp.minute = *p_timestamp - 1;
  ++p_timestamp;

  timestamp.second = *p_timestamp - 1;
  
  timestamp.fraction = 0;
  
  return true;
}

void  OracleConversion::makeDate(const XDBC_TIMESTAMP& dat,char* p_date)
{
  p_date[0] = (dat.year/100) + 100;
  p_date[1] = (dat.year%100) + 100;
  p_date[2] = dat.month;
  p_date[3] = dat.day;
  p_date[4] = dat.hour + 1;
  p_date[5] = dat.minute + 1;
  p_date[6] = dat.second + 1;
}

void  OracleConversion::makeDate(const XDBC_TIMESTAMP& dat,OCIDate* p_date)
{
  OCIDateSetTime(p_date, dat.hour, dat.minute, dat.second);
  OCIDateSetDate(p_date, dat.year, dat.month, dat.day);

}

void  OracleConversion::makeDate(const XDBC_DATE& dat,char* p_date)
{
  p_date[0] = (dat.year/100) + 100;
  p_date[1] = (dat.year%100) + 100;
  p_date[2] = dat.month;
  p_date[3] = dat.day;
  p_date[4] = 1;
  p_date[5] = 1;
  p_date[6] = 1;
}

void  OracleConversion::makeDate(const XDBC_DATE& dat,OCIDate* p_date)
{
  OCIDateSetTime(p_date, 0, 0, 0);
  OCIDateSetDate(p_date, dat.year, dat.month, dat.day);
}

void  OracleConversion::makeDate(const XDBC_TIME& dat,char* p_date)
{
  p_date[0] = (1970/100) + 100;
  p_date[1] = (1970%100) + 100;
  p_date[2] = 1; 
  p_date[3] = 1;
  
  p_date[4] = dat.hour + 1;
  p_date[5] = dat.minute + 1;
  p_date[6] = dat.second + 1;
}

void  OracleConversion::makeDate(const XDBC_TIME& dat,OCIDate* p_date)
{
  OCIDateSetTime(p_date, dat.hour, dat.minute, dat.second);
  OCIDateSetDate(p_date, 1970, 1, 1);
}

void  OracleConversion::makeInterval(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,const XDBC_INTERVAL& interval,OCIInterval* ora_INTERVAL)  __XDBC_THROW_DECLARE(SQLException)
{
  ub2 inv_type = parseIntervalType(interval);
  sword ret_code = 0;
  
  if( SQLT_INTERVAL_YM == inv_type )
  {
    if(interval.interval_sign == XDBC_TRUE)
    {
       ret_code = OCIIntervalSetYearMonth ((dvoid*)p_conn->oci_envhp,
                                            p_conn->oci_errhp,
                                            (sb4)-(interval.intval.year_month.year), 
                                            (sb4)-(interval.intval.year_month.month),
                                             ora_INTERVAL);
     }
     else
     {
       ret_code = OCIIntervalSetYearMonth ((dvoid*)p_conn->oci_envhp,
                                            p_conn->oci_errhp,
                                            (sb4)interval.intval.year_month.year, 
                                            (sb4)interval.intval.year_month.month,
                                            ora_INTERVAL);     
     }
      
  }
  else
  {
    if(interval.interval_sign == XDBC_TRUE)
    {
       ret_code = OCIIntervalSetDaySecond((dvoid*)p_conn->oci_envhp,p_conn->oci_errhp,
                                           (sb4)-(interval.intval.day_second.day),
                                           (sb4)-(interval.intval.day_second.hour),
                                           (sb4)-(interval.intval.day_second.minute),
                                           (sb4)-(interval.intval.day_second.second),
                                           (sb4)-(interval.intval.day_second.fraction),
                                            ora_INTERVAL);       
    }
    else
    {
       ret_code = OCIIntervalSetDaySecond((dvoid*)p_conn->oci_envhp,p_conn->oci_errhp,
                                           (sb4)interval.intval.day_second.day,
                                           (sb4)interval.intval.day_second.hour,
                                           (sb4)interval.intval.day_second.minute,
                                           (sb4)interval.intval.day_second.second,
                                           (sb4)interval.intval.day_second.fraction,
                                            ora_INTERVAL);         
    }  
  }
  
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
  __XDBC_HOOK_CHECK(return); 
}

//--------------------------------------------------------------------------------------------------------

bool  OracleConversion::parseDate(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_DATE& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  OCIDateGetDate(p_date, &dat.year, &dat.month, &dat.day); 
  
  return true;
}

bool OracleConversion::parseTime(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_TIME& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  OCIDateGetTime(p_date, &dat.hour, &dat.minute, &dat.second);
  
  return true;
}

bool OracleConversion::parseTimestamp(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDate* p_date,XDBC_TIMESTAMP& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  OCIDateGetTime(p_date, &dat.hour, &dat.minute, &dat.second);
  OCIDateGetDate(p_date, &dat.year, &dat.month, &dat.day);
  
  return true;
}

bool  OracleConversion::parseDate(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_DATE& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  sb2  year=0;
  ub1  month=0 ;
  ub1  day=0;

  sword ret_code = OCIDateTimeGetDate(p_conn->oci_envhp,p_conn->oci_errhp,ora_DATETIME,&year,&month,&day);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE   p_conn->oci_errhp,ret_code,"OracleConversion");
  __XDBC_HOOK_CHECK(return false);
  
  dat.year  = year;
  dat.month = month;
  dat.day   = day;
  
  return true;
}

bool OracleConversion::parseTime(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_TIME& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  ub1 hour=0;
  ub1 min=0;
  ub1 sec=0;
  ub4 fsec=0;

  sword ret_code = OCIDateTimeGetTime(p_conn->oci_envhp,p_conn->oci_errhp,ora_DATETIME,&hour,&min,&sec,&fsec);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
  __XDBC_HOOK_CHECK(return false);
  
  dat.hour = hour;
  dat.minute = min;
  dat.second = sec;
     
  return true;
}

bool OracleConversion::parseTimestamp(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIDateTime* ora_DATETIME,int oracleType,XDBC_TIMESTAMP& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  sb2  year=0;
  ub1  month=0 ;
  ub1  day=0;

  sword ret_code = OCIDateTimeGetDate(p_conn->oci_envhp,p_conn->oci_errhp,ora_DATETIME,&year,&month,&day);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
  __XDBC_HOOK_CHECK(return false);
    
  ub1 hour=0;
  ub1 min=0;
  ub1 sec=0;
  ub4 fsec=0;

  ret_code = OCIDateTimeGetTime(p_conn->oci_envhp,p_conn->oci_errhp,ora_DATETIME,&hour,&min,&sec,&fsec);
  OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
  __XDBC_HOOK_CHECK(return false);
  
  dat.year  = year;
  dat.month = month;
  dat.day   = day;
    
  dat.hour = hour;
  dat.minute = min;
  dat.second = sec;
  dat.fraction = fsec;
  
  if(   oracleType == SQLT_DATE 
     || oracleType == SQLT_TIMESTAMP_TZ
     || oracleType == SQLT_TIMESTAMP_LTZ)
  {   
    sb1 hour = 0;
    sb1 min = 0;
    
    OCIDateTimeGetTimeZoneOffset ( p_conn->oci_envhp,
                                   p_conn->oci_errhp,
                                   ora_DATETIME,
                                   &hour,
                                   &min);
    
    dat.tzOffset = ((XLONG)hour) * 60 * 60 * 1000  + ((XLONG)min) * 60 * 1000;  
  }
  
  return true;
}

bool OracleConversion::parseInterval(__XDBC_HOOK_DECLARE_MORE OracleConnection* p_conn,OCIInterval* ora_INTERVAL,int oracleType,XDBC_INTERVAL& dat)  __XDBC_THROW_DECLARE(SQLException)
{
  if(oracleType == SQLT_INTERVAL_YM)
  {
    sb4  year=0;
    sb4  month=0 ;

    sword ret_code = OCIIntervalGetYearMonth(p_conn->oci_envhp,p_conn->oci_errhp,&year,&month,ora_INTERVAL);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
    __XDBC_HOOK_CHECK(return false);

    dat.interval_type = XDBC_IS_YEAR_TO_MONTH;
    dat.interval_sign = XDBC_FALSE;
    if(year < 0 || month < 0)
    {
      dat.interval_sign = XDBC_TRUE;
    }    
    
    dat.intval.year_month.year  = Math::abs(year);
    dat.intval.year_month.month = Math::abs(month);
  }
  else
  {
    sb4  day = 0;
    sb4 hour = 0;
    sb4 min = 0;
    sb4 sec = 0;
    sb4 fsec = 0;

    sword ret_code = OCIIntervalGetDaySecond(p_conn->oci_envhp,p_conn->oci_errhp,&day,&hour,&min,&sec,&fsec,ora_INTERVAL);
    OracleConversion::checkException(__XDBC_HOOK_INVOKE_MORE  p_conn->oci_errhp,ret_code,"OracleConversion");
    __XDBC_HOOK_CHECK(return false);
    
    dat.interval_type = XDBC_IS_DAY_TO_SECOND;
    dat.interval_sign = XDBC_FALSE;
    if(day < 0 || hour < 0 || min < 0 || sec < 0 || fsec < 0)
    {
      dat.interval_sign = XDBC_TRUE;
    }
      
    dat.intval.day_second.day = Math::abs(day);
    dat.intval.day_second.hour = Math::abs(hour);
    dat.intval.day_second.minute = Math::abs(min);
    dat.intval.day_second.second = Math::abs(sec);
    dat.intval.day_second.fraction = Math::abs(fsec);
  }
  
  return true;
}

//--------------------------------------------------------------------------------------------------------

ub4  OracleConversion::SQLT_TO_DTYPE(ub2 sqlt)
{
  switch(sqlt)
  {
    case  SQLT_CLOB:
    case  SQLT_BLOB:
    {
      return OCI_DTYPE_LOB;
    }

    case  SQLT_FILE:
    {
      return OCI_DTYPE_FILE;
    }

    case  SQLT_TIMESTAMP:
    {
       return OCI_DTYPE_TIMESTAMP;
    }

    case  SQLT_TIMESTAMP_TZ:
    {
      return OCI_DTYPE_TIMESTAMP_TZ;
    }

    case  SQLT_TIMESTAMP_LTZ:
    {
      return OCI_DTYPE_TIMESTAMP_LTZ;
    }

    case  SQLT_DATE:
    {
      return OCI_DTYPE_DATE;
    }

    case  SQLT_RDD:
    {
      return OCI_DTYPE_ROWID;
    }

    case  SQLT_INTERVAL_YM:
    {
      return OCI_DTYPE_INTERVAL_YM;
    }

    case  SQLT_INTERVAL_DS:
    {
      return OCI_DTYPE_INTERVAL_DS;
    }

    default:
    {
      break;
    }
  }
  
  return 0;
}

//--------------------------------------------------------------------------------------------------------

int   OracleConversion::SQLT_TO_XDBC(ub2 sqlt)
{
  return OracleTypes::getInstance().Oracle_2_XDBC(sqlt);
}

int   OracleConversion::SQLT_TO_XDBC(const OracleColumn& column)
{
  return OracleTypes::getInstance().Oracle_2_XDBC(column);
}

ub2   OracleConversion::XDBC_TO_SQLT(int xtype)
{
  return OracleTypes::getInstance().XDBC_2_Oracle(xtype);
}

ub2  OracleConversion::parseIntervalType(const XDBC_INTERVAL& interval)
{
  if( interval.interval_type == XDBC_IS_YEAR_TO_MONTH)
  {
    return SQLT_INTERVAL_YM;
  }
  else
  {
    return SQLT_INTERVAL_DS;
  }
}

//--------------------------------------------------------------------------------------------------------

void  OracleConversion::checkException(__XDBC_HOOK_DECLARE_MORE OCIError *errhp,sword status,const char* className) __XDBC_THROW_DECLARE(SQLException)
{
  if(OCI_SUCCESS == status)
  {
    //printf("OK - OCI_SUCCESS\n");
    return; 
  }

  text errbuf[1024];
  sb4 errcode = 0;

  StringBuffer __sb;
  if(className != 0)
  {
    __sb << "["<<className << "] - ";
  } 
   
  switch (status)
  {
    case OCI_SUCCESS_WITH_INFO:
    {
      //printf("Error - OCI_SUCCESS_WITH_INFO\n");
      OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,errbuf,(ub4) sizeof(errbuf),OCI_HTYPE_ERROR);
      __sb.append((const char*)errbuf);
      __XDBC_HOOK_RETURN(return,__sb.str(),errcode);
    }
    break;

    case OCI_NEED_DATA:
    {
      //printf("Error - OCI_NEED_DATA\n");
    }
    break;

    case OCI_NO_DATA:
    {
      //printf("Error - OCI_NODATA\n");
    }
    break;

    case OCI_ERROR:
    {
      //printf("Error - OCI_ERROR\n");
      OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,errbuf,(ub4) sizeof(errbuf),OCI_HTYPE_ERROR);
      //printf("Error - %.*s\n", 1024, errbuf);
      __sb.append((const char*)errbuf);
      __XDBC_HOOK_RETURN(return,__sb.str(),errcode);
    }
    break;

    case OCI_INVALID_HANDLE:
    {
      //printf("Error - OCI_INVALID_HANDLE\n");
      __sb.append("OCI_INVALID_HANDLE");
      __XDBC_HOOK_RETURN(return,__sb.str());
    }
    break;

    case OCI_STILL_EXECUTING:
    {
      //printf("Error - OCI_STILL_EXECUTE\n");
    }
    break;

    case OCI_CONTINUE:
    {
      //printf("Error - OCI_CONTINUE\n");
    }
    break;

    default:
    {
      //printf("Default - OCI_SUCCESS\n");
    }
    break;
  }
}

void OracleConversion::parseResultSetMetaData(__XDBC_HOOK_DECLARE_MORE OCIError* oci_errhp,OCIStmt* oci_stmthp,vector<OracleColumn>& __vector_columns)  __XDBC_THROW_DECLARE(SQLException)
{
  ub4 numcols = 0;
  /* Get the number of columns in the query */
  checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp, OCIAttrGet((dvoid *)oci_stmthp, OCI_HTYPE_STMT,
                                      (dvoid *)&numcols, (ub4 *)0, OCI_ATTR_PARAM_COUNT,
                                       oci_errhp),
                                       "parseResultSetMetaData");
  __XDBC_HOOK_CHECK(return);
    
  OCIParam* parmdp = 0;

  text*     namep = 0;
  ub4       sizep = 0;

  ub2       collen = 0;
  ub2       coltyp = 0;
  sword     retval = 0;

  /**
   * The column attribute OCI_ATTR_PRECISION can be returned using an implicit
   * describe with OCIStmtExecute() and an explicit describe with
   * OCIDescribeAny(). When using an implicit describe, the precision should be set
   * to sb2. When using an explicit describe, the precision should be set to ub1 for a
   * placeholder. This is necessary to match the datatype of precision in the dictionary.
   */
  sb2       precision = 0;

  sb1       scale = 0;
  sb1       nullable = 0;
  
  __vector_columns.resize(numcols + 1);
 
  StringBuffer __sb;
  if(DriverManager::isTracingOn)
  {
    __sb << "parseResultSetMetaData|" << File::lineSeparator;
  }
  
  for(int i = 1;i <= numcols; ++i)
  {
    OracleColumn& oracle_column = __vector_columns[i];

    /* get parameter for column i */
    checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
       OCIParamGet((dvoid *)oci_stmthp, OCI_HTYPE_STMT, oci_errhp,(dvoid **)&parmdp, i),
                   "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
      
    __vector_columns[i].__mi_index = i;

    /* Retrieve the length semantics for the column */
    int char_semantics = 0;
    checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
       OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM, (dvoid*) &char_semantics,(ub4 *) 0,
                              (ub4) OCI_ATTR_CHAR_USED, (OCIError*)oci_errhp),
                   "parseResultSetMetaData");
  
    __XDBC_HOOK_CHECK(return);
    
    /* column length */
    if(char_semantics)
    {
       checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
           OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                                  (dvoid*) &collen, (ub4 *) 0,
                                  (ub4) OCI_ATTR_CHAR_SIZE, (OCIError *)oci_errhp),
                       "parseResultSetMetaData");
       __XDBC_HOOK_CHECK(return);
       
       __vector_columns[i].__mb_char_semantics=true;
    }
    else
    {
       checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
          OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                                 (dvoid*) &collen, (ub4 *) 0,
                                 (ub4) OCI_ATTR_DATA_SIZE, (OCIError *)oci_errhp),
                      "parseResultSetMetaData");
       __XDBC_HOOK_CHECK(return);
       
       __vector_columns[i].__mb_char_semantics=false;
    }
  
    __vector_columns[i].__mi_length=collen;

    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
        OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                               (dvoid*) &namep, (ub4 *) &sizep,
                               (ub4) OCI_ATTR_NAME, (OCIError *)oci_errhp),
                    "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__ms_name.append((char*)namep,(size_t) sizep);

    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
        OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                               (dvoid*) &namep, (ub4 *) &sizep,
                               (ub4) OCI_ATTR_SCHEMA_NAME, (OCIError *)oci_errhp),
                   "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__ms_schema_name.append((char*)namep,(size_t) sizep);
 
    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp,
        OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                               (dvoid*) &namep, (ub4 *) &sizep,
                               (ub4) OCI_ATTR_TYPE_NAME, (OCIError *)oci_errhp),
                    "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__ms_type_name.append((char*)namep,(size_t) sizep);
 
    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp,
        OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                               (dvoid*) &coltyp, (ub4 *) 0,
                               (ub4) OCI_ATTR_DATA_TYPE, (OCIError *)oci_errhp),
                    "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__mi_type=coltyp;

    checkException(__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
       OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                              (dvoid*) &precision, (ub4 *) 0,
                              (ub4) OCI_ATTR_PRECISION, (OCIError *)oci_errhp),
                   "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__mi_precision=precision;

    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
        OCIAttrGet ((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                                (dvoid*) &scale, (ub4 *) 0,
                                (ub4) OCI_ATTR_SCALE, (OCIError *)oci_errhp),
                    "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__mi_scale=scale;
  
    checkException (__XDBC_HOOK_INVOKE_MORE  oci_errhp, 
        OCIAttrGet ((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,
                                (dvoid*) &nullable, (ub4 *) 0,
                                (ub4) OCI_ATTR_IS_NULL , (OCIError *)oci_errhp),
                    "parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
    
    __vector_columns[i].__mi_nullable=nullable;
  
    if(__vector_columns[i].__mi_type == SQLT_NUM)
    {
      __vector_columns[i].__mi_type = SQLT_VNU;
    }
 
    if(__vector_columns[i].__ms_type_name.empty())
    {
      __vector_columns[i].__ms_type_name = OracleTypes::getInstance().getTypeName(__vector_columns[i]);
    }
    
    if(DriverManager::isTracingOn)
    { 
      __vector_columns[i].dump(__sb);
    }
  }

  __XDBC_TRACE(__OracleConversion_CLASS_NAME,__sb.str());
}

__OPENCODE_END_NAMESPACE
