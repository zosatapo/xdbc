#include "xdbc_demo.h"
#include "xdbc/XDBCUtilities.h" 

#include "port/ioc_smart.h"
#include "port/libsys.h"
#include "port/utilities.h"
#include "port/profile.h"
#include "port/properties.h"
#include "port/file.h"

#include <iostream>

USING_OPENCODE_STL_NS
USING_OPENCODE_NS

Connection* getConnection(__XDBC_HOOK_DECLARE_MORE const string& dbname) __OPENCODE_THROW_DECLARE(SQLException)
{
  string __dbname = dbname;
  if(__dbname.empty())
  {
    __dbname = System::getProperty("xdbc.dbname");   
  }

  if(__dbname.empty())
  {
    __XDBC_HOOK_RETURN(return 0,"no dbname provided");  
  }

  string __database ;
  string __dbuser ;
  string __password ;
           
  Connection* __p_conn = DriverManager::getConnection(__XDBC_HOOK_INVOKE_MORE __dbname,__database,__dbuser,__password);
  xdbc_smart_ptr<Connection> __sp_conn(__p_conn);
  __XDBC_HOOK_CHECK(return 0);
  
  __p_conn->connect(__XDBC_HOOK_INVOKE_MORE __database,__dbuser,__password);  
  __XDBC_HOOK_CHECK(return 0);
    
  return __sp_conn.release();
}

void  dumpMetaData(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet)  __OPENCODE_THROW_DECLARE(SQLException)
{
  smart_ptr<ResultSetMetaData> __sp_meta(resultSet->getMetaData(__XDBC_HOOK_INVOKE_ONCE));
  
  for(int i = 1;i <= __sp_meta->getColumnCount(__XDBC_HOOK_INVOKE_ONCE); ++i)
  {
    cout<<"INDEX : " << i << ", TYPE : "<< __sp_meta->getColumnType(__XDBC_HOOK_INVOKE_MORE i) << endl;
  }
}

void  dumpResultSet(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet)  __OPENCODE_THROW_DECLARE(SQLException)
{
  smart_ptr<ResultSetMetaData> __sp_meta(resultSet->getMetaData(__XDBC_HOOK_INVOKE_ONCE));
  while(resultSet->next(__XDBC_HOOK_INVOKE_ONCE))
  {
    dumpRow(__XDBC_HOOK_INVOKE_MORE resultSet,__sp_meta.get());
  }    
}
  
