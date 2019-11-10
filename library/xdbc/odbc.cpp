#include "xdbc/odbc.h"
#include "xdbc/odbcConnection.h"
#include "xdbc/odbcTypes.h" 
#include "port/utilities.h"
#include "port/libsys.h"
#include "port/file.h"
#include "port/profile.h" 
#include "port/ioc_smart.h" 
#include "port/ioc_sbuf.h"

__OPENCODE_BEGIN_NAMESPACE

#define __ODBCCONVERSION_CLASS_NAME   "OdbcConversion"

OdbcDriver::~OdbcDriver()
{
  delete __odbc_apis;
  __odbc_syslib.close();
}

OdbcDriver::OdbcDriver()
{
  __odbc_apis = 0; 
}

OdbcDriver::OdbcDriver(const SystemLibrary& __odbc_syslibValue)
{ 
  this->__odbc_apis = new struct ODBC_LIBRARY_APIS();
  this->__odbc_syslib = __odbc_syslibValue;   
}

#define __XDBC_LOAD_TRACE(__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_name2)\
{\
  if(__odbc_apis->__xdbc_api_ptr == 0)\
  {\
    StringBuffer __sb;\
    __sb << "checking odbc api '" << __xdbc_api_name <<"' ... FAILED <--";\
    __XDBC_TRACE("OdbcDriverManager",__sb.str());\
  }\
  else\
  {\
    StringBuffer __sb;\
    __sb << "checking odbc api '" << __xdbc_api_name << "' ... PASSED";\
    __XDBC_TRACE("OdbcDriverManager",__sb.str());\
  }\
}

#define __XDBC_LOAD_CHECK(__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_name2)\
{\
  if(__odbc_apis->__xdbc_api_ptr == 0)\
  {\
    StringBuffer __sb;\
    __sb << "odbc api '"<< __xdbc_api_name << "' not found in '" << __odbc_syslib.getPathname() << "'";\
    __XDBC_HOOK_RETURN(return false,__sb.str());\
  }\
}  

#define __XDBC_LOAD_DOUBLE_CHECK(__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_ptr2,__xdbc_api_name2)\
{\
  if( (__odbc_apis->__xdbc_api_ptr == 0) && (__odbc_apis->__xdbc_api_ptr2 == 0))\
  {\
    StringBuffer __sb;\
    __sb << "odbc api '"<< __xdbc_api_name << "' AND '" << __xdbc_api_name2 << "' not found in '" << __odbc_syslib.getPathname() << "'";\
    __XDBC_HOOK_RETURN(return false,__sb.str());\
  }\
}

#define __XDBC_LOAD(__xdbc_api_soh,__xdbc_api_type,__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_name2,__xdbc_api_check,__xdbc_api_trace)\
{\
  __odbc_apis->__xdbc_api_ptr      = (__xdbc_api_type)__xdbc_api_soh.findSymbol(__xdbc_api_name); \
  if((__odbc_apis->__xdbc_api_ptr == 0) && (__xdbc_api_name2 != 0))\
  {\
     __odbc_apis->__xdbc_api_ptr      = (__xdbc_api_type)__xdbc_api_soh.findSymbol(__xdbc_api_name2); \
  }\
  if(__xdbc_api_trace)\
  {\
    __XDBC_LOAD_TRACE(__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_name2);\
  }\
  if(__xdbc_api_check)\
  {\
    __XDBC_LOAD_CHECK(__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_name2);\
  }\
}

bool OdbcDriver::load() __XDBC_THROW_DECLARE(SQLException)
{  
  __odbc_apis->__mi_odbc_version =  ODBCVER;
  __odbc_apis->__mi_driver_version =  0x0250;
  
#if (ODBCVER >= 0x0300)
  ODBCSQLAPI_SQLALLOCHANDLE __api_hook_1 = (ODBCSQLAPI_SQLALLOCHANDLE)__odbc_syslib.findSymbol("SQLAllocHandle");
  if(__api_hook_1 != 0)
  {
    __odbc_apis->__mi_driver_version = ODBCVER;
  }
#endif
  
  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    __sb << "checking odbc apis form ODBC Driver '" << __odbc_syslib.getPathname() << "' : " 
         << "__mi_odbc_version   = <<0x" 
         << Number::toString(__odbc_apis->__mi_odbc_version,16) 
         << ">>, __mi_driver_version = <<0x" 
         << Number::toString(__odbc_apis->__mi_driver_version,16) 
         << ">>" << File::lineSeparator;
                                                  
    __XDBC_FORCED_TRACE("OdbcDriverManager",__sb.str()); 
  }

  
  int __flag_check_lt = (__odbc_apis->__mi_driver_version < 0x0300) ? XDBC_TRUE : XDBC_FALSE;
  int __flag_check_eqgt = (__odbc_apis->__mi_driver_version >= 0x0300) ? XDBC_TRUE : XDBC_FALSE;
 
  /********************************************************************  Core Functions           */

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLALLOCCONNECT,__api_SQLALLOCCONNECT,"SQLAllocConnect",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLALLOCENV,__api_SQLALLOCENV,"SQLAllocEnv",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLALLOCSTMT,__api_SQLALLOCSTMT,"SQLAllocStmt",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFREECONNECT,__api_SQLFREECONNECT,"SQLFreeConnect",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFREEENV,__api_SQLFREEENV,"SQLFreeEnv",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFREESTMT,__api_SQLFREESTMT,"SQLFreeStmt",0,__flag_check_lt,XDBC_TRUE);;
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLERROR,__api_SQLERROR,"SQLError",0,__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLTRANSACT,__api_SQLTRANSACT,"SQLTransact",0,__flag_check_lt,XDBC_TRUE);
    
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDISCONNECT,__api_SQLDISCONNECT,"SQLDisconnect",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLEXECUTE,__api_SQLEXECUTE,"SQLExecute",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFETCH,__api_SQLFETCH,"SQLFetch",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLNUMRESULTCOLS,__api_SQLNUMRESULTCOLS,"SQLNumResultCols",0,XDBC_TRUE,XDBC_TRUE);
 

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLBINDCOL_INT,__api_SQLBINDCOL_INT,"SQLBindCol",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLBINDCOL_LEN,__api_SQLBINDCOL_LEN,"SQLBindCol",0,XDBC_FALSE,XDBC_FALSE);
 
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLROWCOUNT_INT,__api_SQLROWCOUNT_INT,"SQLRowCount",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLROWCOUNT_LEN,__api_SQLROWCOUNT_LEN,"SQLRowCount",0,XDBC_FALSE,XDBC_FALSE);
  

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBECOL_INT,__api_SQLDESCRIBECOL_INT,"SQLDescribeCol",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBECOL_LEN,__api_SQLDESCRIBECOL_LEN,"SQLDescribeCol",0,XDBC_FALSE,XDBC_FALSE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBECOLW_INT,__api_SQLDESCRIBECOLW_INT,"SQLDescribeColW",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBECOLW_LEN,__api_SQLDESCRIBECOLW_LEN,"SQLDescribeColW",0,XDBC_FALSE,XDBC_FALSE);
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLDESCRIBECOL_INT,"SQLDescribeCol",__api_SQLDESCRIBECOLW_INT,"SQLDescribeColW");


  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLPREPARE,__api_SQLPREPARE,"SQLPrepare",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLPREPAREW,__api_SQLPREPAREW,"SQLPrepareW",0,XDBC_FALSE,XDBC_TRUE);  
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLPREPARE,"SQLPrepare",__api_SQLPREPAREW,"SQLPrepareW"); 
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLEXECDIRECT,__api_SQLEXECDIRECT,"SQLExecDirect",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLEXECDIRECTW,__api_SQLEXECDIRECTW,"SQLExecDirectW",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLEXECDIRECT,"SQLExecDirect",__api_SQLEXECDIRECTW,"SQLExecDirectW"); 
  
  /********************************************************************  Level 1 Functions        */

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETCONNECTOPTION,__api_SQLGETCONNECTOPTION,"SQLGetConnectOption","SQLGetConnectOptionW",__flag_check_lt,XDBC_TRUE);

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETCONNECTOPTION_INT,__api_SQLSETCONNECTOPTION_INT,"SQLSetConnectOption","SQLSetConnectOptionW",__flag_check_lt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETCONNECTOPTION_LEN,__api_SQLSETCONNECTOPTION_LEN,"SQLSetConnectOption","SQLSetConnectOptionW",__flag_check_lt,XDBC_FALSE);

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETSTMTOPTION_INT,__api_SQLSETSTMTOPTION_INT,"SQLSetStmtOption","SQLSetStmtOptionW",XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETSTMTOPTION_LEN,__api_SQLSETSTMTOPTION_LEN,"SQLSetStmtOption","SQLSetStmtOptionW",XDBC_FALSE,XDBC_FALSE);    
 
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDRIVERCONNECT,__api_SQLDRIVERCONNECT,"SQLDriverConnect",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDRIVERCONNECTW,__api_SQLDRIVERCONNECTW,"SQLDriverConnectW",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLDRIVERCONNECT,"SQLDriverConnect",__api_SQLDRIVERCONNECTW,"SQLDriverConnectW");
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETINFO,__api_SQLGETINFO,"SQLGetInfo",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETINFOW,__api_SQLGETINFOW,"SQLGetInfoW",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLGETINFO,"SQLGetInfo",__api_SQLGETINFOW,"SQLGetInfoW"); 
  
