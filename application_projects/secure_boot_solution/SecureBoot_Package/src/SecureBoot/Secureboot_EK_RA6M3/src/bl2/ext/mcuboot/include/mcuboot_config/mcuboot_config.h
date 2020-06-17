/*
 * Copyright (c) 2018 Open Source Foundries Limited
 * Copyright (c) 2019 Arm Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 */

#ifndef __MCUBOOT_CONFIG_H__
#define __MCUBOOT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This file is also included by the simulator, but we don't want to
 * define anything here in simulator builds.
 *
 * Instead of using mcuboot_config.h, the simulator adds MCUBOOT_xxx
 * configuration flags to the compiler command lines based on the
 * values of environment variables. However, the file still must
 * exist, or bootutil won't build.
 */
#ifndef __BOOTSIM__

/*
 * In TF-M most of the configuration flags (e.g. signature type,
 * upgrade mode ...) are handled by the CMake-based buildsystem and
 * added to the compiler command lines.
 */

/*
 * Cryptographic settings
 */
#define MCUBOOT_USE_MBED_TLS

/*
 * Logging
 */
#define MCUBOOT_HAVE_LOGGING    1

#endif /* !__BOOTSIM__ */

/*
 * Watchdog feeding
 */
#define MCUBOOT_WATCHDOG_FEED()     \
    do {                            \
        /* Do nothing. */           \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __MCUBOOT_CONFIG_H__ */
