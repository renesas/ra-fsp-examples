#ifndef R_UARTA_CFG_H_
#define R_UARTA_CFG_H_

#if (USE_VIRTUAL_COM == 1)
#ifdef __cplusplus
            extern "C" {
            #endif

#define UARTA_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define UARTA_CFG_DTC_SUPPORT_ENABLE (0)
#define UARTA_CFG_RECEIVE_ERROR_INTERRUPT_MODE 1
#ifdef __cplusplus
            }
            #endif

#endif /* USE_VIRTUAL_COM */
#endif /* R_UARTA_CFG_H_ */