/********************************************************************  Level 2 Functions        */

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLPARAMOPTIONS,__api_SQLPARAMOPTIONS,"SQLParamOptions",0,__flag_check_lt,XDBC_TRUE);

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBEPARAM_INT,__api_SQLDESCRIBEPARAM_INT,"SQLDescribeParam",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLDESCRIBEPARAM_LEN,__api_SQLDESCRIBEPARAM_LEN,"SQLDescribeParam",0,XDBC_FALSE,XDBC_FALSE);
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLNUMPARAMS,__api_SQLNUMPARAMS,"SQLNumParams",0,XDBC_TRUE,XDBC_TRUE);
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLBINDPARAMETER_INT,__api_SQLBINDPARAMETER_INT,"SQLBindParameter",0,XDBC_TRUE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLBINDPARAMETER_LEN,__api_SQLBINDPARAMETER_LEN,"SQLBindParameter",0,XDBC_FALSE,XDBC_FALSE);
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLPROCEDURECOLUMNS,__api_SQLPROCEDURECOLUMNS,"SQLProcedureColumns",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLPROCEDURECOLUMNSW,__api_SQLPROCEDURECOLUMNSW,"SQLProcedureColumnsW",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLPROCEDURECOLUMNS,"SQLProcedureColumns",__api_SQLPROCEDURECOLUMNSW,"SQLProcedureColumnsW"); 
  
/********************************************************************  ODBC3.x Functions        */

  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLALLOCHANDLE,__api_SQLALLOCHANDLE,"SQLAllocHandle",0,__flag_check_eqgt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFREEHANDLE,__api_SQLFREEHANDLE,"SQLFreeHandle",0,__flag_check_eqgt,XDBC_TRUE);  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLENDTRAN,__api_SQLENDTRAN,"SQLEndTran",0,__flag_check_eqgt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLFETCHSCROLL,__api_SQLFETCHSCROLL,"SQLFetchScroll",0,__flag_check_eqgt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETENVATTR,__api_SQLGETENVATTR,"SQLGetEnvAttr",0,__flag_check_eqgt,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETENVATTR,__api_SQLSETENVATTR,"SQLSetEnvAttr",0,__flag_check_eqgt,XDBC_TRUE);   
 
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLCLOSECURSOR,__api_SQLCLOSECURSOR,"SQLCloseCursor",0,XDBC_FALSE,XDBC_TRUE);
 
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETCONNECTATTR,__api_SQLGETCONNECTATTR,"SQLGetConnectAttr",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETCONNECTATTRW,__api_SQLGETCONNECTATTRW,"SQLGetConnectAttrW",0,XDBC_FALSE,XDBC_TRUE); 
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETCONNECTATTR,__api_SQLSETCONNECTATTR,"SQLSetConnectAttr",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETCONNECTATTRW,__api_SQLSETCONNECTATTRW,"SQLSetConnectAttrW",0,XDBC_FALSE,XDBC_TRUE); 
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETSTMTATTR,__api_SQLGETSTMTATTR,"SQLGetStmtAttr",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETSTMTATTRW,__api_SQLGETSTMTATTRW,"SQLGetStmtAttrW",0,XDBC_FALSE,XDBC_TRUE); 
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETSTMTATTR,__api_SQLSETSTMTATTR,"SQLSetStmtAttr",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLSETSTMTATTRW,__api_SQLSETSTMTATTRW,"SQLSetStmtAttrW",0,XDBC_FALSE,XDBC_TRUE);   
  
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETDIAGREC,__api_SQLGETDIAGREC,"SQLGetDiagRec",0,XDBC_FALSE,XDBC_TRUE);
  __XDBC_LOAD(__odbc_syslib,ODBCSQLAPI_SQLGETDIAGRECW,__api_SQLGETDIAGRECW,"SQLGetDiagRecW",0,XDBC_FALSE,XDBC_TRUE); 

  if(__flag_check_eqgt)
  {
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLFREESTMT,"SQLFreeStmt",__api_SQLCLOSECURSOR,"SQLCloseCursor");
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLGETCONNECTATTR,"SQLGetConnectAttr",__api_SQLGETCONNECTATTRW,"SQLGetConnectAttrW");
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLSETCONNECTATTR,"SQLSetConnectAttr",__api_SQLSETCONNECTATTRW,"SQLSetConnectAttrW");
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLGETSTMTATTR,"SQLGetStmtAttr",__api_SQLGETSTMTATTRW,"SQLGetStmtAttrW");
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLSETSTMTATTR,"SQLSetStmtAttr",__api_SQLSETSTMTATTRW,"SQLSetStmtAttrW");
  __XDBC_LOAD_DOUBLE_CHECK(__api_SQLGETDIAGREC,"SQLGetDiagRec",__api_SQLGETDIAGRECW,"SQLGetDiagRecW");     
  }

  return true;
}


IMPLEMENT_LIFETIME_BEGIN(OdbcDriverMapDoImpl)
IMPLEMENT_LIFETIME_END(OdbcDriverMapDoImpl)

OdbcDriverMapDoImpl::~OdbcDriverMapDoImpl()
{
  OPENCODE_MAP<string,OdbcDriver*>::iterator __pos_driver = __oc_map_drivers.begin();
  OPENCODE_MAP<string,OdbcDriver*>::iterator __end_driver = __oc_map_drivers.end();
  while(__pos_driver != __end_driver)
  {
    if(__pos_driver->second != 0)
    {
      delete __pos_driver->second;
    }  
    ++__pos_driver;
  }

  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*>::iterator __pos_options = __oc_map_options.begin();
  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*>::iterator __end_options = __oc_map_options.end();
  while(__pos_options != __end_options)
  {
    if(__pos_options->second != 0)
    {
      __pos_options->second->__options_syslib.close();
      delete __pos_options->second;
    }  
    ++__pos_options;
  }
}

OdbcDriverMapDoImpl::OdbcDriverMapDoImpl()
{
  // to set DriverManager::isTracingOn
  DriverMap::getInstance(); 
}

struct ODBC_DRIVER_OPTIONS*  OdbcDriverMapDoImpl::findOptions(const string& dsn,__XDBC_ATTRIBUTES& __xdbc_attributes)
{
  if( !DriverManager::findDatabaseAttributes(dsn,"odbc.ini","ODBC_CONFIG","ODBC.CONFIG",__xdbc_attributes))
  {
    return 0;
  }	
  	
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading);
  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*>::iterator __pos_options = __oc_map_options.find(dsn);
  if(__pos_options != __oc_map_options.end())
  {
    return (__pos_options->second);
  }
  
  return 0;
}

void   OdbcDriverMapDoImpl::registerOptions(const string& dsn,struct ODBC_DRIVER_OPTIONS* options)
{
  LOGGING_DEBUG("OdbcDriverManager::registerOptions|" << dsn);
 
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading);
  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*>::iterator __pos_options = __oc_map_options.find(dsn);
  if(__pos_options != __oc_map_options.end())
  {
    delete __pos_options->second;
    __pos_options->second = 0;
  }

  __oc_map_options[dsn] = options;
}
		
void   OdbcDriverMapDoImpl::deregisterOptions(const string& dsn)
{
  LOGGING_DEBUG("OdbcDriverManager::deregisterOptions|" << dsn);
  
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading); 
  OPENCODE_MAP<string,struct ODBC_DRIVER_OPTIONS*>::iterator __pos = __oc_map_options.find(dsn);
  if(__pos == __oc_map_options.end())
  {
    return;
  }

  struct ODBC_DRIVER_OPTIONS* __odbc_options = __pos->second;
  delete __odbc_options;
  __oc_map_options.erase(__pos);
}
  
OdbcDriver*  OdbcDriverMapDoImpl::findDriver(const string& libpathname)
{
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading);
  OPENCODE_MAP<string,OdbcDriver*>::iterator __pos = __oc_map_drivers.find(libpathname);

  if(__pos == __oc_map_drivers.end())
  {
    return 0;
  }

  return (__pos->second);
}

void  OdbcDriverMapDoImpl::registerDriver(const string& libpathname,OdbcDriver* driver)
{
  LOGGING_DEBUG("OdbcDriverManager::registerDriver|" << libpathname);
  
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading);
  OPENCODE_MAP<string,OdbcDriver*>::iterator __pos = __oc_map_drivers.find(libpathname);
  if(__pos != __oc_map_drivers.end())
  {
    delete __pos->second;
    __pos->second = 0;
  }

  __oc_map_drivers[libpathname] = driver;
}

void  OdbcDriverMapDoImpl::deregisterDriver(const string& libpathname)
{
  LOGGING_DEBUG("OdbcDriverManager::deregisterDriver|" << libpathname);
  
  Mutex::Guard g(__oc_dm_mutex,System::__M_multithreading); 
  OPENCODE_MAP<string,OdbcDriver*>::iterator __pos = __oc_map_drivers.find(libpathname);
  if(__pos == __oc_map_drivers.end())
  {
    return;
  }

  OdbcDriver* __p_driver = (__pos->second);
  delete __p_driver;
  __oc_map_drivers.erase(__pos);
}

