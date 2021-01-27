/***********************************************************************************************************************
 * File Name    : itm_write.c
 * Description  : implementation of ITM write routine.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
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
