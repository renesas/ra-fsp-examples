/*
 * header.h
 *
 *  Created on: 2 Oct 2019
 *      Author: b3800274
 */

#ifndef HEADER_H_
#define HEADER_H_

#include "downloader_thread.h"

#define PRIMARY_NS_IMAGE_START_ADDRESS      0x00050000
#define PRIMARY_NS_IMAGE_END_ADDRESS        0x00088000
#define SECONDARY_NS_IMAGE_START_ADDRESS    0x000C0000
#define SECONDARY_NS_IMAGE_END_ADDRESS      0x000F8000
#define SECONDARY_S_IMAGE_START_ADDRESS     0x00088000
#define SECONDARY_S_IMAGE_END_ADDRESS       0x000C0000
#define FLASH_BLOCK_SIZE                    (32 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_NS_IMAGE_END_ADDRESS - SECONDARY_NS_IMAGE_START_ADDRESS) / FLASH_BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
}enable_disable_t;

void ThreadsAndInterrupts(enable_disable_t EnableDisable);
void display_image_slot_info(void);

#endif /* HEADER_H_ */