void OdbcDriverManager::getFunctions(__XDBC_HOOK_DECLARE_MORE const string& dsn,__XDBC_ATTRIBUTES& xdbc_attributes,struct ODBC_DRIVER_OPTIONS*& odbcOptions,struct ODBC_LIBRARY_APIS*& odbcApis)__XDBC_THROW_DECLARE(SQLException)
{
  struct ODBC_DRIVER_OPTIONS* __p_options = OdbcDriverMap::getInstance().findOptions(dsn,xdbc_attributes);
	
  string __db_libpathname;
  __XDBC_ATTRIBUTES::iterator __pos = xdbc_attributes.begin();
  __XDBC_ATTRIBUTES::iterator __end = xdbc_attributes.end();
  	  
  while(__pos != __end)
  {
    if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"driver"))
    {
      __db_libpathname = StringsKit::trim(__pos->second);
      break;
    }   
    ++__pos;
  }

  LOGGING_DEBUG("OdbcDriverManager::getFunctions|" << __db_libpathname);

  OdbcDriver* __p_driver = OdbcDriverMap::getInstance().findDriver(__db_libpathname);

  if((__p_driver == 0) && !__db_libpathname.empty())
  {
    SystemLibrary __odbc_syslib;
    __OPENCODE_TRY
    {
      __odbc_syslib = SystemLibrary::getInstance(__db_libpathname);
    }
    __OPENCODE_CATCH(IOException& ex)
    {
      StringBuffer __sb;
      __sb << "[SQLException] openDriverLibrary - " << ex.getMessage();
      __XDBC_HOOK_RETURN(return,__sb.str());
    }
        
    __p_driver = new OdbcDriver(__odbc_syslib);
    smart_ptr<OdbcDriver> __sp_driver(__p_driver);
    __p_driver->load();    
    OdbcDriverMap::getInstance().registerDriver(__db_libpathname,__sp_driver.release());
  }

  if(__p_driver == 0)
  {
    StringBuffer __sb;
    __sb <<"OdbcDriver not found , DSN = " << dsn << " , libpathname = " << __db_libpathname;
    __XDBC_HOOK_RETURN(return,__sb.str());
  }  
  LOGGING_DEBUG("OdbcDriverManager::findDriver|libpathname=" << __db_libpathname);
  
  if(__p_options == 0)
  {
    __p_options = OdbcDriverManager::loadOptions(__p_driver,xdbc_attributes);
    OdbcDriverMap::getInstance().registerOptions(dsn,__p_options);
  }

  odbcApis    = __p_driver->__odbc_apis;
  odbcOptions = __p_options;
}

#define __XDBC_OPTIONS_TRACE(__xdbc_api_ptr,__xdbc_api_name)\
{\
  if(__odbc_options->__xdbc_api_ptr == 0)\
  {\
    StringBuffer __sb;\
    __sb << "checking options api '" << __xdbc_api_name <<"' ... FAILED";\
    __XDBC_TRACE("OdbcDriverManager",__sb.str());\
  }\
  else\
  {\
    StringBuffer __sb;\
    __sb << "checking options api '" << __xdbc_api_name << "' ... PASSED";\
    __XDBC_TRACE("OdbcDriverManager",__sb.str());\
  }\
}

#define __XDBC_OPTIONS_LOAD(__xdbc_api_soh,__xdbc_api_type,__xdbc_api_ptr,__xdbc_api_name,__xdbc_api_default,__xdbc_api_trace)\
{\
  __odbc_options->__xdbc_api_ptr      = (__xdbc_api_type)__xdbc_api_soh.findSymbol(__xdbc_api_name); \
  if(__xdbc_api_trace)\
  {\
    __XDBC_OPTIONS_TRACE(__xdbc_api_ptr,__xdbc_api_name);\
  }\
  if(__odbc_options->__xdbc_api_ptr == 0)\
  {\
     __odbc_options->__xdbc_api_ptr   = __xdbc_api_default; \
  }\
}

struct ODBC_DRIVER_OPTIONS* OdbcDriverManager::loadOptions(__XDBC_HOOK_DECLARE_MORE OdbcDriver* odbcDriver,__XDBC_ATTRIBUTES& __xdbc_attributes) __XDBC_THROW_DECLARE(SQLException)
{
  string __db_unicode("ANSI");
  string __db_options("BOTH");

  struct ODBC_DRIVER_OPTIONS* __odbc_options = new struct ODBC_DRIVER_OPTIONS();

  __odbc_options->__options_SUPPORTSCONNECTATTR = __default_odbcSupportsConnectAttr;
  __odbc_options->__options_SUPPORTSSTMTATTR    = __default_odbcSupportsStmtAttr;
  __odbc_options->__options_SUPPORTSENVATTR     = __default_odbcSupportsEnvAttr;
  __odbc_options->__options_ODBCGETTYPENAME     = __default_odbcGetTypeName;
  __odbc_options->__options_ADJUSTDEFINABLE     = __default_odbcAdjustDefinable;
  __odbc_options->__options_ADJUSTBINDABLE      = __default_odbcAdjustBindable; 
  __odbc_options->__options_ANSITOUNICODE       = __default_odbcAnsiToUnicode; 
  __odbc_options->__options_UNICODETOANSI       = __default_odbcUnicodeToAnsi; 
  __odbc_options->__options_FORMATSQL           = __default_odbcFormatSQL; 

  __odbc_options->__mb_fintAvailable   = false;
  __odbc_options->__mb_tbintAvailable  = true;
  __odbc_options->__mb_fntsAvailable   = true;

#if SQLAPI_INTEGER_OR_SQLLEN   == __ODBCAPI_OPTIONS_INTEGER
  __db_options = "INTEGER";
  __odbc_options->__mb_fintAvailable = true;
#elif SQLAPI_INTEGER_OR_SQLLEN == __ODBCAPI_OPTIONS_SQLLEN
  __db_options = "SQLLEN";
  __odbc_options->__mb_fintAvailable = false;
#else
  __db_options = "BOTH";
  __odbc_options->__mb_fintAvailable = false;
#endif

  __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_ASNI;
  __odbc_options->__mi_xlob_max        = 1024 * 200; //200k 

  string __db_opt_libpathname;
  SystemLibrary __options_syslib;
  __XDBC_ATTRIBUTES::const_iterator __pos = __xdbc_attributes.begin();
  __XDBC_ATTRIBUTES::const_iterator __end = __xdbc_attributes.end();
  while(__pos != __end)
  {
    if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.Driver"))
    {
      __db_opt_libpathname = StringsKit::trim(__pos->second);
      if(!__db_opt_libpathname.empty())
      {
        __OPENCODE_TRY
        {
          __options_syslib = SystemLibrary::getInstance(__db_opt_libpathname);
          __odbc_options->__options_syslib = __options_syslib;
        }
        __OPENCODE_CATCH(IOException& ex)
        {
          __options_syslib.close();
          
          StringBuffer __sb;
          __sb << "[SQLException] openOptionsLibrary - " << ex.getMessage();
          __XDBC_TRACE("OdbcDriverManager",__sb.str());
        }
      }
    }
#if SQLAPI_INTEGER_OR_SQLLEN   == __ODBCAPI_OPTIONS_BOTH
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.fintAvailable"))
    {
      string __db_fintAvailable = StringsKit::trim(__pos->second);
      if(   StringsKit::equalsIgnoreCase(__db_fintAvailable.c_str(),"YES")
         || StringsKit::equalsIgnoreCase(__db_fintAvailable.c_str(),"ON")
         || StringsKit::equalsIgnoreCase(__db_fintAvailable.c_str(),"1"))
      {
        __odbc_options->__mb_fintAvailable = true;
      }
    }
#endif
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.fntsAvailable"))
    {
      string __db_fntsAvailable = StringsKit::trim(__pos->second);
      if(   StringsKit::equalsIgnoreCase(__db_fntsAvailable.c_str(),"NO")
         || StringsKit::equalsIgnoreCase(__db_fntsAvailable.c_str(),"OFF")
         || StringsKit::equalsIgnoreCase(__db_fntsAvailable.c_str(),"0"))
      {
        __odbc_options->__mb_fntsAvailable = false;
      }
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.tbintAvailable"))
    {
      string __db_tbintAvailable = StringsKit::trim(__pos->second);
      if(   StringsKit::equalsIgnoreCase(__db_tbintAvailable.c_str(),"NO")
         || StringsKit::equalsIgnoreCase(__db_tbintAvailable.c_str(),"OFF")
         || StringsKit::equalsIgnoreCase(__db_tbintAvailable.c_str(),"0"))
      {
        __odbc_options->__mb_tbintAvailable = false;
      }
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.Unicode"))
    {
      __db_unicode = StringsKit::trim(__pos->second);
      if(StringsKit::equalsIgnoreCase(__db_unicode.c_str(),"UTF8"))
      {
        __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_UTF8;
        __db_unicode = "UTF8";
      }
      else if(StringsKit::equalsIgnoreCase(__db_unicode.c_str(),"UTF16"))
      {
        __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_UTF16;
        __db_unicode = "UTF16";
      }
      else if(StringsKit::equalsIgnoreCase(__db_unicode.c_str(),"UTF32"))
      {
        __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_UTF32;
        __db_unicode = "UTF32";
      }
      else
      {
        __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_ASNI;
        __db_unicode = "ANSI";
      }
    }
    else if(StringsKit::equalsIgnoreCase(__pos->first.c_str(),"OdbcOptions.XlobMax"))
    {
      string __tmp_lobmax = StringsKit::toLowerCase(StringsKit::trim(__pos->second));
      XLONG multiplier = 1;
      size_t index;
  
      if((index = __tmp_lobmax.find(("kb"))) != string::npos)
      {
        multiplier = 1024;
        __tmp_lobmax = __tmp_lobmax.substr(0, index);
      }
      else if((index = __tmp_lobmax.find(("mb"))) != string::npos) 
      {
        multiplier = 1024*1024;
        __tmp_lobmax = __tmp_lobmax.substr(0, index);
      }

      if(!__tmp_lobmax.empty())
      {
        __OPENCODE_TRY
        {
          __odbc_options->__mi_xlob_max = Number::parseInt(__tmp_lobmax.c_str()) * multiplier;
        }
        __OPENCODE_CATCH(NumberFormatException& ex)
        {
        }	  	
      }
    }

    ++__pos;
  }

#if (ODBCVER >= 0x0300)
  if((odbcDriver!= 0) && (odbcDriver->__odbc_apis->__api_SQLDRIVERCONNECTW != 0))
  {
    if(__odbc_options->__mi_driver_unicode == __ODBC_UNICODE_ASNI)
    {
      __odbc_options->__mi_driver_unicode  = __ODBC_UNICODE_UTF16;
      __db_unicode = "UTF16";
    }
  }
#endif

  if(DriverManager::isTracingOn)
  {
    StringBuffer __sb;
    if(__options_syslib.isOpened())
    {
      __sb << "checking options form  DriverOptions '" << __options_syslib.getPathname() << "' : " ;
    }
    else
    {
      __sb << "checking options form  DriverOptions  : " ;
    }

    __sb << "__mi_driver_unicode = <<" 
         << __db_unicode
         << ">>, __db_options = <<" 
         << __db_options
         << ">>, __mb_fintAvailable = <<"
         << __odbc_options->__mb_fintAvailable
         << ">>, __mb_fntsAvailable = <<"
         << __odbc_options->__mb_fntsAvailable         
         << ">>, __mb_tbintAvailable = <<"
         << __odbc_options->__mb_tbintAvailable
         << ">>, __mi_xlob_max = <<"
         << __odbc_options->__mi_xlob_max
         << ">>" <<File::lineSeparator;
                                                  
    __XDBC_FORCED_TRACE("OdbcDriverManager",__sb.str()); 
  } 
   
  if(__options_syslib.isOpened())
  {
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_SUPPORTSCONNECTATTR,__options_SUPPORTSCONNECTATTR,"odbcSupportsConnectAttr",__default_odbcSupportsConnectAttr,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_SUPPORTSSTMTATTR,__options_SUPPORTSSTMTATTR,"odbcSupportsStmtAttr",__default_odbcSupportsStmtAttr,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_SUPPORTSENVATTR,__options_SUPPORTSENVATTR,"odbcSupportsEnvAttr",__default_odbcSupportsEnvAttr,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_ODBCGETTYPENAME,__options_ODBCGETTYPENAME,"odbcGetTypeName",__default_odbcGetTypeName,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_ADJUSTDEFINABLE,__options_ADJUSTDEFINABLE,"odbcAdjustDefinable",__default_odbcAdjustDefinable,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_ADJUSTBINDABLE,__options_ADJUSTBINDABLE,"odbcAdjustBindable",__default_odbcAdjustBindable,XDBC_TRUE);
	  
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_ANSITOUNICODE,__options_ANSITOUNICODE,"odbcAnsiToUnicode",__default_odbcAnsiToUnicode,XDBC_TRUE);
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_UNICODETOANSI,__options_UNICODETOANSI,"odbcUnicodeToAnsi",__default_odbcUnicodeToAnsi,XDBC_TRUE);
	  
