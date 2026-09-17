

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "r_typedefs.h"
#include "jlink_console.h"
#include "common_init.h"
#include "common_utils.h"
#include "board_i2c_master.h"
#include "board_i2c_master.h"

#include "prod_id_main.h"

/* Debug messages on console for this module  */
#define SHOW_DEBUG (false)

static bool_t read_eeprom_data(st_board_eeprom_data_t* p_eeprom_data);

uint8_t g_virtual_eeprom[16][16];
uint8_t g_memory_block[BLOCK_RESERVATION];

volatile fsp_err_t g_err = (FSP_ERR_ASSERTION);

uint8_t g_doit = 0;

#if SHOW_DEBUG
static fsp_err_t display_virtual_memory(uint8_t * mapped_memory, uint8_t bank);
#endif

static char_t s_print_buffer[BUFFER_LINE_LENGTH] = { };

#define OPERATION_DELAY (50)

char_t g_block_type_lookup[][24] = {
    "IDENITY_BLOCK",
    "ASSEMBLY_VERSION",
    "SERIAL_NUMBER",
    "CONFIGURATION_BLOCK",
    "MAC_ADDRESS",
    "FIRMWARE_VERSION",
    "" /* MUST ERMINAL WITH EMPTY STR ndx*/
};


/*
rem     ETHER_PHY_LSI_TYPE_KSZ8091RNB = 1,     ///< Select configuration for KSZ8091RNB.
rem     ETHER_PHY_LSI_TYPE_KSZ8041    = 2,     ///< Select configuration for KSZ8041.
rem     ETHER_PHY_LSI_TYPE_DP83620    = 3,     ///< Select configuration for DP83620.
rem     ETHER_PHY_LSI_TYPE_ICS1894    = 4,     ///< Select configuration for ICS1894.
rem     ETHER_PHY_LSI_TYPE_GPY111     = 5,     ///< Select configuration for GPY111.
rem     ETHER_PHY_LSI_TYPE_VSC8541    = 6,     ///< Select configuration for VSC8541.
rem     ETHER_PHY_LSI_TYPE_CUSTOM     = 0xFFU, ///< Select configuration for User custom.
*/

char_t g_phy_type_lookup[][24] = {
    "DEFAULT",
    "KSZ891RNB",
    "KSZ8041",
    "DP83620",
    "ICS1894",
    "GPY111",
    "VSC8541-02",
    "CUSTOM",
    "" /* MUST ERMINAL WITH EMPTY STR ndx*/
};

/**********************************************************************************************************************
 * Function Name: map_virtual_address
 * Description  : Map EEPROM virtual address.
 * Argument     : absolute_address
 * Return Value : .
 *********************************************************************************************************************/
uint16_t map_virtual_address(uint16_t absolute_address)
{
   volatile  uint16_t virtual_addr = 0xFFFF;

    if(absolute_address > BLOCK_OFFSET)
    {
        virtual_addr = absolute_address - BLOCK_OFFSET;
    }

    return(virtual_addr);
}

#if SHOW_DEBUG

/**********************************************************************************************************************
 * Function Name: display_virtual_memory
 * Description  : Display virtual memory from EEPROM
 * Arguments    : mapped_memory
 *              : bank
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t display_virtual_memory(uint8_t * mapped_memory, uint8_t bank)
{
	uint8_t bbyte = 0;

	if(bank == 16)
	{
	    sprintf (s_print_buffer, "BANK 0x%04d: %02x-%02x: Data [", (bank), (16 * bank), 0xFF );
	    print_to_console(s_print_buffer);
	}
	else
	{
	    sprintf (s_print_buffer,"BANK 0x%04d: %02x-%02x: Data [", (bank), (16 * bank), (16 *(bank + 1)) / 0xff );
        print_to_console(s_print_buffer);
	}

	for(bbyte = 0; bbyte < 15; bbyte++)
	{
	    sprintf (s_print_buffer,"0x%02x,", mapped_memory[bbyte]);
        print_to_console(s_print_buffer);
	}
	sprintf (s_print_buffer,"0x%02x]\r\n", mapped_memory[bbyte]);
    print_to_console(s_print_buffer);

	return (FSP_SUCCESS);
}
#endif

/* Check for valid Header*/
const char_t g_validation_str[] = "RENESAS\0";