void  dumpRow(__XDBC_HOOK_DECLARE_MORE ResultSet* resultSet,ResultSetMetaData* __sp_meta)  __OPENCODE_THROW_DECLARE(SQLException)
{
  cout<<"-------Fetch()----[" << resultSet->getRow(__XDBC_HOOK_INVOKE_ONCE) << "]-----"<<endl;
  
  for(int i = 1;i <= __sp_meta->getColumnCount(__XDBC_HOOK_INVOKE_ONCE); ++i)
  {
    int __col_type = __sp_meta->getColumnType(__XDBC_HOOK_INVOKE_MORE i);
    char __column_name[1024] ={0};
    sprintf(__column_name,"%10s",__sp_meta->getColumnName(__XDBC_HOOK_INVOKE_MORE i).c_str());  

    switch(__col_type)
    {
      case XDBC_TYPE_CHAR:
      case XDBC_TYPE_VARCHAR:
      case XDBC_TYPE_LONGVARCHAR:
      {
        int __sizeBytes = 0;
        const char* __p_char = resultSet->getString(__XDBC_HOOK_INVOKE_MORE i,__sizeBytes);
        if(__sizeBytes <= 0)
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : <" << __sizeBytes << "> " << __p_char << endl;
          cout << __column_name << "  : <" << __sizeBytes << "> " << MemoryKit::numDump(__p_char,__sizeBytes) << endl;
        }  
      }
      break;  
      case XDBC_TYPE_BINARY:
      case XDBC_TYPE_VARBINARY:
      case XDBC_TYPE_LONGVARBINARY:
      {
        int __sizeBytes = 0;
        const char* __p_char = resultSet->getString(__XDBC_HOOK_INVOKE_MORE i,__sizeBytes);
        if(__sizeBytes <= 0)
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : <" << __sizeBytes << "> " << MemoryKit::numDump(__p_char,__sizeBytes) << endl;
        }  
      }
      break; 
      case XDBC_TYPE_BIT:
      case XDBC_TYPE_TINYINT:
      case XDBC_TYPE_BOOLEAN:
      {
        char __colval = 0;

        if(!resultSet->getByte(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }        
      }
      break;
      case XDBC_TYPE_SMALLINT:
      {
        short __colval = 0;

        if(!resultSet->getShort(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }
      }
      break; 
      case XDBC_TYPE_INTEGER:
      {
        int __colval = 0;

        if(!resultSet->getInt(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }        
      }
      break; 
      case XDBC_TYPE_BIGINT:
      {
        XLONG __colval = 0;

        if(!resultSet->getLong(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }        
      }
      break; 
      case XDBC_TYPE_FLOAT:
      case XDBC_TYPE_REAL:
      {
        float __colval = 0;

        if(!resultSet->getFloat(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }        
      }
      break; 
      case XDBC_TYPE_DOUBLE:
      case XDBC_TYPE_NUMERIC:
      case XDBC_TYPE_DECIMAL:
      {
        double __colval = 0;

        if(!resultSet->getDouble(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {        
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << __colval << endl;
        }
      }
      break;             
      case XDBC_TYPE_DATE:
      {
        XDBC_DATE __colval;
        XDBC_DATE::init(&__colval);
          
        if(!resultSet->getDate(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {        
          cout << __column_name << "  : " << XDBCUtilities::toString(__colval) << endl;
        } 
      }
      break;  
      case XDBC_TYPE_TIME:
      {
        XDBC_TIME __colval;
        XDBC_TIME::init(&__colval);
          
        if(!resultSet->getTime(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << XDBCUtilities::toString(__colval) << endl;
        }
      }
      break;  
      case XDBC_TYPE_TIMESTAMP:
      {
        XDBC_TIMESTAMP __colval;
        XDBC_TIMESTAMP::init(&__colval);
          
        if(!resultSet->getTimestamp(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << XDBCUtilities::toString(__colval) << endl;
        }
      }
      break;
      case XDBC_TYPE_INTERVAL:
      {
        XDBC_INTERVAL __colval;
        XDBC_INTERVAL::init(&__colval);
          
        if(!resultSet->getInterval(__XDBC_HOOK_INVOKE_MORE i,__colval))
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : " << XDBCUtilities::toString(__colval) << endl;
        }
      }
      break;
      case XDBC_TYPE_BLOB:
      {
        Blob* __p_blob = resultSet->getBlob(__XDBC_HOOK_INVOKE_MORE i);
        smart_ptr<Blob> __sp_blob(__p_blob);
        
        if(__p_blob == 0)
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : <" << __p_blob->length(__XDBC_HOOK_INVOKE_ONCE) << ">" << endl;
          
          char lob_data[100] = {0};
          int size = __p_blob->read(__XDBC_HOOK_INVOKE_MORE  1,lob_data,0,100);
          
          cout << "|" << MemoryKit::numDump(lob_data,size) << "|" << endl;
          cout << "|" << lob_data << "|" << endl;
        }    
      }
      break;
      case XDBC_TYPE_CLOB:
      {
        Clob* __p_clob = resultSet->getClob(__XDBC_HOOK_INVOKE_MORE i);
        smart_ptr<Clob> __sp_clob(__p_clob);
        
        if(__p_clob == 0)
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : <" << __p_clob->length(__XDBC_HOOK_INVOKE_ONCE) << ">" << endl;
          
          char lob_data[100] = {0};
          int size = __p_clob->read(__XDBC_HOOK_INVOKE_MORE 1,lob_data,0,100);
          
          cout << "|" << MemoryKit::numDump(lob_data,size) << "|" << endl;
          cout << "|" << lob_data << "|" << endl;
        } 
      }
      break;
      default:
      {
        int __sizeBytes = 0;
        const char* __p_char = resultSet->getString(__XDBC_HOOK_INVOKE_MORE i,__sizeBytes);
        if(__sizeBytes == 0)
        {
          cout << __column_name << "  : <NULL>" << endl;
        }
        else
        {
          cout << __column_name << "  : <" << __sizeBytes << "> " << MemoryKit::numDump(__p_char,__sizeBytes) << endl;
        }         
      }
      break;      
    } //~end  switch(__col_type)
    
  }//~end for(int i = 1;i <= __sp_meta->getColumnCount(); ++i)
     
}