	  __XDBC_OPTIONS_LOAD(__options_syslib,ODBCSQLOPTIONS_FORMATSQL,__options_FORMATSQL,"odbcFormatSQL",__default_odbcFormatSQL,XDBC_TRUE);
  }
  
  return __odbc_options;    	
}

//------------------------------------------------------
// ODBC_PARAM Class 
//------------------------------------------------------
ODBC_PARAM::ODBC_PARAM()
{
  __mi_index  =  0;
  
  __mi_length  =  0;
  __mi_type  =  0;
  __mi_precision = 0;
  __mi_scale = 0;
  __mi_nullable  =  0;

  __mb_stored_proc =  false;                                                                                                     
  __mi_iomode = SQL_PARAM_INPUT_OUTPUT;                                                                                                           
  __mi_NUM_PREC_RADIX = 10;  
  __mi_SQL_DATA_TYPE = 0;                                             
  __mi_SQL_DATETIME_SUB = 0;                                         
  __mi_CHAR_OCTET_LENGTH = 0;                                                                                       
  __mi_IS_NULLABLE = 0; 
  
  __mi_bind_size = 0;
  __mi_bind_ind_int = SQL_NO_NULLS;
  __mi_bind_ind_len = SQL_NO_NULLS;
  __mb_has_bound = false;
  __mi_bind_type = 0; 
  __mi_buf_offset = 0;
  __mi_buf_size = 0; 
}

ODBC_PARAM::ODBC_PARAM(const ODBC_PARAM& rv)
{
  __mi_index              = rv.__mi_index;
                          
  __mi_length             = rv.__mi_length;
  __mi_type               = rv.__mi_type;
  __mi_precision          = rv.__mi_precision;
  __mi_scale              = rv.__mi_scale;
  __mi_nullable           = rv.__mi_nullable;
                          
  __ms_name               = rv.__ms_name;
  __ms_schema_name        = rv.__ms_schema_name;
  __ms_type_name          = rv.__ms_type_name;

  __mb_stored_proc        = rv.__mb_stored_proc;
  __ms_catalog_name       = rv.__mb_stored_proc;                                                                                            
  __ms_proc_name          = rv.__ms_proc_name;                                                                                                         
  __mi_iomode             = rv.__mi_iomode;                                                                                                           
  __mi_NUM_PREC_RADIX     = rv.__mi_NUM_PREC_RADIX;                                                                                                   
  __ms_REMARK             = rv.__mi_NUM_PREC_RADIX;                                                        
  __ms_COLUMN_DEF         = rv.__ms_COLUMN_DEF;                                              
  __mi_SQL_DATA_TYPE      = rv.__mi_SQL_DATA_TYPE;                                             
  __mi_SQL_DATETIME_SUB   = rv.__mi_SQL_DATETIME_SUB;                                         
  __mi_CHAR_OCTET_LENGTH  = rv.__mi_CHAR_OCTET_LENGTH;                                                                                       
  __mi_IS_NULLABLE        = rv.__mi_IS_NULLABLE;   

  __mu_value              = rv.__mu_value; 
  __mi_bind_size          = rv.__mi_bind_size;
  __mi_bind_ind_int       = rv.__mi_bind_ind_int;
  __mi_bind_ind_len       = rv.__mi_bind_ind_len;
  __mb_has_bound          = rv.__mb_has_bound;
  __mi_bind_type          = rv.__mi_bind_type;                          
  __mi_buf_offset         = rv.__mi_buf_offset;
  __mi_buf_size           = rv.__mi_buf_size;  

  __tmp_std_str           = rv.__tmp_std_str;
}

ODBC_PARAM& ODBC_PARAM::operator = (const ODBC_PARAM& rv)
{
  if(&rv  ==  this)
  {
    return *this;
  }

   __mi_index             = rv.__mi_index;
                          
  __mi_length             = rv.__mi_length;
  __mi_type               = rv.__mi_type;
  __mi_precision          = rv.__mi_precision;
  __mi_scale              = rv.__mi_scale;
  __mi_nullable           = rv.__mi_nullable;
                          
  __ms_name               = rv.__ms_name;
  __ms_schema_name        = rv.__ms_schema_name;
  __ms_type_name          = rv.__ms_type_name;

  __mb_stored_proc        = rv.__mb_stored_proc;
  __ms_catalog_name       = rv.__mb_stored_proc;                                                                                            
  __ms_proc_name          = rv.__ms_proc_name;                                                                                                         
  __mi_iomode             = rv.__mi_iomode;                                                                                                           
  __mi_NUM_PREC_RADIX     = rv.__mi_NUM_PREC_RADIX;                                                                                                   
  __ms_REMARK             = rv.__mi_NUM_PREC_RADIX;                                                        
  __ms_COLUMN_DEF         = rv.__ms_COLUMN_DEF;                                              
  __mi_SQL_DATA_TYPE      = rv.__mi_SQL_DATA_TYPE;                                             
  __mi_SQL_DATETIME_SUB   = rv.__mi_SQL_DATETIME_SUB;                                         
  __mi_CHAR_OCTET_LENGTH  = rv.__mi_CHAR_OCTET_LENGTH;                                                                                       
  __mi_IS_NULLABLE        = rv.__mi_IS_NULLABLE;   

  __mu_value              = rv.__mu_value; 
  __mi_bind_size          = rv.__mi_bind_size;
  __mi_bind_ind_int       = rv.__mi_bind_ind_int;
  __mi_bind_ind_len       = rv.__mi_bind_ind_len;
  __mb_has_bound          = rv.__mb_has_bound;
  __mi_bind_type          = rv.__mi_bind_type;                          
  __mi_buf_offset         = rv.__mi_buf_offset;
  __mi_buf_size           = rv.__mi_buf_size;  

  __tmp_std_str           = rv.__tmp_std_str;
    
  return *this;
}


void ODBC_PARAM::dump(StringBuffer& __sb)
{
  __sb << "-------------------------------------------" << File::lineSeparator;
 
	if(__mb_stored_proc)
	{
          __sb << "__mi_index              : " << __mi_index << File::lineSeparator;

	  __sb << "__ms_catalog_name       : " << __ms_catalog_name << File::lineSeparator;
	  __sb << "__ms_schema_name        : " << __ms_schema_name << File::lineSeparator;
	  __sb << "__ms_proc_name          : " << __ms_proc_name << File::lineSeparator;
	  __sb << "__ms_name               : " << __ms_name << File::lineSeparator;
	  __sb << "__mi_iomode             : " << __mi_iomode << File::lineSeparator;
	  __sb << "__mi_type               : " << __mi_type << File::lineSeparator;
	    
	  __sb << "__ms_type_name          : " << __ms_type_name << File::lineSeparator;
	  __sb << "__mi_precision          : " << __mi_precision << File::lineSeparator;
	  __sb << "__mi_length             : " << __mi_length << File::lineSeparator;
	  __sb << "__mi_scale              : " << __mi_scale << File::lineSeparator;
	  __sb << "__mi_NUM_PREC_RADIX     : " << __mi_NUM_PREC_RADIX << File::lineSeparator;
	  __sb << "__mi_nullable           : " << __mi_nullable << File::lineSeparator;
	  
	  __sb << "__ms_REMARK             : " << __ms_REMARK << File::lineSeparator;
	  __sb << "__ms_COLUMN_DEF         : " << __ms_COLUMN_DEF << File::lineSeparator;
	  __sb << "__mi_SQL_DATA_TYPE      : " << __mi_SQL_DATA_TYPE << File::lineSeparator;
	  __sb << "__mi_SQL_DATETIME_SUB   : " << __mi_SQL_DATETIME_SUB << File::lineSeparator;
	  __sb << "__mi_CHAR_OCTET_LENGTH  : " << __mi_CHAR_OCTET_LENGTH << File::lineSeparator;
	  __sb << "__mi_IS_NULLABLE        : " << __mi_IS_NULLABLE << File::lineSeparator;
  }
  else
  {
	  __sb << "__mi_index              : " << __mi_index << File::lineSeparator;
          __sb << "__mi_iomode             : " << __mi_iomode << File::lineSeparator;
	  __sb << "__mi_type               : " << __mi_type << File::lineSeparator;
	  __sb << "__ms_type_name          : " << __ms_type_name << File::lineSeparator;
	  __sb << "__mi_length             : " << __mi_length << File::lineSeparator;
	  __sb << "__mi_precision          : " << __mi_precision << File::lineSeparator;
	  __sb << "__mi_scale              : " << __mi_scale << File::lineSeparator;
	  __sb << "__ms_schema_name        : " << __ms_schema_name << File::lineSeparator;
  }
}