/**********************************************************************************************************************
 * Function Name: register_blocklist
 * Description  : List the register by block
 * Arguments    : data
 *              : p_eeprom_data
 * Return Value : .
 *********************************************************************************************************************/
static bool_t register_blocklist(uint8_t * data, st_board_eeprom_data_t * p_eeprom_data)
{
    bool_t valid_display = false;
    uint8_t memory_block[256];

    memcpy(&memory_block, data, 256);

    volatile s_eeprom_header_data_block_t * p_hdr = ((s_eeprom_header_data_block_t *)&memory_block[BLOCK_OFFSET_HEADER]);
    volatile s_eeprom_common_data_block_t * p_tmp = (s_eeprom_common_data_block_t*)&p_hdr->next_subblock_location;


    if(strcmp(g_validation_str, (char_t *)p_hdr->validation_block) == 0)
    {
        /* Decode valid block */
        p_eeprom_data->populated = true;
    }
    else
    {
        return (valid_display);
    }

    volatile uint16_t virtual_offset = map_virtual_address(p_hdr->next_subblock_location);
    volatile uint16_t *p_data = (uint16_t *) &memory_block[virtual_offset];

    /* check that header block has been found */
    if (0xFFFF != *p_data)
    {
        p_tmp = (s_eeprom_common_data_block_t*)&p_hdr->next_subblock_location;
        do
        {
            virtual_offset = map_virtual_address(p_tmp->next_subblock_location);
            p_data = (uint16_t *) &memory_block[virtual_offset];

            if (0xFFFF != *p_data)
            {
                /* Display information about current block */
                virtual_offset = map_virtual_address(p_tmp->next_subblock_location);

                /* Increment to next block */
                p_tmp = ((s_eeprom_common_data_block_t *)&memory_block[virtual_offset]);

                if(p_tmp->sub_block_type < 8)
                {
                    /* debug id type only */
                    if( IDENITY_BLOCK == p_tmp->sub_block_type)
                    {
                        sprintf (s_print_buffer,"%s", (char_t *)&((s_eeprom_identity_data_block_t *) p_tmp)->identity_str);
                        p_eeprom_data->p_identity_str = malloc(strlen(s_print_buffer) + 1);
                        if (NULL != p_eeprom_data->p_identity_str)
                        {
                            memcpy(p_eeprom_data->p_identity_str, &s_print_buffer, strlen(s_print_buffer));
                        }
                        else
                        {
                            p_eeprom_data->populated = false;
                            break;
                        }
                    }
                    if( SERIAL_NUMBER == p_tmp->sub_block_type)
                    {
                        sprintf (s_print_buffer,"%s", (char_t *)&((s_eeprom_serial_number_block_t *) p_tmp)->serial_str);
                        p_eeprom_data->p_serial_str = malloc(strlen(s_print_buffer) + 1);
                        if (NULL != p_eeprom_data->p_serial_str)
                        {
                            memcpy(p_eeprom_data->p_serial_str, &s_print_buffer, strlen(s_print_buffer));
                        }
                        else
                        {
                            p_eeprom_data->populated = false;
                            break;
                        }
                    }
                    if( MAC_ADDRESS == p_tmp->sub_block_type)
                    {
                        sprintf (s_print_buffer,"%02x-%02x-%02x-%02x-%02x-%02x",
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[0],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[1],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[2],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[3],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[4],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[5]
                                      );

                        toupperstr((char_t *)&s_print_buffer, strlen(s_print_buffer));

                        p_eeprom_data->mac_t.p_mac_str = malloc(strlen(s_print_buffer) + 1);

                        if (NULL != p_eeprom_data->mac_t.p_mac_str)
                        {
                            memcpy(p_eeprom_data->mac_t.p_mac_str, &s_print_buffer, strlen(s_print_buffer));

                            p_eeprom_data->mac_t.port_number = ((s_eeprom_mac_address_block_t *) p_tmp)->port_number;
                            p_eeprom_data->mac_t.phytype     = ((s_eeprom_mac_address_block_t *) p_tmp)->phytype;

                            p_eeprom_data->mac_t.ipconfig[0] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[0];
                            p_eeprom_data->mac_t.ipconfig[1] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[1];
                            p_eeprom_data->mac_t.ipconfig[2] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[2];
                            p_eeprom_data->mac_t.ipconfig[3] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[3];
                            p_eeprom_data->mac_t.ipconfig[4] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[4];
                            p_eeprom_data->mac_t.ipconfig[5] = ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[5];
                        }
                        else
                        {
                            p_eeprom_data->populated = false;
                            break;
                        }
                    }
                }
            }
        }
        while (0xFFFF != *p_data);

        valid_display = true;
    }

    return (valid_display);
}

