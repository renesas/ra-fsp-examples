
#include "bsp_api.h"

int _write(int file, char *ptr, int len);

int _write(int file, char *ptr, int len)
{
    FSP_PARAMETER_NOT_USED(file);
    int ret = 0;
    int32_t todo;

    /* If the debugger is connected */
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
    {
        /* Send the character */
          for (todo = 0; todo < len; todo++)
          {
        	  if ('\r' != *ptr)
        		  ITM_SendChar((uint32_t)*ptr);/* CMSIS call */
              ptr++;
          }
          ret = len;
    }

    return ret;
}