//---------------------------------------------------
// ODBC_COLUMN Class 
//---------------------------------------------------

ODBC_COLUMN::ODBC_COLUMN()
{
  __mi_index = -1;

  __mi_length = 0;
  __mi_type = 0;
  __mi_nullable = 0;
  __mi_precision = 0;
  __mi_scale = 0;
  
  __mb_char_semantics = false;
}

ODBC_COLUMN::ODBC_COLUMN(const ODBC_COLUMN& rv)
{
  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_nullable = rv.__mi_nullable;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;  

  __mb_char_semantics = rv.__mb_char_semantics;
  __ms_table_name = rv.__ms_table_name;
}

ODBC_COLUMN& ODBC_COLUMN::operator = (const ODBC_COLUMN& rv)
{
  if(&rv  ==  this)
  {
    return *this;
  }


  __mi_index = rv.__mi_index;

  __mi_length = rv.__mi_length;
  __mi_type = rv.__mi_type;
  __mi_nullable = rv.__mi_nullable;
  __mi_precision = rv.__mi_precision;
  __mi_scale = rv.__mi_scale;

  __ms_name = rv.__ms_name;
  __ms_schema_name = rv.__ms_schema_name;
  __ms_type_name = rv.__ms_type_name;

  __mb_char_semantics = rv.__mb_char_semantics;
  __ms_table_name = rv.__ms_table_name;

  return *this;
}

void ODBC_COLUMN::dump(StringBuffer& __sb)
{
  __sb << "-------------------------------------------" << File::lineSeparator;
  __sb << "__mi_index          : " << __mi_index << File::lineSeparator;

  __sb << "__ms_name           : " << __ms_name << File::lineSeparator;
  __sb << "__mi_type           : " << __mi_type << File::lineSeparator;
  __sb << "__ms_type_name      : " << __ms_type_name << File::lineSeparator;
  __sb << "__mi_length         : " << __mi_length << File::lineSeparator;
  __sb << "__mi_nullable       : " << __mi_nullable << File::lineSeparator;
  __sb << "__mi_precision      : " << __mi_precision << File::lineSeparator; 
  __sb << "__mi_scale          : " << __mi_scale << File::lineSeparator;

  __sb << "__mb_char_semantics : " << __mb_char_semantics << File::lineSeparator;  
  __sb << "__ms_schema_name    : " << __ms_schema_name << File::lineSeparator;
  __sb << "__ms_table_name     : " << __ms_table_name << File::lineSeparator;
}

ODBC_VALUE::ODBC_VALUE()
{
  memset(&odbc_TIMESTAMP,0,sizeof(odbc_TIMESTAMP));
  memset(&odbc_DATE,0,sizeof(odbc_DATE));
  memset(&odbc_TIME,0,sizeof(odbc_TIME));

  memset(&odbc_NUMBERIC,0,sizeof(odbc_NUMBERIC));
  odbc_BIGINT    =  0;
}

ODBC_VALUE::ODBC_VALUE(const ODBC_VALUE& rv)
{
  odbc_TIMESTAMP  =  rv.odbc_TIMESTAMP;
  odbc_DATE  =  rv.odbc_DATE;
  odbc_TIME  =  rv.odbc_TIME;

  odbc_NUMBERIC  =  rv.odbc_NUMBERIC;
  odbc_BIGINT    =  rv.odbc_BIGINT;
}

ODBC_VALUE& ODBC_VALUE::operator = (const ODBC_VALUE& rv)
{
  if(&rv  ==  this)
  {
    return *this;
  }
  
  
  odbc_TIMESTAMP  =  rv.odbc_TIMESTAMP;
  odbc_DATE  =  rv.odbc_DATE;
  odbc_TIME  =  rv.odbc_TIME;

  odbc_NUMBERIC  =  rv.odbc_NUMBERIC;
  odbc_BIGINT    =  rv.odbc_BIGINT;
 
  return *this;
}

ODBC_DEFINE::ODBC_DEFINE()
{
  __mi_define_type  = 0;
  __mi_define_c_type  =  0;
  __mi_define_size  =  0;
  __mi_define_ind_int  =  0;

  __mi_buf_offset  =  0;
  __mi_buf_size  =  0;
}

ODBC_DEFINE::ODBC_DEFINE(const ODBC_DEFINE& rv)
{ 
  __mi_define_type  = rv.__mi_define_type;
  __mi_define_c_type  =  rv.__mi_define_c_type;
  __mi_define_size = rv.__mi_define_size;
  __mi_define_ind_int = rv.__mi_define_ind_int;

  __mu_value  =  rv.__mu_value;

  __mi_buf_offset = rv.__mi_buf_offset;
  __mi_buf_size = rv.__mi_buf_size;
}

ODBC_DEFINE& ODBC_DEFINE::operator = (const ODBC_DEFINE& rv)
{
  if(&rv  ==  this)
  {
    return *this;
  }
 
  __mi_define_type  = rv.__mi_define_type;
  __mi_define_c_type  =  rv.__mi_define_c_type;
  __mi_define_size = rv.__mi_define_size;
  __mi_define_ind_int = rv.__mi_define_ind_int;

  __mu_value  =  rv.__mu_value;

  __mi_buf_offset = rv.__mi_buf_offset;
  __mi_buf_size = rv.__mi_buf_size;

  return *this;
}

OdbcResultSetCollection::~OdbcResultSetCollection() 
{
  if(__p_rowbuf != 0)
  {
    delete []__p_rowbuf;
  }  
}
  
OdbcResultSetCollection::OdbcResultSetCollection()
{
  __p_rowbuf = 0;
  __rowbuf_size = 0;
}

OdbcResultSetCollection::OdbcResultSetCollection(const OdbcResultSetCollection& rv)
{
  __vector_defines = rv.__vector_defines;
  __vector_columns = rv.__vector_columns;  
  __map_columns = rv.__map_columns; 
    
  
  if(__p_rowbuf != 0)
  {
    delete []__p_rowbuf;
    __p_rowbuf = 0;
    __rowbuf_size = 0;
  } 

  
  if(__rowbuf_size > 0)
  {
    __rowbuf_size = rv.__rowbuf_size;
    __p_rowbuf = new char[__rowbuf_size];
    memcpy(__p_rowbuf,rv.__p_rowbuf,__rowbuf_size);
  }    
}

OdbcResultSetCollection& OdbcResultSetCollection::operator=(const OdbcResultSetCollection& rv)
{
  if(this == &rv)
  {
    return *this;
  }  

  __vector_defines = rv.__vector_defines;
  __vector_columns = rv.__vector_columns;  
  __map_columns    = rv.__map_columns; 
    
  if(__p_rowbuf != 0 )
  {
    delete []__p_rowbuf;
    __p_rowbuf = 0;
    __rowbuf_size = 0;
  }

  
  
  if(__rowbuf_size > 0)
  {
    __rowbuf_size = rv.__rowbuf_size;
    __p_rowbuf = new char[__rowbuf_size];
    memcpy(__p_rowbuf,rv.__p_rowbuf,__rowbuf_size);
  } 
    
  return *this;
}


void OdbcConverter::setXdbcDate(XDBC_DATE& xdbcDate,const SQL_DATE_STRUCT& odbcDate)
{
  xdbcDate.year     = odbcDate.year;
  xdbcDate.month    = odbcDate.month;
  xdbcDate.day      = odbcDate.day;
}

void OdbcConverter::setOdbcDate(SQL_DATE_STRUCT& odbcDate,const XDBC_DATE& xdbcDate)
{
  odbcDate.year     = xdbcDate.year;
  odbcDate.month    = xdbcDate.month;
  odbcDate.day      = xdbcDate.day;
}


void OdbcConverter::setXdbcTime(XDBC_TIME& xdbcTime,const SQL_TIME_STRUCT& odbcTime)
{
  xdbcTime.hour     = odbcTime.hour;
  xdbcTime.minute   = odbcTime.minute;
  xdbcTime.second   = odbcTime.second;  
}

void OdbcConverter::setOdbcTime(SQL_TIME_STRUCT& odbcTime,const XDBC_TIME& xdbcTime)
{
  odbcTime.hour     = xdbcTime.hour;
  odbcTime.minute   = xdbcTime.minute;
  odbcTime.second   = xdbcTime.second;  
}


