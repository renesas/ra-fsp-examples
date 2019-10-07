

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �eAS IS�f WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/


/*
 * moduleTable.h
 *
 *  Created on: 3 Mar 2016
 *      Author: paul
 */

#ifndef SRC_MODULE_INFO_H_
#define SRC_MODULE_INFO_H_

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif


/* **************************************************************************************
 * Module Ids
 *
 * The moduleId is
 *    - an index into the module table.
 *    - referenced in an update to identify which module is being updated and which module is authorising the update.
 *
 * A few module id values are fixed (and well known).
 * They are the id's for the kernel and delegation.
 * The majority of ids are free for the app developer to use as they wish.
 *
 * id range | purpose                       | Module table
 * 0..127   | Application                   | Main module table.
 * 128..191 | OEM recovery + Delegation.    | Mapped to the OEM Recovery module table
 * 192..255 | kernel                        | Single entry tables in kernel and 1st rx.
 *
 * In practise the number of ids per table is less due to the limited memory for the MTs.
 * **************************************************************************************/

typedef uint8_t   ModuleId;

#define MODULE_TABLE_SHIFT   (6)
#define MODULE_TABLE_ID_MASK (0xFF << MODULE_TABLE_SHIFT)
#define MODULE_ID_MASK       (0xFF >> (8 - MODULE_TABLE_SHIFT))

typedef enum
{
    MODULE_TABLE_ID_MAIN            = 0 << MODULE_TABLE_SHIFT,
    MODULE_TABLE_ID_MAIN2           = 1 << MODULE_TABLE_SHIFT,
    MODULE_TABLE_ID_OEM_RX          = 2 << MODULE_TABLE_SHIFT,
    MODULE_TABLE_ID_KERNEL          = 3 << MODULE_TABLE_SHIFT
} ModuleTableIds;

typedef enum
{
    MODULE_ID_REA_MASTER_KEY        = 0 | MODULE_TABLE_ID_KERNEL,
    MODULE_ID_FAB_KEY               = 1 | MODULE_TABLE_ID_KERNEL,
    MODULE_ID_STZ_JC_SIGNING_KEY    = 2 | MODULE_TABLE_ID_KERNEL,
    MODULE_ID_OEM_JC_SIGNING_KEY    = 3 | MODULE_TABLE_ID_KERNEL,

    MODULE_ID_SECURITY_KERNEL       = 10 | MODULE_TABLE_ID_KERNEL,
    MODULE_ID_1ST_RX                = 11 | MODULE_TABLE_ID_KERNEL,

    MODULE_ID_OEM_RECOVERY_RX       = 0 | MODULE_TABLE_ID_OEM_RX,
    MODULE_ID_OEM_DELIGATION_KEY    = 1 | MODULE_TABLE_ID_OEM_RX,

    MODULE_ID_INVALID_ID            = 255,
} ModuleIdsReserved;

typedef enum
{
    MODULE_SECURITY_KERNEL,
    MODULE_1ST_RECEIVER,
    MODULE_MAIN,
    MODULE_OEM_RECEIVER,
} ModuleName;

/* **************************************************************************************
 * Define the sizes of the various module tables.
 * **************************************************************************************
 */
#define MODULE_TABLE_NUM_ENTRIES_IN_KERNEL  (1)
#define MODULE_TABLE_NUM_ENTRIES_IN_MAIN    (25)
#define MODULE_TABLE_NUM_ENTRIES_IN_1ST_RX  (4)
#define MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX  (4)

#define MAX_NUM_MODULES  (MODULE_TABLE_NUM_ENTRIES_IN_KERNEL + MODULE_TABLE_NUM_ENTRIES_IN_MAIN + MODULE_TABLE_NUM_ENTRIES_IN_1ST_RX + MODULE_TABLE_NUM_ENTRIES_IN_OEM_RX)




/* **************************************************************************************
 * Module version info for updates.
 *
 * The Update GUI needs to display the version numbers of the modules installed on the
 * device so that the appropriate update can be selected. This structure is used to pass
 * the info to the GUI.
 * **************************************************************************************
*/

typedef struct
{
    ModuleId module;
    uint32_t version;
} ModuleVersionRecord;

typedef struct
{
    uint32_t numModules;
    ModuleVersionRecord info[MAX_NUM_MODULES];
} ModuleVersionInfo;


#ifdef __cplusplus
}
#endif

#endif /* SRC_MODULE_INFO_H_ */
