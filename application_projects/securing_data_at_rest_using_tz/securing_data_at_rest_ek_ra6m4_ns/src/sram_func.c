/*
 * sram_func.c
 *
 *  Created on: Sep 5, 2020
 *      Author: xianghui.wang
 */
#include "bsp_api.h"
#include <string.h>
#include "sram_func.h"
#include "app_definitions.h"


int *var_in_secure_2 = (int*)SECURE_SRAM_WRITE_ADDR;
int var_in_secure2 = SRAM_WRITE_DATA;



void write_to_secure_sram(void)
{
	/* write to secure SRAM location */
	/* below function call will trigger secure fault due to invalid exception return */
	*var_in_secure_2 = SRAM_WRITE_DATA;
}

void read_secure_sram(void)
{
	void (*FuncPointer) (void) = (void (*) (void)) SECURE_SRAM_READ_ADDR;
	/* below function call will trigger secure fault due to invalid entry point */
	FuncPointer ();
}