void OdbcConverter::setXdbcInterval(XDBC_INTERVAL& xdbcInterval,const SQL_INTERVAL_STRUCT& odbcInterval)
{
  switch(odbcInterval.interval_type)
  {
    case SQL_IS_YEAR:
    case SQL_IS_MONTH:
    case SQL_IS_YEAR_TO_MONTH:
    {
      xdbcInterval.interval_type           = (XDBCINTERVAL)odbcInterval.interval_type;
      xdbcInterval.interval_sign           = odbcInterval.interval_sign;
      xdbcInterval.intval.year_month.year  = odbcInterval.intval.year_month.year;
      xdbcInterval.intval.year_month.month = odbcInterval.intval.year_month.month;
    }
    break;
    
    default:
    {
      xdbcInterval.interval_type              = (XDBCINTERVAL)odbcInterval.interval_type;
      xdbcInterval.interval_sign              = odbcInterval.interval_sign;
      xdbcInterval.intval.day_second.day      = odbcInterval.intval.day_second.day;    
      xdbcInterval.intval.day_second.hour     = odbcInterval.intval.day_second.hour; 
      xdbcInterval.intval.day_second.minute   = odbcInterval.intval.day_second.minute; 
      xdbcInterval.intval.day_second.second   = odbcInterval.intval.day_second.second; 
      xdbcInterval.intval.day_second.fraction = odbcInterval.intval.day_second.fraction;  
    }   
  } 
}

void OdbcConverter::setOdbcInterval(SQL_INTERVAL_STRUCT& odbcInterval,const XDBC_INTERVAL& xdbcInterval)
{
  switch(xdbcInterval.interval_type)
  {
    case XDBC_IS_YEAR:
    case XDBC_IS_MONTH:
    case XDBC_IS_YEAR_TO_MONTH:
    {
      odbcInterval.interval_type           = (SQLINTERVAL)xdbcInterval.interval_type;
      odbcInterval.interval_sign           = xdbcInterval.interval_sign;
      odbcInterval.intval.year_month.year  = xdbcInterval.intval.year_month.year;
      odbcInterval.intval.year_month.month = xdbcInterval.intval.year_month.month;
    }
    break;
    
    default:
    {
      odbcInterval.interval_type              = (SQLINTERVAL)xdbcInterval.interval_type;
      odbcInterval.interval_sign              = xdbcInterval.interval_sign;
      odbcInterval.intval.day_second.day      = xdbcInterval.intval.day_second.day;    
      odbcInterval.intval.day_second.hour     = xdbcInterval.intval.day_second.hour; 
      odbcInterval.intval.day_second.minute   = xdbcInterval.intval.day_second.minute; 
      odbcInterval.intval.day_second.second   = xdbcInterval.intval.day_second.second; 
      odbcInterval.intval.day_second.fraction = xdbcInterval.intval.day_second.fraction;    
    }   
  }
}

void OdbcConverter::setXdbcTimestamp(XDBC_TIMESTAMP& xdbcTimestamp,const SQL_TIMESTAMP_STRUCT& odbcTimestamp,ODBC_COLUMN* odbcColumn)
{
  xdbcTimestamp.year      = odbcTimestamp.year;
  xdbcTimestamp.month     = odbcTimestamp.month;
  xdbcTimestamp.day       = odbcTimestamp.day;
  xdbcTimestamp.hour      = odbcTimestamp.hour;
  xdbcTimestamp.minute    = odbcTimestamp.minute;
  xdbcTimestamp.second    = odbcTimestamp.second;
  xdbcTimestamp.fraction  = 0;

  switch(odbcColumn->__mi_precision)
  {
    case 26://TIMESTAMP
    {
      if(odbcTimestamp.fraction > 0)
      {
        xdbcTimestamp.fraction = odbcTimestamp.fraction * 1000;
      }
      break;
    }

    case 20: //DATETIME
    default:
    {
      break;
    }

  } //~end switch(odbcColumn->__mi_precision)
}

void OdbcConverter::setXdbcTimestamp(XDBC_TIMESTAMP& xdbcTimestamp,const SQL_TIMESTAMP_STRUCT& odbcTimestamp,ODBC_PARAM* odbcParam)
{
  xdbcTimestamp.year     = odbcTimestamp.year;
  xdbcTimestamp.month    = odbcTimestamp.month;
  xdbcTimestamp.day      = odbcTimestamp.day;
  xdbcTimestamp.hour     = odbcTimestamp.hour;
  xdbcTimestamp.minute   = odbcTimestamp.minute;
  xdbcTimestamp.second   = odbcTimestamp.second;
  xdbcTimestamp.fraction = 0;

  switch(odbcParam->__mi_precision)
  {
    case 26://TIMESTAMP
    {
      if(odbcTimestamp.fraction > 0)
      {
        xdbcTimestamp.fraction = odbcTimestamp.fraction * 1000;
      }
      break;
    }

    case 20: //DATETIME
    default:
    {
      break;
    }

  } //~end switch(odbcParam->__mi_precision)
}

void OdbcConverter::setOdbcTimestamp(TIMESTAMP_STRUCT& odbcTimestamp,const XDBC_TIMESTAMP& xdbcTimestamp,ODBC_PARAM* odbcParam)
{
  odbcTimestamp.year     = xdbcTimestamp.year;
  odbcTimestamp.month    = xdbcTimestamp.month;
  odbcTimestamp.day      = xdbcTimestamp.day;
  odbcTimestamp.hour     = xdbcTimestamp.hour;
  odbcTimestamp.minute   = xdbcTimestamp.minute;
  odbcTimestamp.second   = xdbcTimestamp.second;
  odbcTimestamp.fraction = 0;

  switch(odbcParam->__mi_precision)
  {
    case 26://TIMESTAMP
    {
      if(xdbcTimestamp.fraction > 0)
      {
        odbcTimestamp.fraction = xdbcTimestamp.fraction / 1000;
      }
      break;
    }

    case 20: //DATETIME
    default:
    {
      break;
    }

  } //~end switch(odbcParam->__mi_precision)
}


void OdbcConverter::setXdbcNumberic(XDBC_NUMERIC& xdbcNumberic,const SQL_NUMERIC_STRUCT& odbcNumberic)
{
  memcpy(xdbcNumberic.val,odbcNumberic.val,__M_min(XDBC_MAX_NUMERIC_LEN,SQL_MAX_NUMERIC_LEN));
  xdbcNumberic.sign      = odbcNumberic.sign;
  xdbcNumberic.precision = odbcNumberic.precision;
  xdbcNumberic.scale     = odbcNumberic.scale; 
}

void OdbcConverter::setOdbcNumberic(SQL_NUMERIC_STRUCT& odbcNumberic, const XDBC_NUMERIC& xdbcNumberic)
{
  memcpy(odbcNumberic.val,xdbcNumberic.val,__M_min(XDBC_MAX_NUMERIC_LEN,SQL_MAX_NUMERIC_LEN));
  odbcNumberic.sign      = xdbcNumberic.sign;
  odbcNumberic.precision = xdbcNumberic.precision;
  odbcNumberic.scale     = xdbcNumberic.scale; 
}
  
void OdbcKit::checkException(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_hstmt,SQLRETURN __odbc_sqlreturn,const char* __className) __XDBC_THROW_DECLARE(SQLException)
{
  //printf("OdbcKit::checkException - __odbc_sqlreturn = %d\n",__odbc_sqlreturn);

  if(SQL_SUCCESS == __odbc_sqlreturn)
  {
    return; 
  }
  
  StringBuffer __sb;
  if(__className != 0)
  {
    __sb << "["<<__className << "] - ";
  } 
 
  int __maxsize = SQL_MAX_MESSAGE_LENGTH - 1;
  SQLINTEGER __NativeError = 0;

  SQLCHAR __Sqlstate[SQL_MAX_MESSAGE_LENGTH - 1]  = {0};
  SQLSMALLINT __StateLength = __maxsize;
  SQLSMALLINT __StateLengthPtr = __maxsize;

  SQLCHAR __MessageText[SQL_MAX_MESSAGE_LENGTH - 1] = {0};
  SQLSMALLINT __BufferLength = __maxsize;
  SQLSMALLINT __BufferLengthPtr = __maxsize;

  switch (__odbc_sqlreturn)
  {    
    case SQL_INVALID_HANDLE:
    {
      __sb << "SQL_INVALID_HANDLE";
    }
    break;
    case SQL_NEED_DATA:
    {
      __sb << "SQL_NEED_DATA";
    }
    break;
  }

  switch (__odbc_sqlreturn)
  {    
    case SQL_ERROR:  
    //case SQL_SUCCESS_WITH_INFO:
    case SQL_NEED_DATA:
    case SQL_INVALID_HANDLE:
    {
      SQLSMALLINT handle_type = SQL_HANDLE_STMT; 
      SQLHANDLE   handle = odbc_hstmt;       
      if(odbc_hstmt != SQL_NULL_HSTMT)
      {
        handle_type = SQL_HANDLE_STMT; 
        handle = odbc_hstmt;
      } 
      else if (odbc_conn->odbc_hdbc != SQL_NULL_HDBC)
      {
        handle_type = SQL_HANDLE_DBC; 
        handle = odbc_conn->odbc_hdbc;          
      }   
      else
      {
        handle_type = SQL_HANDLE_ENV; 
        handle = odbc_conn->odbc_henv;              
      }  
      
      __odbc_sqlreturn = __odbc_SQLGetDiagRec(odbc_conn,handle_type,handle,1,__Sqlstate,__StateLength,&__StateLengthPtr,&__NativeError,__MessageText,__BufferLength,&__BufferLengthPtr);      
      printf("__odbc_SQLGetDiagRec - __odbc_sqlreturn = %d\n",__odbc_sqlreturn);
      string __s_Sqlstate((const char*)__Sqlstate);
      __sb.append((const char*)__MessageText);
      __XDBC_HOOK_RETURN(return,__sb.str(),__s_Sqlstate,__NativeError);         
    }
    break; 

    case SQL_NO_DATA_FOUND:
    default:
    {
    }
    break;
  }  
} 

