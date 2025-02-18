
#include "new_thread0.h"
#include "psa/error.h"
#include "psa/initial_attestation.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_psa/psa/crypto.h"
#include "hal_data.h"
#include "tfm_ns_interface.h"
#include "tfm_platform_api.h"
#include "tfm_ioctl_api.h"

fsp_err_t status;
uint32_t tfm_err_status;
const uint8_t string[] = "\r\n *** In non-secure image *** \n\r ";
const uint8_t flash_test_data[128] = "Renesas RA6M4 TFM TF-M Flash service write test data 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21.";
uint32_t flash_test_result = 0;
uint32_t tx_count = 0;
uint32_t rx_count = 0;
uint8_t rx_data = 0;
/* New Thread entry function */
/* pvParameters contains TaskHandle_t */

void new_thread0_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Use Attestation Service */
    tfm_ns_interface_init();
    uint8_t challenge[33]={0x2a};
    size_t size=0;
    size_t token_buf_size=800;
    uint8_t token_buf[800]={0x3b};
    size_t token_size=0;
    if(PSA_SUCCESS != psa_initial_attest_get_token_size(32,&size))
        while(1);
    if(PSA_SUCCESS != psa_initial_attest_get_token(challenge, 32, token_buf, token_buf_size, &token_size))
        while(1);

    /* Use Storage Service */
    uint8_t data1[10]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa};
    psa_storage_uid_t uid =1;
    size_t data_length=10;
    psa_storage_create_flags_t flags=PSA_STORAGE_FLAG_NO_CONFIDENTIALITY | PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION;
    if(PSA_SUCCESS != psa_its_set( uid, data_length, (const void *)&data1[0],flags))
        while(1);
    if(PSA_SUCCESS != psa_its_get( uid,0,data_length,(void *)token_buf,(size_t *)&data1[0]))
        while(1);


    /* Use Crypto Service */
    if(PSA_SUCCESS != psa_generate_random(data1, data_length))
        while(1);
    psa_algorithm_t      alg               = PSA_ALG_SHA_256;
    psa_hash_operation_t operation         = {0};
    uint8_t              input_1[]         = {'a', 'b', 'c'};
    uint8_t              expected_hash_1[] =
    {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, // NOLINT(readability-magic-numbers)
        0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, // NOLINT(readability-magic-numbers)
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad                          // NOLINT(readability-magic-numbers)
    };
    uint8_t input_2[]         = {'a', 'b', 'c', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd'};
    uint8_t expected_hash_2[] =
    {
        0x7b, 0xd3, 0x40, 0xd0, 0x29, 0xf7, 0x61, 0xd7, 0xaf, 0x22, 0xaf, 0x96, // NOLINT(readability-magic-numbers)
        0xeb, 0xaf, 0xba, 0x8f, 0xbe, 0x2b, 0xd6, 0xb3, 0x91, 0x58, 0xb6, 0xe2, // NOLINT(readability-magic-numbers)
        0x13, 0x81, 0x64, 0x18, 0xad, 0xd4, 0x14, 0x3b                          // NOLINT(readability-magic-numbers)
    };
    size_t expected_hash_len = PSA_HASH_SIZE(alg);

    if(PSA_SUCCESS != psa_hash_setup(&operation, alg))
        while(1);
    if(PSA_SUCCESS != psa_hash_update(&operation, input_1, sizeof(input_1)))
        while(1);
    if(PSA_SUCCESS != psa_hash_verify(&operation, expected_hash_1, expected_hash_len))
        while(1);
    if(PSA_SUCCESS != psa_hash_abort(&operation))
        while(1);

    if(PSA_SUCCESS != psa_hash_setup(&operation, alg))
        while(1);
    if(PSA_SUCCESS != psa_hash_update(&operation, input_2, sizeof(input_2)))
        while(1);
    if(PSA_SUCCESS != psa_hash_verify(&operation, expected_hash_2, expected_hash_len))
        while(1);
    if(PSA_SUCCESS != psa_hash_abort(&operation))
        while(1);


    tfm_err_status =  tfm_platform_flash_init(&flash_test_result);
    tfm_err_status =  tfm_platform_flash_erase((uint32_t) 0xF8000, 1, &flash_test_result);
    tfm_err_status =  tfm_platform_flash_write((uint32_t*)&flash_test_data[0], (uint32_t)0xF8000, 128, &flash_test_result);




    status = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
    if (status!= FSP_SUCCESS)
    {
        while(1);
    }

    //Test tx
    status = R_SCI_UART_Write(&g_uart0_ctrl, &string[0], sizeof(string));
    if (status!= FSP_SUCCESS)
    {
        while(1);
    }

    //Simple Echo code to test tx/rx using interrupts
    status = R_SCI_UART_Read(&g_uart0_ctrl, &rx_data, 1);
    if (status!= FSP_SUCCESS)
    {
        while(1);
    }


    /* TODO: add your own code here */
    while (1)
    {
        if (rx_count != 0)
        {
            status = R_SCI_UART_Write(&g_uart0_ctrl, &rx_data, 1);
            if (status!= FSP_SUCCESS)
            {
                while(1);
            }
            rx_count = 0;
            status = R_SCI_UART_Read(&g_uart0_ctrl, &rx_data, 1);
            if (status!= FSP_SUCCESS)
            {
                while(1);
            }
        }
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
        vTaskDelay (1);
    }
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    if (p_args->event == UART_EVENT_TX_COMPLETE)
    {
        tx_count++;
    }

    if (p_args->event == UART_EVENT_RX_COMPLETE)
    {
        rx_count++;
    }
}
