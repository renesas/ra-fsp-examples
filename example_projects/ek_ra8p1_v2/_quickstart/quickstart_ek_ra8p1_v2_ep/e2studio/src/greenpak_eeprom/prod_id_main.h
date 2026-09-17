
#include "r_typedefs.h"

#ifndef PROD_ID_MAIN_H_
#define PROD_ID_MAIN_H_

#define EEPROM_SIZE         (0x0100)             // 2K eeprom size 256 bytes
#define BLOCK_RESERVATION   (0x0100)             // 1K reserved, not protected for device configuration
#define BLOCK_OFFSET        (0x0000)             // Last 1k bytes
#define FIRST_BLOCK         (EEPROM_SIZE - 0x12) // Last 1k bytes size of header block +2 bytes
#define BLOCK_OFFSET_HEADER (0x00F0)             // Last 16 bytes fixed address
#define BLOCK_OFFSET_INITAL_SUBBLOCK (0x0FC)     // Last 16 bytes fixed address

#define FORCED_SCL_PIN (BSP_IO_PORT_04_PIN_10)
#define FORCED_SDA_PIN (BSP_IO_PORT_04_PIN_09)

typedef enum block
{
    IDENITY_BLOCK = 0,
    ASSEMBLY_VERSION,
    SERIAL_NUMBER,
    CONFIGURATION_BLOCK,
    MAC_ADDRESS,
    FIRMWARE_VERSION,
    UNDEFINED_TYPE
} e_block_t;

/* Head block structure */
typedef struct eeprom_header_data_block
{
    uint16_t end_marker;                 // always 0xFFF
    char_t   validation_block[8];        // 'RENESAS '
    uint16_t block_version_number;       // initial version V0000 (decimal Major(byte) Minor(byte)
    uint16_t next_subblock_location;     // absolute address (offset from start of eeprom) OR 0xFF,0xFF
    uint16_t reserverd;                  // not used, reserved for future use;
} s_eeprom_header_data_block_t;

typedef struct eeprom_common_data_block
{
    uint16_t next_subblock_location;     // absolute address (offset from start of eeprom) OR 0xFF,0xFF
    uint16_t sub_block_type;             // 0x00,0x01 Identify as identity Address Block Type
} s_eeprom_common_data_block_t;

/* Identity block structure */
typedef struct eeprom_identity_data_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    char_t  *identity_str ;               // ASCII Identity String, Null Terminated
} s_eeprom_identity_data_block_t;

typedef struct eeprom_assembly_version_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    uint16_t assembly_version_number;
} s_eeprom_assembly_version_block_t;

typedef struct eeprom_serial_number_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    char_t  *serial_str ;                // ASCII Serial String, Null Terminated
} s_eeprom_serial_number_block_t;

typedef struct eeprom_configuration_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    char_t  *configuration_str ;         // ASCII Configuration String, Null Terminated
} s_eeprom_configuration_block_t;

typedef struct eeprom_mac_address_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    uint8_t port_number;                 // Port Number
    uint8_t phytype;                     // Address for a Phy Type
    uint8_t ipconfig[6];                 // Array of 6 bytes for Mac address
} s_eeprom_mac_address_block_t;

typedef struct eeprom_firmware_version_block
{
    s_eeprom_common_data_block_t info;   // Common block for each sub_block
    uint16_t firmware_type_number;       // Numbers 0-4 represent from Undefined to PMIC
    char_t* firmware_version_identifier; // ASCII Configuration String, Null Terminated
} s_eeprom_firmware_version_block_t;


typedef struct eeprom_virtual_device
{
   uint8_t memory_block[BLOCK_RESERVATION];
} s_eeprom_virtual_device_t;


extern uint16_t map_virtual_address(uint16_t absolute_address);
extern uint8_t g_memory_block[BLOCK_RESERVATION];

extern bool_t prod_id_main(st_board_eeprom_data_t* p_eeprom_data);
extern bool_t display_blocklist(uint8_t * data);


#endif /* PROD_ID_MAIN_H_ */