bool OdbcKit::formatSQL(struct ODBC_DRIVER_OPTIONS* odbc_options,const string& inputSQL,string& outputSQL)
{
  ODBCSQLOPTIONS_FORMATSQL __options_FORMATSQL = odbc_options->__options_FORMATSQL;
  
  int __fsql_SQLBufferSize = (int)inputSQL.size() * 2;
  char* __fsql_SQLBuffer = new char[__fsql_SQLBufferSize];
  SQLINTEGER __fsql_outBufferSize = 0;
  SQLRETURN __fsql_return = (__options_FORMATSQL)((SQLCHAR*)inputSQL.data(),inputSQL.size(),(SQLCHAR*)__fsql_SQLBuffer,&__fsql_outBufferSize);
  if(__fsql_return == SQL_SUCCESS)
  {
  	outputSQL.assign(__fsql_SQLBuffer,__fsql_outBufferSize);
  }
  else
  {
  	outputSQL = inputSQL;
	}	
  	
  delete []__fsql_SQLBuffer;
  
  return (__fsql_return == SQL_SUCCESS);
}

void OdbcKit::parseResultSetMetaData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_stmthp,vector<ODBC_COLUMN>& vector_columns) __XDBC_THROW_DECLARE(SQLException)
{ 
  SQLSMALLINT numcols = 0;
  SQLRETURN __odbc_sqlreturn = 0;
  
  /* Get the number of columns in the query */
  __odbc_sqlreturn = (odbc_conn->__odbc_apis->__api_SQLNUMRESULTCOLS)(odbc_stmthp, &numcols);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE odbc_conn,odbc_stmthp,__odbc_sqlreturn,"parseResultSetMetaData");
    __XDBC_HOOK_CHECK(return);
  }
  
  vector_columns.resize(numcols + 1);
     
  SQLCHAR       colname [1024] = {0};
  SQLSMALLINT   colname_size_in = 1024;
  SQLSMALLINT   colname_size_out = 1024;
  
  StringBuffer __sb;
  if(DriverManager::isTracingOn)
  {
    __sb << "parseResultSetMetaData|" << File::lineSeparator;
  }

  SQLSMALLINT  __col_type = 0;
  SQLUINTEGER  __col_len = 0;
  SQLSMALLINT  __col_scale = 0;
  SQLSMALLINT  __col_nullable = 0;
  
  OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
  struct ODBC_DRIVER_OPTIONS* __odbc_options = odbc_conn->__odbc_options;
 
  for(int i = 1;i <= numcols; ++i)
  {
    ODBC_COLUMN& odbc_column = vector_columns[i];
    odbc_column.__mb_char_semantics = false;
    odbc_column.__mi_index = i;
    
    __odbc_sqlreturn = __odbc_SQLDescribeCol(odbc_conn,odbc_stmthp, 
                                             (SQLSMALLINT) i,
                                             colname,
                                             (SQLSMALLINT)colname_size_in,
                                             &colname_size_out, &(__col_type),
                                             &(__col_len),
                                             &(__col_scale),
                                             &(__col_nullable));

    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {                                                      
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE odbc_conn,odbc_stmthp,__odbc_sqlreturn,"parseResultSetMetaData");
      __XDBC_HOOK_CHECK(return);
    }

    odbc_column.__mi_type     = __col_type;
    odbc_column.__mi_length   = __col_len ;
    odbc_column.__mi_scale    = __col_scale;
    odbc_column.__mi_nullable = __col_nullable;
                     
    odbc_column.__ms_type_name = __odbcTypes->getTypeName(__odbc_options,vector_columns[i]);
    odbc_column.__mi_precision = odbc_column.__mi_length;

    size_t separator = 0;
    const char *temp_str = 0;
    colname[colname_size_out] = '\0';
    const char* __P_ms_name = (const char*)colname;  
 
    if ((temp_str = strstr(__P_ms_name, ".")) != 0)
    {
      separator = strlen(__P_ms_name) - strlen(temp_str);
      odbc_column.__ms_table_name.assign(__P_ms_name,separator);
      odbc_column.__ms_name = temp_str + 1;
    }
    else
    { 
      odbc_column.__ms_name = __P_ms_name;
    }

    if(DriverManager::isTracingOn)
    { 
      odbc_column.dump(__sb);
    }
  }

  if(DriverManager::isTracingOn && (numcols > 0))
  {
    __XDBC_FORCED_TRACE(__ODBCCONVERSION_CLASS_NAME,__sb.str());    
  } 
}

void OdbcKit::prepareVariableData(__XDBC_HOOK_DECLARE_MORE OdbcConnection* odbc_conn,SQLHSTMT odbc_stmthp,vector<ODBC_PARAM*>& __vector_params) __XDBC_THROW_DECLARE(SQLException)
{
  SQLSMALLINT numvars = 0;
  SQLRETURN __odbc_sqlreturn = SQL_SUCCESS;
  
  /* Get the number of parameters in the query */
  __odbc_sqlreturn = (odbc_conn->__odbc_apis->__api_SQLNUMPARAMS)(odbc_stmthp, &numvars);
  if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
  {
    OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE odbc_conn,odbc_stmthp,__odbc_sqlreturn,"prepareVariableData");
    __XDBC_HOOK_CHECK(return);
  }
  
  __vector_params.resize(numvars + 1);
  
  SQLSMALLINT   vartype = 0;
  SQLULEN       varlen = 0;
  SQLSMALLINT   scale = 0;
  SQLSMALLINT   nullable = 0;
  
  StringBuffer __sb;
  if(DriverManager::isTracingOn)
  {
    __sb << "prepareVariableData(" <<  numvars << ")|" << File::lineSeparator;
  }
  
  __vector_params[0] = 0;

  SQLSMALLINT    __param_type = 0;
  SQLUINTEGER     __param_len = 0;
  SQLSMALLINT    __param_scale = 0;
  SQLSMALLINT    __param_nullable = 0;
 
  OdbcTypes* __odbcTypes = &(OdbcTypesImpl::getInstance());
  struct ODBC_DRIVER_OPTIONS* __odbc_options = odbc_conn->__odbc_options;
 
  for(int i = 1;i <= numvars; ++i)
  {

    ODBC_PARAM* odbc_param = new ODBC_PARAM();
    __vector_params[i] = odbc_param;
    odbc_param->__mi_index = i;

    __odbc_sqlreturn = __odbc_SQLDescribeParam(odbc_conn,odbc_stmthp, 
                                               (SQLSMALLINT) i,
                                               &(__param_type), 
                                               &(__param_len),
                                               &(__param_scale), 
                                               &(__param_nullable));
                                                                            
    if(__ODBC_HOOK_CHECK_FAILED(__odbc_sqlreturn))
    {
      OdbcKit::checkException(__XDBC_HOOK_INVOKE_MORE odbc_conn,odbc_stmthp,__odbc_sqlreturn,"prepareVariableData");
      __XDBC_HOOK_CHECK(return);
    }
    
    odbc_param->__mi_type = __param_type;
    odbc_param->__mi_length = __param_len;
    odbc_param->__mi_scale = __param_scale;
    odbc_param->__mi_nullable = __param_nullable;
    
    odbc_param->__ms_type_name = __odbcTypes->getTypeName(__odbc_options,odbc_param->__mi_type);
    odbc_param->__mi_precision = odbc_param->__mi_length;

    if(DriverManager::isTracingOn)
    { 
      odbc_param->dump(__sb);
    }
  }
  
  if(DriverManager::isTracingOn && (numvars > 0))
  {
    __XDBC_FORCED_TRACE(__ODBCCONVERSION_CLASS_NAME,__sb.str());    
  } 
}

