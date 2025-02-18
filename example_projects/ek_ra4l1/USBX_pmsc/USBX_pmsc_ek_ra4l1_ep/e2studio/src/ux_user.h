/***********************************************************************************************************************
 * File Name    : ux_user.h
 * Description  : This file contains user defines for configuring USBX in specific ways. This file will have an effect
 *                only if the application and USBX library are built with UX_INCLUDE_USER_DEFINE_FILE defined.
 **********************************************************************************************************************/

#ifndef UX_USER_H_
#define UX_USER_H_

/* If the macro UX_SLAVE_REQUEST_DATA_MAX_LENGTH is not explicitly defined, the default value is set to 2048 bytes.
 * For our applications, it requires larger data buffers to interface with QSPI memory (default page size of 4096
 * bytes). It is recommended to set the value of this macro to 4096 to ensure compatibility */
#define UX_SLAVE_REQUEST_DATA_MAX_LENGTH (4096)

#endif /* UX_USER_H_ */
