#include "xdbc/oradefs.h"
#include "xdbc/oracleConnection.h"
#include "port/ioc_G_config.h"

USING_OPENCODE_NS

/* library's utility function */
extern "C" __OPENCODE_EXPORT_DECL Connection* _xdbc_getConnection();
 
extern "C" void _xdbc_init();

//extern "C" void _init(void);

extern "C" void __wrap__init(void);

Connection* _xdbc_getConnection()
{
  return new OracleConnection();
}

void _xdbc_init()
{
}


void _init(void)
{
  _xdbc_init();
}

void __wrap__init(void)
{
  _init();
}