void OdbcKit::adjustDefinable(OdbcConnection* odbc_conn,ODBC_COLUMN* __p_column,ODBC_DEFINE* __p_define)
{
  OdbcKit::fixColumnSize(odbc_conn,__p_column);

  switch(__p_column->__mi_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      __p_define->__mi_define_c_type = SQL_C_CHAR;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      __p_define->__mi_define_c_type = SQL_C_BINARY;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break;      
//-----------------------------
    case SQL_BIT:
    {
      __p_define->__mi_define_c_type = SQL_C_BIT;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_TINYINT:
    {
      __p_define->__mi_define_c_type = SQL_C_TINYINT;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_SMALLINT:
    {
      __p_define->__mi_define_c_type = SQL_C_SHORT;
      __p_define->__mi_define_type   = __p_column->__mi_type;
    }
    break; 
//-----------------------------
    case SQL_INTEGER:
    {
      __p_define->__mi_define_c_type = SQL_C_LONG;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_BIGINT:
    {
      __p_define->__mi_define_c_type = SQL_C_BIGINT;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_REAL:
    {
      __p_define->__mi_define_c_type = SQL_C_FLOAT;
      __p_define->__mi_define_type   = __p_column->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      if (__p_column->__mi_scale == 0 && __p_column->__mi_precision > 0 && __p_column->__mi_precision <= 18)
      {
        bool __mb_tbintAvailable = odbc_conn->__odbc_options->__mb_tbintAvailable;
        if (__p_column->__mi_precision <= 2)
        {
          __p_define->__mi_define_type   = SQL_TINYINT;
          __p_define->__mi_define_c_type = SQL_C_TINYINT;
        }
        else if (__p_column->__mi_precision <= 4)
        {
          __p_define->__mi_define_type   = SQL_SMALLINT;
          __p_define->__mi_define_c_type = SQL_C_SHORT;
        }
        else if (__p_column->__mi_precision <= 9)
        {
          __p_define->__mi_define_type   = SQL_INTEGER;
          __p_define->__mi_define_c_type = SQL_C_LONG;
        }       
        else if(__mb_tbintAvailable)
        {
          __p_define->__mi_define_type   = SQL_BIGINT;
          __p_define->__mi_define_c_type = SQL_C_BIGINT;
        }
        else
        {
           __p_define->__mi_define_type   = SQL_DOUBLE;
          __p_define->__mi_define_c_type = SQL_C_DOUBLE;  
        }      
      }
      else
      {
        __p_define->__mi_define_type   = SQL_DOUBLE;
        __p_define->__mi_define_c_type = SQL_C_DOUBLE;  
      }
    }
    break; 
//-----------------------------
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      bool __mb_tbintAvailable = odbc_conn->__odbc_options->__mb_tbintAvailable;
        
      if (__p_column->__mi_scale == 0 && __p_column->__mi_precision > 0 && __p_column->__mi_precision <= 18)
      { 
        if (__p_column->__mi_precision <= 2)
        {
          __p_define->__mi_define_type   = SQL_TINYINT;
          __p_define->__mi_define_c_type = SQL_C_TINYINT;
        }
        else if (__p_column->__mi_precision <= 4)
        {
          __p_define->__mi_define_type   = SQL_SMALLINT;
          __p_define->__mi_define_c_type = SQL_C_SHORT;
        }
        else if (__p_column->__mi_precision <= 8)
        {
          __p_define->__mi_define_type   = SQL_INTEGER;
          __p_define->__mi_define_c_type = SQL_C_LONG;
        }
        else if(__mb_tbintAvailable)
        {
          __p_define->__mi_define_type   = SQL_BIGINT;
          __p_define->__mi_define_c_type = SQL_C_BIGINT;
        }
        else
        {
           __p_define->__mi_define_type   = SQL_DOUBLE;
          __p_define->__mi_define_c_type = SQL_C_DOUBLE;  
        }      
      }
      else
      {
        __p_define->__mi_define_type   = SQL_DOUBLE;
        __p_define->__mi_define_c_type = SQL_C_DOUBLE;  
      }      
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_DATE:
    case SQL_DATE:  
    {
      __p_define->__mi_define_type   = SQL_TYPE_DATE;
      __p_define->__mi_define_c_type = SQL_C_TYPE_DATE;
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_TIME: 
    case SQL_TIME:   
    {
      __p_define->__mi_define_type   = SQL_TYPE_TIME;
      __p_define->__mi_define_c_type = SQL_C_TYPE_TIME;
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_TIMESTAMP: 
    case SQL_TIMESTAMP: 
    {
      __p_define->__mi_define_type   = SQL_TYPE_TIMESTAMP;
      __p_define->__mi_define_c_type = SQL_C_TYPE_TIMESTAMP;
      
    }
    break;                                                 
//-----------------------------
    default:
    {
      __p_define->__mi_define_c_type = SQL_C_DEFAULT; 
      __p_define->__mi_define_type   = __p_column->__mi_type;  
    }
    break;
  } 
}

void OdbcKit::adjustBindable(OdbcConnection* odbc_conn,ODBC_PARAM* __p_param)
{
  switch(__p_param->__mi_type)
  {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    {
      bool __mb_fntsAvailable = odbc_conn->__odbc_options->__mb_fntsAvailable;
      
      __p_param->__mi_bind_c_type = SQL_C_CHAR;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
      if(__mb_fntsAvailable)
      {
        __p_param->__mi_bind_ind_int = SQL_NTS;
        __p_param->__mi_bind_ind_len = SQL_NTS;
      }
    }
    break;
//-----------------------------
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
    {
      __p_param->__mi_bind_c_type = SQL_C_BINARY;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break;      
//-----------------------------
    case SQL_BIT:
    case SQL_TINYINT:  
    {
      __p_param->__mi_bind_c_type = SQL_C_TINYINT;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break;  
//-----------------------------
    case SQL_SMALLINT:
    {
      __p_param->__mi_bind_c_type = SQL_C_SHORT;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_INTEGER:
    {
      __p_param->__mi_bind_c_type = SQL_C_LONG;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_BIGINT:
    {
      __p_param->__mi_bind_c_type = SQL_C_BIGINT;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_REAL:
    {
      __p_param->__mi_bind_c_type = SQL_C_FLOAT;
      __p_param->__mi_bind_type   = __p_param->__mi_type;
      
    }
    break; 
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      if (__p_param->__mi_scale == 0 && __p_param->__mi_precision > 0 && __p_param->__mi_precision <= 18)
      {
        bool __mb_tbintAvailable = odbc_conn->__odbc_options->__mb_tbintAvailable;
        if (__p_param->__mi_precision <= 2)
        {
          __p_param->__mi_bind_type   = SQL_TINYINT;
          __p_param->__mi_bind_c_type = SQL_C_TINYINT;
        }
        else if (__p_param->__mi_precision <= 4)
        {
          __p_param->__mi_bind_type   = SQL_SMALLINT;
          __p_param->__mi_bind_c_type = SQL_C_SHORT;
        }
        else if (__p_param->__mi_precision <= 9)
        {
          __p_param->__mi_bind_type   = SQL_INTEGER;
          __p_param->__mi_bind_c_type = SQL_C_LONG;
        }
        else if(__mb_tbintAvailable)
        {
          __p_param->__mi_bind_type   = SQL_BIGINT;
          __p_param->__mi_bind_c_type = SQL_C_BIGINT;
        }
        else
        {
          __p_param->__mi_bind_type   = SQL_DOUBLE;
          __p_param->__mi_bind_c_type = SQL_C_DOUBLE;  
        }      
      }
      else
      {
        __p_param->__mi_bind_type   = SQL_DOUBLE;
        __p_param->__mi_bind_c_type = SQL_C_DOUBLE;  
      }
    }
    break; 
//-----------------------------
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      if (__p_param->__mi_scale == 0 && __p_param->__mi_precision > 0 && __p_param->__mi_precision <= 18)
      {
        bool __mb_tbintAvailable = odbc_conn->__odbc_options->__mb_tbintAvailable;        
        if (__p_param->__mi_precision <= 2)
        {
          __p_param->__mi_bind_type   = SQL_TINYINT;
          __p_param->__mi_bind_c_type = SQL_C_TINYINT;
        }
        else if (__p_param->__mi_precision <= 4)
        {
          __p_param->__mi_bind_type   = SQL_SMALLINT;
          __p_param->__mi_bind_c_type = SQL_C_SHORT;
        }
        else if (__p_param->__mi_precision <= 8)
        {
          __p_param->__mi_bind_type   = SQL_INTEGER;
          __p_param->__mi_bind_c_type = SQL_C_LONG;
        }
        else if(__mb_tbintAvailable)
        {
          __p_param->__mi_bind_type   = SQL_BIGINT;
          __p_param->__mi_bind_c_type = SQL_C_BIGINT;
        }
        else
        {
          __p_param->__mi_bind_type   = SQL_DOUBLE;
          __p_param->__mi_bind_c_type = SQL_C_DOUBLE;  
        }      
      }
      else
      {
        __p_param->__mi_bind_type   = SQL_DOUBLE;
        __p_param->__mi_bind_c_type = SQL_C_DOUBLE;  
      }
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_DATE:
    case SQL_DATE:  
    {
      __p_param->__mi_bind_type   = SQL_TYPE_DATE;
      __p_param->__mi_bind_c_type = SQL_C_TYPE_DATE;
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_TIME: 
    case SQL_TIME:   
    {
      __p_param->__mi_bind_type   = SQL_TYPE_TIME;
      __p_param->__mi_bind_c_type = SQL_C_TYPE_TIME;
      
    }
    break; 
//-----------------------------
    case SQL_TYPE_TIMESTAMP: 
    case SQL_TIMESTAMP: 
    {
      __p_param->__mi_bind_type   = SQL_TYPE_TIMESTAMP;
      __p_param->__mi_bind_c_type = SQL_C_TYPE_TIMESTAMP;
      
    }
    break;                                                 
//-----------------------------
    default:
    {
    }
    break;
  }  
}

void OdbcKit::fixColumnSize(OdbcConnection* odbc_conn,ODBC_COLUMN* odbc_column)
{
  if(odbc_column->__mi_length != 0)
  {
    return ;
  }
  
  switch(odbc_column->__mi_type)
  {
    case SQL_BIT:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLCHAR);
    }
    break;
//-----------------------------
    case SQL_TINYINT:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLCHAR);
    }
    break;
//-----------------------------
    case SQL_SMALLINT:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLSMALLINT);
    }
    break;
//-----------------------------
    case SQL_INTEGER:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLINTEGER);
    }
    break;
//-----------------------------
    case SQL_BIGINT:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLBIGINT);
    }
    break;
//-----------------------------
    case SQL_REAL:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLREAL);
    }
    break;
//-----------------------------
    case SQL_FLOAT:
    case SQL_DOUBLE:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQLDOUBLE);
    }
    break;
//-----------------------------
    case SQL_NUMERIC:
    case SQL_DECIMAL:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQL_MAX_NUMERIC_LEN);
    }
    break;
//-----------------------------
    case SQL_TYPE_DATE:
    case SQL_DATE:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQL_DATE_STRUCT);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIME:
    case SQL_TIME:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQL_TIME_STRUCT);
    }
    break;
//-----------------------------
    case SQL_TYPE_TIMESTAMP:
    case SQL_TIMESTAMP:
    {
      odbc_column->__mi_length = (SQLINTEGER)sizeof(SQL_TIMESTAMP_STRUCT);
    }
    break;
//-----------------------------
    default:
    {
    }
    break;
  }
}  

__OPENCODE_END_NAMESPACE
