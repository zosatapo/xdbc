#include "xdbc/ifxdefs.h"
#include "xdbc/ifxConnection.h"

USING_OPENCODE_NS

/* library's utility function */
 __OPENCODE_EXPORT_DECL extern "C" Connection* _xdbc_getConnection();

extern "C" void _xdbc_init();

//extern "C" void _init(void);

extern "C" void __wrap__init(void);

Connection* _xdbc_getConnection()
{
  return new IfxConnection();
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


