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

#ifndef __TFM_PLAT_NV_COUNTERS_H__
#define __TFM_PLAT_NV_COUNTERS_H__

#include <stdint.h>
#include "tfm_plat_defs.h"

enum tfm_nv_counter_t {
    PLAT_NV_COUNTER_0 = 0,  /* Used by SST service */
    PLAT_NV_COUNTER_1,      /* Used by SST service */
    PLAT_NV_COUNTER_2,      /* Used by SST service */
    PLAT_NV_COUNTER_3,      /* Used by bootloader */
    PLAT_NV_COUNTER_MAX
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialises all non-volatile (NV) counters.
 *
 * \return  TFM_PLAT_ERR_SUCCESS if the initialization succeeds, otherwise
 *          TFM_PLAT_ERR_SYSTEM_ERR
 */
enum tfm_plat_err_t tfm_plat_init_nv_counter(void);

/**
 * \brief Reads the given non-volatile (NV) counter.
 *
 * \param[in]  counter_id  NV counter ID.
 * \param[in]  size        Size of the buffer to store NV counter value
 *                         in bytes.
 * \param[out] val         Pointer to store the current NV counter value.
 *
 * \return  TFM_PLAT_ERR_SUCCESS if the value is read correctly. Otherwise,
 *          it returns TFM_PLAT_ERR_SYSTEM_ERR.
 */
enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val);

/**
 * \brief Increments the given non-volatile (NV) counter.
 *
 * \param[in] counter_id  NV counter ID.
 *
 * \return  When the NV counter reaches its maximum value, the
 *          TFM_PLAT_ERR_MAX_VALUE error is returned to indicate the value
 *          cannot be incremented. Otherwise, it returns TFM_PLAT_ERR_SUCCESS.
 */
enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                              enum tfm_nv_counter_t counter_id);

/**
 * \brief Sets the given non-volatile (NV) counter to the specified value.
 *
 * \param[in] counter_id  NV counter ID.
 * \param[in] value       New value of the NV counter. The maximum value that
 *                        can be set depends on the constraints of the
 *                        underlying implementation, but it always must be
 *                        greater than or equal to the current NV counter value.
 *
 * \retval TFM_PLAT_ERR_SUCCESS         The NV counter is set successfully
 * \retval TFM_PLAT_ERR_INVALID_INPUT   The new value is less than the current
 *                                      counter value
 * \retval TFM_PLAT_ERR_MAX_VALUE       The new value is greater than the
 *                                      maximum value of the NV counter
 * \retval TFM_PLAT_ERR_UNSUPPORTED     The function is not implemented for
 *                                      the given platform or the new value is
 *                                      not representable on the underlying
 *                                      counter implementation
 * \retval TFM_PLAT_ERR_SYSTEM_ERR      An unspecified error occurred
 *                                      (none of the other standard error codes
 *                                      are applicable)
 */
enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                            uint32_t value);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_NV_COUNTERS_H__ */

