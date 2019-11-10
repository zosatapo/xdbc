#include "xdbc/pooledPreparedStatement.h"
#include "xdbc/XDBCUtilities.h"

__OPENCODE_BEGIN_NAMESPACE

PooledPreparedStatement::~PooledPreparedStatement()
{
  
}

PooledPreparedStatement::PooledPreparedStatement(PreparedStatement* pPreparedStatement)
:__p_pstmt(pPreparedStatement),__pooled_ref_count(0)
{
}

ResultSet* PooledPreparedStatement::executeQuery(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_pstmt->executeQuery(__XDBC_HOOK_INVOKE_ONCE);
}

int PooledPreparedStatement::executeUpdate(__XDBC_HOOK_DECLARE_ONCE) __XDBC_THROW_DECLARE(SQLException)
{
  return __p_pstmt->executeUpdate(__XDBC_HOOK_INVOKE_ONCE);
}

void PooledPreparedStatement::setByte(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const char& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setByte(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setShort(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const short& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setShort(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setInt(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const int& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setInt(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setLong(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const XLONG& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setLong(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setFloat(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const float& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setFloat(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setDouble(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const double& x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setDouble(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex, const string* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setString(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setString(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setString(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,count,is_null);
}

void PooledPreparedStatement::setDate(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const XDBC_DATE* x,bool is_null)__XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setDate(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setTime(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const XDBC_TIME* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setTime(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setTimestamp(__XDBC_HOOK_DECLARE_MORE  int paramIndex,const XDBC_TIMESTAMP* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setTimestamp(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setInterval(__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const XDBC_INTERVAL* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setInterval(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,const Blob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setBlob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setBlob(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,count,is_null);
}

void PooledPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex, const Clob* x,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,is_null);
}

void PooledPreparedStatement::setClob (__XDBC_HOOK_DECLARE_MORE  int paramIndex,  const char* x,int count,bool is_null) __XDBC_THROW_DECLARE(SQLException)
{
  __p_pstmt->setClob(__XDBC_HOOK_INVOKE_MORE  paramIndex,x,count,is_null);
}


//-------------------------------------------------------------
// Statement Interfaces
//-------------------------------------------------------------
void PooledPreparedStatement::close()
{ 
  //数据库对象缓存机制关键技术
}

void PooledPreparedStatement::destroy()
{ 
  //数据库对象缓存机制关键技术
}

int PooledPreparedStatement::getResultSetType(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __p_pstmt->getResultSetType(__XDBC_HOOK_INVOKE_ONCE);
}

Connection* PooledPreparedStatement::getConnection(__XDBC_HOOK_DECLARE_ONCE)  __XDBC_THROW_DECLARE(SQLException)
{
  return __p_pstmt->getConnection(__XDBC_HOOK_INVOKE_ONCE);
}


void PooledPreparedStatement::pooledClose()
{
  __XDBC_INTERFACE_CLOSE(__p_pstmt); 
}
  
void PooledPreparedStatement::pooledDestroy()
{    
  __XDBC_INTERFACE_DESTROY(__p_pstmt); 
  delete this;
}    
  
__OPENCODE_END_NAMESPACE
