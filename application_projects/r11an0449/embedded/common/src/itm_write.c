/***********************************************************************************************************************
 * File Name    : itm_write.c
 * Description  : implementation of ITM write routine.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
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

#if 0

int internal_write(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

/*****************************************************************************************************************
 *  @brief      internal_write: write to ITM
 *  @param[in]  file: not used
 *  @param[in]  ptr: pointer to the data to write
 *  @param[in]  len: length of the data to write
 *  @retval     number of bytes written
 *  ****************************************************************************************************************/
int internal_write(int file, char *ptr, int len)
{
    FSP_PARAMETER_NOT_USED(file);
	int ret = 0;
	int32_t todo = 0;

    /* If the debugger is connected */
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
    {
        /* Send the character */
    	  for (todo = 0; todo < len; todo++)
    	  {
    		  ITM_SendChar((uint32_t)*ptr);/* CMSIS call */
    		  ptr++;
    	  }
    	  ret = len;
	}
    return ret;
}

/*****************************************************************************************************************
 *  @brief      _write: ITM data write
 *  @param[in]  file: not used
 *  @param[in]  ptr: pointer to the data to write
 *  @param[in]  len: length of the data to write
 *  @retval     number of bytes written
 *  ****************************************************************************************************************/
int _write(int file, char *ptr, int len)
{
  FSP_PARAMETER_NOT_USED(file);
  int ret = 0;

  ret = internal_write(file, ptr, len);

  return ret;
}

#endif