/**********************************************************************************************************************
 * Function Name: display_blocklist
 * Description  : Display EEPROM blocklist
 * Argument     : data
 * Return Value : .
 *********************************************************************************************************************/
bool_t display_blocklist(uint8_t * data)
{
    bool_t valid_display = false;
    uint8_t memory_block[256];

    memcpy(&memory_block, data, 256);

    volatile s_eeprom_header_data_block_t * p_hdr = ((s_eeprom_header_data_block_t *)&memory_block[BLOCK_OFFSET_HEADER]);
    volatile s_eeprom_common_data_block_t * p_tmp = (s_eeprom_common_data_block_t*)&p_hdr->next_subblock_location;


    if(strcmp(g_validation_str, (char_t *)p_hdr->validation_block) == 0)
    {
#if SHOW_DEBUG
        sprintf (s_print_buffer,"[GREEN]Header Block VALID [WHITE]\r\n");
        print_to_console(s_print_buffer);
        sprintf (s_print_buffer,"Block Format Version: %u\r\n", p_hdr->block_version_number);
        print_to_console(s_print_buffer);

        if(p_hdr->next_subblock_location != 0xFFFF)
        {
            sprintf (s_print_buffer,"Initial Block: 0x%04x\r\n", memory_block[BLOCK_OFFSET_INITAL_SUBBLOCK]);
            print_to_console(s_print_buffer);
        }
#endif // #if SHOW_DEBUG
    }
    else
    {
#if SHOW_DEBUG
        sprintf (s_print_buffer,"[RED]Header Block INVALID [WHITE]\r\n");
        print_to_console(s_print_buffer);
#endif // #if SHOW_DEBUG

        return (valid_display);
    }

    volatile uint16_t virtual_offset = map_virtual_address(p_hdr->next_subblock_location);
    volatile uint16_t *p_data = (uint16_t *) &memory_block[virtual_offset];

    /* check that header block has been found */
    if (0xFFFF != *p_data)
    {
        p_tmp = (s_eeprom_common_data_block_t*)&p_hdr->next_subblock_location;
        do
        {
            virtual_offset = map_virtual_address(p_tmp->next_subblock_location);
            p_data = (uint16_t *) &memory_block[virtual_offset];

            if (0xFFFF != *p_data)
            {
                /* Display information about current block */
                virtual_offset = map_virtual_address(p_tmp->next_subblock_location);

                /* Increment to next block */
                p_tmp = ((s_eeprom_common_data_block_t *)&memory_block[virtual_offset]);

                if(p_tmp->sub_block_type < 8)
                {
#if SHOW_DEBUG
                    sprintf (s_print_buffer,"Current Block Type: %s ", g_block_type_lookup[p_tmp->sub_block_type]);
                    print_to_console(s_print_buffer);
#endif // #if SHOW_DEBUG
                    /* debug id type only */
                    if( IDENITY_BLOCK == p_tmp->sub_block_type)
                    {
#if SHOW_DEBUG
                        sprintf (s_print_buffer,"[%s] \r\n", &((s_eeprom_identity_data_block_t *) p_tmp)->identity_str);
                        print_to_console(s_print_buffer);
#endif // #if SHOW_DEBUG
                    }
                    if( SERIAL_NUMBER == p_tmp->sub_block_type)
                    {
#if SHOW_DEBUG
                        sprintf (s_print_buffer,"[%s] \r\n", &((s_eeprom_serial_number_block_t *) p_tmp)->serial_str);
                        print_to_console(s_print_buffer);
#endif // #if SHOW_DEBUG
                    }
                    if( MAC_ADDRESS == p_tmp->sub_block_type)
                    {
#if SHOW_DEBUG
                        sprintf (s_print_buffer,"[port %d, PHY %s, MAC 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x] \r\n", ((s_eeprom_mac_address_block_t *) p_tmp)->port_number,
                                      g_phy_type_lookup[((s_eeprom_mac_address_block_t *) p_tmp)->phytype],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[0],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[1],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[2],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[3],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[4],
                                      ((s_eeprom_mac_address_block_t *) p_tmp)->ipconfig[5]
                                      );
                         print_to_console(s_print_buffer);
#endif // #if SHOW_DEBUG
                    }
                }
            }
        }
        while (0xFFFF != *p_data);

        valid_display = true;
    }

    return (valid_display);
}


