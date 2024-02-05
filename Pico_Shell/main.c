#include "PicoShell/picoshell.h"

int main(void)
  {
    ReturnStatus status = STATUS_TRUE ;
    while (STATUS_FALSE != status )
      {
        status = GetShellMessage() ;
	
      }
  return 0 ;
}
