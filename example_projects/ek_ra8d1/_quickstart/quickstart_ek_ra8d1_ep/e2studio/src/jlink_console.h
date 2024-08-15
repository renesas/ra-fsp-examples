/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : jlink_console_main.h
 * Description  : Entry function.
 *********************************************************************************************************************/

#ifndef JLINK_CONSOLE_ENTRY_H_
#define JLINK_CONSOLE_ENTRY_H_

/*******************************************************************************************************************//**
 * @ingroup hal_entry
 * @{
 **********************************************************************************************************************/
extern void jlink_console_init(void);

extern void      start_key_check(void);
extern uint8_t   get_detected_key(void);
extern bool_t    key_pressed(void);
extern uint8_t * input_from_console_fifo(void);

// extern void input_from_console_fifo (uint8_t* pFifo);
uint8_t get_new_chars(uint8_t * pBuf);

/** @} */
#endif                                 /* JLINK_CONSOLE_ENTRY_H_ */