/**********************************************************************************************************************
 * Function Name: read_eeprom_data
 * Description  : Read EEPROM data.
 * Argument     : p_eeprom_data
 * Return Value : .
 *********************************************************************************************************************/
bool_t read_eeprom_data(st_board_eeprom_data_t* p_eeprom_data)
{
	bool_t completed_without_issue = false;

    R_BSP_PinAccessEnable();

#if SHOW_DEBUG
    print_to_console("Current state of EEPROM USER Data \r\n");
#endif // #if SHOW_DEBUG


	R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, EK_RA8P1_I2C_SLAVE_ADDR_GREENPAK_EEPROM, I2C_MASTER_ADDR_MODE_7BIT);
    for(uint8_t rd_bank = 0; rd_bank < 16; rd_bank++)
    {
        for(uint8_t rd_byte = 0; rd_byte < 16; rd_byte++)
        {
            rd_sensor_reg8_8((16 * rd_bank) + rd_byte, (uint8_t *)&g_virtual_eeprom[rd_bank][rd_byte]);
        }
#if SHOW_DEBUG
       	display_virtual_memory(g_virtual_eeprom[rd_bank], rd_bank);
#endif // #if SHOW_DEBUG
    }

#if SHOW_DEBUG
    display_blocklist((uint8_t *)&g_virtual_eeprom);
#endif // #if SHOW_DEBUG

    register_blocklist((uint8_t *)&g_virtual_eeprom, p_eeprom_data);

	return (completed_without_issue);
}

/**********************************************************************************************************************
 * Function Name: prod_id_main
 * Description  : Main function for EEPROM id read.
 * Argument     : p_eeprom_data
 * Return Value : .
 *********************************************************************************************************************/
bool_t prod_id_main(st_board_eeprom_data_t* p_eeprom_data)
{
	bool_t completed_without_issue = false;
	fsp_err_t fsp_err = FSP_ERR_NOT_OPEN;

    if (0 == g_board_i2c_master_ctrl.open)
	{
		/* Open i2c if not opened already */
		fsp_err = R_IIC_MASTER_Open(&g_board_i2c_master_ctrl, &g_board_i2c_master_cfg);
		if (FSP_SUCCESS != fsp_err)
		{
            assert(-1);

			return (completed_without_issue);
		}
	}

#if SHOW_DEBUG
    print_to_console("Reading EEPROM state\r\n");
#endif

	completed_without_issue = read_eeprom_data(p_eeprom_data); // minimum x * 16 bytes

	return (completed_without_issue);
}
