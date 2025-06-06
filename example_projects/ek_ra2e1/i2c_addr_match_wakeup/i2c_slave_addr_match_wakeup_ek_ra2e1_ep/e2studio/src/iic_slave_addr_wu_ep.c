/***********************************************************************************************************************
 * File Name    : iic_slave_addr_wu_ep.c
 * Description  : Contains data structures and functions used in iic_slave_addr_wu_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "bsp_clock_cfg.h"
#include "common_utils.h"
#include "iic_slave_addr_wu_ep.h"
#include "timer_init.h"

/**********************************************************************************************************************
* Private global variables declarations
**********************************************************************************************************************/
volatile bool g_sw_pushed_flag          = false;
volatile bool g_timer_sleep_flag        = false;
volatile bool g_iic_rx_complete_flag    = false;
volatile bool g_iic_tx_complete_flag    = false;
volatile bool g_iic_wu_detect_flag      = false;

static bool g_iic_wp_mode_state = false; /* false: Not wakeup mode, true: In wakeup mode */

static uint8_t g_iic_data[BUFF_SIZE]    = {RESET_VALUE};
static uint8_t g_iic_icier_backup       = RESET_VALUE;

static iic_slave_instance_ctrl_t* p_iic_instance_ct;

static void (* p_iic_wp_user_callback)(void) = NULL;

/***********************************************************************************************************************
* Private function declarations
***********************************************************************************************************************/
static fsp_err_t icu_init(void);
static fsp_err_t iic_slave_init(void);
static fsp_err_t lpm_init(void);
static fsp_err_t iic_slave_write(void);
static fsp_err_t iic_slave_read(void);
static void icu_deinit(void);
static void iic_slave_deinit(void);
static void lpm_deinit(void);
static void iic_wakeup_mode_operation_post_in_isr(void);
static void clock_out_enable(void);
static void clock_out_disable(void);
static void source_cancel_lpm_detect(void);
static void lpm_enter_procedure(void);
static fsp_err_t lpm_exit_procedure(void);
static fsp_err_t iic_slave_operation(void);
static fsp_err_t iic_slave_wakeup_ep_main_process(void);

/***********************************************************************************************************************
* Function implementations
***********************************************************************************************************************/
/***********************************************************************************************************************
 * Function Name: iic_slave_wakeup_ep_entry
 * Description  : This function starts the IIC Slave address match wakeup example operation.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_slave_wakeup_ep_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = iic_slave_wakeup_ep_main_process();
    handle_error(err, "\r\niic_slave_wakeup_ep_main_process failed!!!\r\n");
}
/***********************************************************************************************************************
* End of function iic_slave_wakeup_ep_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_wakeup_ep_main_process
 * Description  : This function performs the main process of the example project.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_slave_wakeup_ep_main_process(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    fsp_err_t           err     = FSP_SUCCESS;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Turn OFF the error LED */
    if (g_bsp_leds.led_count > 1)
    {
        LED_TURN_OFF(ERR_LED);
    }

    /* Turn OFF the state LED */
    LED_TURN_OFF(STATE_LED);

#if (USE_SERIAL_TERM == 1)
    /* Initialize UART module first to print log to serial terminal */
    err = TERM_INIT;
    if (FSP_SUCCESS != err)
    {
        /* Turn ON the error LED to indicate TERM_INIT failed */
        if (g_bsp_leds.led_count > 1)
        {
            LED_TURN_ON(ERR_LED);
        }

        /* Error trap */
        ERROR_TRAP;
    }
#endif /* USE_SERIAL_TERM */

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Print EP information and the board type */
    APP_PRINT(EP_INFO);
    APP_PRINT(BOARD_TYPE);

    /* Initialize the ICU module */
    err = icu_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nicu_init failed!!!\r\n");

    /* Initialize the IIC Slave module */
    err = iic_slave_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_slave_init failed!!!\r\n");

    /* Initialize the LPM module */
    err = lpm_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nlpm_init failed!!!\r\n");

    /* Initialize the GPT module */
    err = timer_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\ntimer_init failed!!!\r\n");

    /* Print current board state */
    APP_PRINT(NORMAL_STATUS);

    /* Turn ON the state LED */
    LED_TURN_ON(STATE_LED);

    while (true)
    {
        /* Perform IIC Slave operation */
        err = iic_slave_operation();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\niic_slave_operation failed!!!\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_slave_wakeup_ep_main_process
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_operation
 * Description  : This function performs the IIC Slave operation.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_slave_operation(void)
{
    fsp_err_t   err = FSP_SUCCESS;

    if (g_iic_rx_complete_flag)
    {
        /* Reset flag */
        g_iic_rx_complete_flag = false;

        /* Verify the I2C receive operation */
        APP_PRINT("\r\nRead the message from the Master board was successful!");
        if (RESET_VALUE != memcmp (&g_iic_data[0], EXPECTED_MESSAGE, EXPECTED_MESSAGE_SIZE))
        {
            err = FSP_ERR_TRANSFER_ABORTED;
            APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nThe received data mismatches the expected data!!!\r\n");
        }
        else
        {
            APP_PRINT("\r\nThe received data matches the expected data!\r\n");
        }

        /* Reset GPT timer */
        err = timer_reset();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\ntimer_reset failed!!!\r\n");
    }
    else if (g_iic_tx_complete_flag)
    {
        /* Reset flag */
        g_iic_tx_complete_flag = false;

        APP_PRINT("\r\nWrite the message to the Master board was successful!\r\n");

        /* Reset GPT timer */
        err = timer_reset();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\ntimer_reset failed!!!\r\n");
    }
    else if (g_timer_sleep_flag)
    {
        /* Reset flag */
        g_timer_sleep_flag = false;

        /* Procedure before enter to LPM */
        lpm_enter_procedure();

        /* Enter to LPM */
        err = R_LPM_LowPowerModeEnter(&g_lpm_sw_standby_ctrl);
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_LPM_LowPowerModeEnter API failed!!!***\r\n");

        /* Procedure after exit LPM */
        err = lpm_exit_procedure();
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\nlpm_exit_procedure failed!!!\r\n");
    }
    else
    {
        /* Do nothing */
    }

    return err;
}
/***********************************************************************************************************************
* End of function iic_slave_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: icu_init
 * Description  : This function initializes and enables ICU module.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_ctrl, &g_external_irq_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_ICU_ExternalIrqOpen API failed!!!***\r\n");

    /* Enable external interrupt */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "***\r\nR_ICU_ExternalIrqEnable API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function icu_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: icu_deinit
 * Description  : This function de-initializes ICU module.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize ICU module if it had already opened */
    if (MODULE_CLOSE != g_external_irq_ctrl.open)
    {
        err = R_ICU_ExternalIrqClose(&g_external_irq_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n***R_ICU_ExternalIrqClose API failed!!!***\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function icu_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_init
 * Description  : This function initializes IIC Slave module
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_slave_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize IIC Slave module */
    err = R_IIC_SLAVE_Open(&g_i2c_slave_ctrl, &g_i2c_slave_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_SLAVE_Open API failed!!!***\r\n");

    /* Set IIC instance control */
    iic_wakeup_instance_set(&g_i2c_slave_ctrl);

    /* Print the Slave address */
    APP_PRINT("\r\nSlave address: 0x%X\r\n", g_i2c_slave_cfg.slave);

    return err;
}
/***********************************************************************************************************************
* End of function iic_slave_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_deinit
 * Description  : This function de-initializes IIC Slave module
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void iic_slave_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize IIC Slave module if it had already opened */
    if (MODULE_CLOSE != g_i2c_slave_ctrl.open)
    {
        err = R_IIC_SLAVE_Close(&g_i2c_slave_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n***R_IIC_SLAVE_Close API failed!!!***\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function iic_slave_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lpm_init
 * Description  : This function initializes LPM module
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize LPM module */
    err = R_LPM_Open(&g_lpm_sw_standby_ctrl, &g_lpm_sw_standby_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_LPM_Open API failed!!!***\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function lpm_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lpm_deinit
 * Description  : This function de-initializes LPM module
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void lpm_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* De-initialize LPM module if it had already opened */
    if (MODULE_CLOSE != g_lpm_sw_standby_ctrl.lpm_open)
    {
        err = R_LPM_Close(&g_lpm_sw_standby_ctrl);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n***R_LPM_Close API failed!!!***\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function lpm_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lpm_enter_procedure
 * Description  : This function performs a procedure before entering LPM mode
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void lpm_enter_procedure(void)
{
    /* Turn OFF the state LED */
    LED_TURN_OFF(STATE_LED);

    /* Enable WU IRQ */
    iic_wakeup_cpu_interrupt_enable(WUI_PRIORITY, NULL, &iic_wakeup_callback);

    /* Pre process of IIC wakeup */
    iic_wakeup_mode_operation_pre();

    /* Disable clock output */
    clock_out_disable();

    /* GPT module stops with undefined in SW Standby mode */
    timer_deinit();

    /* Print the current board state */
    APP_PRINT(LPM_STATUS);

    /* Delay to ensure the information was displayed before entering LPM */
    R_BSP_SoftwareDelay(DELAY_ONE_HUNDRED_MS, BSP_DELAY_UNITS_MILLISECONDS);

#if (USE_SERIAL_TERM == 1)
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    /* SCI UART module stops with undefined in SW Standby mode */
    serial_deinit();
#endif
#endif /* USE_SERIAL_TERM */
}
/***********************************************************************************************************************
* End of function lpm_enter_procedure
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: lpm_exit_procedure
 * Description  : This function performs a procedure after exiting LPM mode.
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t lpm_exit_procedure(void)
{
    fsp_err_t err = FSP_SUCCESS;

#if (USE_SERIAL_TERM == 1)
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
    /* Re-initialize SCI UART module after wake up */
    err = TERM_INIT;
    if (FSP_SUCCESS != err)
    {
        /* Turn ON the error LED to indicate TERM_INIT failed */
        LED_TURN_OFF(ERR_LED);

        /* Error trap */
        ERROR_TRAP;
    }
#endif
#endif /* USE_SERIAL_TERM */

    /* Re-initialize GPT timer after the Slave board wake up */
    err = timer_init();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\ntimer_init failed!!!\r\n");

    /* Reset GPT timer */
    err = timer_reset();
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\ntimer_reset failed!!!\r\n");

    /* Post process of IIC wakeup */
    iic_wakeup_mode_operation_post();

    /* Enable Clock output */
    clock_out_enable();

    /* Detect source cancel LPM */
    source_cancel_lpm_detect();

    /* Disable WU IRQ */
    iic_wakeup_cpu_interrupt_disable();

    /* Turn ON the state LED */
    LED_TURN_ON(STATE_LED);

    /* Print current board state */
    APP_PRINT(NORMAL_STATUS);

    return err;
}
/***********************************************************************************************************************
* End of function lpm_exit_procedure
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: clock_out_enable
 * Description  : This function enables clock output.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void clock_out_enable(void)
{
    /* Configure clock output pin */
    R_IOPORT_PinCfg(&g_ioport_ctrl, CLKOUT_PIN, IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CLKOUT_COMP_RTC);

    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_UNLOCK;

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = CLK_DISABLE;

    /* Clock Output Frequency Division Ratio */
    R_SYSTEM->CKOCR_b.CKODIV = CLK_DIV;

    /* Select HOCO in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_Msk)
            | (BSP_CLOCKS_SOURCE_CLOCK_HOCO << R_SYSTEM_CKOCR_CKOSEL_Pos);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_HOCO;
#endif

    /* Clock Out Enable */
    R_SYSTEM->CKOCR_b.CKOEN = CLK_ENABLE;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function clock_out_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: clock_out_disable
 * Description  : This function disables clock output.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void clock_out_disable(void)
{
    /* Configure clock output pin */
    R_IOPORT_PinCfg(&g_ioport_ctrl, CLKOUT_PIN, IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CLKOUT_COMP_RTC);

    /* Unlock protect register */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_UNLOCK;

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = CLK_DISABLE;

    /* Clock Output Frequency Division Ratio */
    R_SYSTEM->CKOCR_b.CKODIV = CLK_DIV;

    /* Select HOCO in Clock Out Control Register */
#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK)
    R_SYSTEM->CKOCR = (R_SYSTEM->CKOCR & (uint8_t) ~R_SYSTEM_CKOCR_CKOSEL_Msk)
    | (BSP_CLOCKS_SOURCE_CLOCK_HOCO << R_SYSTEM_CKOCR_CKOSEL_Pos);
#else
    R_SYSTEM->CKOCR_b.CKOSEL = BSP_CLOCKS_SOURCE_CLOCK_HOCO;
#endif

    /* Clock Out Disable */
    R_SYSTEM->CKOCR_b.CKOEN = CLK_DISABLE;

    /* Lock protect register */
    R_SYSTEM->PRCR = (uint16_t) BSP_PRV_PRCR_LOCK;
}
/***********************************************************************************************************************
* End of function clock_out_disable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: source_cancel_lpm_detect
 * Description  : This function detects module which had canceled LPM mode
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
static void source_cancel_lpm_detect(void)
{
    if (g_iic_wu_detect_flag)
    {
        /* Reset flag */
        g_iic_wu_detect_flag = false;
        APP_PRINT(LPM_CANCEL_SOURCE_IIC);
    }
    else if (g_sw_pushed_flag)
    {
        /* Reset flag */
        g_sw_pushed_flag = false;
        APP_PRINT(LPM_CANCEL_SOURCE_IRQ);
    }
    else
    {
        APP_PRINT(LPM_CANCEL_SOURCE_NOT_DETECTED);
    }
}
/***********************************************************************************************************************
* End of function source_cancel_lpm_detect
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_instance_set
 * Description  : This function sets IIC Slave instance control
 * Arguments    : p_ctrl    Pointer to IIC Slave control block
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_instance_set(iic_slave_instance_ctrl_t* p_ctrl)
{
    p_iic_instance_ct = p_ctrl;
}
/***********************************************************************************************************************
* End of function iic_wakeup_instance_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_cpu_interrupt_enable
 * Description  : This function enables IIC address match wakeup interrupt
 * Arguments    : priority      Interrupt priority
 *                p_context     Pointer to user-provided context
 *                p_callback    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_cpu_interrupt_enable(uint32_t priority, void * p_context, void (* p_callback)(void))
{
    /* Enable WU IRQ */
    R_BSP_IrqCfgEnable(VECTOR_NUMBER_IIC0_WUI, priority, p_context);

    /* Set user callback function pointer using given information */
    p_iic_wp_user_callback = p_callback;
}
/***********************************************************************************************************************
* End of function iic_wakeup_cpu_interrupt_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_cpu_interrupt_disable
 * Description  : This function disables IIC address match wakeup interrupt
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_cpu_interrupt_disable(void)
{
    /* Disable WU IRQ */
    R_BSP_IrqDisable(VECTOR_NUMBER_IIC0_WUI);

    /* Reset user callback function pointer */
    p_iic_wp_user_callback = NULL;
}
/***********************************************************************************************************************
* End of function iic_wakeup_cpu_interrupt_disable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_mode_operation_pre
 * Description  : This function performs a procedure before wakeup interrupt was generated.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_mode_operation_pre(void)
{
    R_IIC0_Type* p_reg = R_IIC0;

    /* Refer to "26.8 Wakeup Function" section in User's Manual: Hardware (r01uh0852ej0150-ra2e1.pdf)
     * for wakeup operation */

    /* [1] Wait until I2C bus is open and stay in standby state */
    while(!(p_reg->ICCR2_b.BBSY == 0));

    /* Check if module is in slave receive state */
    while(!((p_reg->ICCR2_b.MST == 0) && (p_reg->ICCR2_b.TRS == 0)));

    /* [2] Negate internal reset (if asserted) */
    uint8_t current_iicr1 = p_reg->ICCR1;
    p_reg->ICCR1 = (current_iicr1 & ~(R_IIC0_ICCR1_IICRST_Msk & R_IIC0_ICCR1_ICE_Msk))
                   | ((IIC_IICR1_IICRST_VALUE << R_IIC0_ICCR1_IICRST_Pos) & R_IIC0_ICCR1_IICRST_Msk)
                   | ((1 << R_IIC0_ICCR1_ICE_Pos) & R_IIC0_ICCR1_ICE_Msk);

    /* [3] Set up WUACK for the desired wakeup mode. Enable wakeup interrupt */
    p_reg->ICWUR_b.WUACK = IIC_ICWUR_WUACK_VALUE;
    p_reg->ICWUR_b.WUIE = 1;

    /* [4] Enable wakeup function */
    p_reg->ICWUR_b.WUE = 1;

    /* [5] The IIC operating state is changed from PCLK synchronous to PCLK asynchronous */
    p_reg->ICWUR2 &= (uint8_t)(~R_IIC0_ICWUR2_WUSEN_Msk);

    /* Check wakeup function asynchronous operation status flag */
    while(!(p_reg->ICWUR2_b.WUASYF == 1));

    /* Backup the current interrupt enabling state before change */
    g_iic_icier_backup = p_reg->ICIER;

    /* [6] Disable all interrupt requests except WUI */
    p_reg->ICIER = 0x0;

    /* Set WP mode state flag */
    g_iic_wp_mode_state = true;
}
/***********************************************************************************************************************
* End of function iic_wakeup_mode_operation_pre
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_mode_operation_post
 * Description  : This function performs a procedure after MCU waked up by an interrupt other than a wakeup interrupt.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_mode_operation_post(void)
{
    R_IIC0_Type* p_reg = R_IIC0;

    if(g_iic_wp_mode_state == true)
    {
        /* The IIC operating state is changed from PCLK asynchronous to PCLK synchronous */
        p_reg->ICWUR2 |= R_IIC0_ICWUR2_WUSEN_Msk;

        /* Disable wakeup interrupt */
        p_reg->ICWUR_b.WUIE = 0;

        /* Disable wakeup function */
        p_reg->ICWUR_b.WUE = 0;

        /* Wait until the IIC operating state switch is completed */
        while(!(p_reg->ICWUR2_b.WUSYF == 1));

        /* Reset the interrupt enabling state using the backup value */
        p_reg->ICIER = g_iic_icier_backup;

        /* Reset WP mode state flag */
        g_iic_wp_mode_state = false;
    }
}
/***********************************************************************************************************************
* End of function iic_wakeup_mode_operation_post
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_mode_operation_post_in_isr
 * Description  : This function performs a procedure after wakeup interrupt was generated.
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_mode_operation_post_in_isr (void)
{
    volatile uint8_t dummy = 0x0;
    FSP_PARAMETER_NOT_USED(dummy);

    R_IIC0_Type* p_reg = R_IIC0;

    /* Refer to "26.8 Wakeup Function" section in User's Manual: Hardware (r01uh0852ej0150-ra2e1.pdf)
     * for wakeup operation */

    /* [9] Wait for WUF = 1. */
    while(!(p_reg->ICWUR_b.WUF == 1));

    /* [10] The IIC operating state is changed from PCLK asynchronous to PCLK synchronous */
    p_reg->ICWUR2 |= R_IIC0_ICWUR2_WUSEN_Msk;

    /* Wait until IIC operating state switch is completed */
    while(!(p_reg->ICWUR2_b.WUSYF == 1));

    /* [11] Write 0 to WUF. Read and check that WUF = 0 before returning from the interrupt handling */
    dummy = p_reg->ICWUR_b.WUF;
    p_reg->ICWUR_b.WUF = 0;
    while(!(p_reg->ICWUR_b.WUF == 0));

    /* [12] Disable wakeup interrupt */
    p_reg->ICWUR_b.WUIE = 0;

    /* [13] Disable wakeup function */
    p_reg->ICWUR_b.WUE = 0;

    /* Reset the interrupt enabling state using the backup value */
    p_reg->ICIER = g_iic_icier_backup;

    /* Reset WP mode state flag */
    g_iic_wp_mode_state = false;
}
/***********************************************************************************************************************
* End of function iic_wakeup_mode_operation_post_in_isr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_wu_isr
 * Description  : This function handles IIC Slave address match wakeup interrupt
 * Arguments    : None
 * Return value : None
 **********************************************************************************************************************/
void iic_slave_wu_isr(void)
{
    /* Save context if RTOS is used */
    FSP_CONTEXT_SAVE

    /* Post process of IIC wakeup */
    iic_wakeup_mode_operation_post_in_isr();

    /* Clear the IR flag */
    R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet());

    /* Check IIC Transmit or Receive mode */
    if(R_IIC0->ICCR2 & R_IIC0_ICCR2_TRS_Msk) /* The IIC Transmit mode */
    {
        /* Set the status flag to ensure this conditional clause execution only once */
        p_iic_instance_ct->notify_request = true;

        /* Clear the Start and Stop condition flag for Slave Read/Write operation */
        p_iic_instance_ct->p_reg->ICSR2 &= ((uint8_t) ~((uint8_t) ICSR2_STOP_BIT | (uint8_t) ICSR2_START_BIT));

        fsp_err_t err   =   FSP_SUCCESS;

        /* Perform IIC Slave write operation after wakeup */
        err = iic_slave_write();
        handle_error(err, "\r\niic_slave_write failed!!!\r\n");

        /* Enable the Start and Stop condition detection interrupt */
        p_iic_instance_ct->p_reg->ICIER = (uint8_t) ((uint8_t) IIC_STP_EN_BIT |
                                          (uint8_t) IIC_STR_EN_BIT |
                                          (uint8_t) IIC_TMO_EN_BIT |
                                          (uint8_t) IIC_ALD_EN_BIT |
                                          (uint8_t) IIC_NAK_EN_BIT |
                                          (uint8_t) IIC_RXI_EN_BIT |
                                          (uint8_t) IIC_TXI_EN_BIT);

        /* Enable start interrupt */
        p_iic_instance_ct->start_interrupt_enabled = true;

        /* Write the data byte, this will also release SCL */
        p_iic_instance_ct->p_reg->ICDRT = p_iic_instance_ct->p_buff[p_iic_instance_ct->loaded];
        p_iic_instance_ct->loaded++;

        /* Keep track of the actual number of transactions */
        p_iic_instance_ct->transaction_count++;

    }
    else /* The IIC Receive mode */
    {
        /* Perform dummy read after an address match detection */
        if (!p_iic_instance_ct->do_dummy_read)
        {
            p_iic_instance_ct->do_dummy_read = true;

            volatile uint8_t dummy_data = p_iic_instance_ct->p_reg->ICDRR;
            FSP_PARAMETER_NOT_USED(dummy_data);

            /* Clear the START condition detected flag */
            p_iic_instance_ct->p_reg->ICSR2 &= (uint8_t) ~(ICSR2_START_BIT);
        }
    }

    if(p_iic_wp_user_callback != NULL)
    {
        /* Call user callback function in R_IIC_SLAVE driver */
        p_iic_wp_user_callback();
    }

    /* Restore context if RTOS is used */
    FSP_CONTEXT_RESTORE
}
/***********************************************************************************************************************
* End of function iic_slave_wu_isr
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_write
 * Description  : This function performs a slave write operation
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_slave_write(void)
{
    fsp_err_t err   = FSP_SUCCESS;

    /* Prepare g_iic_data buffer */
    memset(&g_iic_data[0], RESET_VALUE, sizeof(g_iic_data));
    memcpy(&g_iic_data[0], SLAVE_MESSAGE, SLAVE_MESSAGE_SIZE);

    /* Perform IIC Slave write operation */
    err = R_IIC_SLAVE_Write(&g_i2c_slave_ctrl, &g_iic_data[0], SLAVE_MESSAGE_SIZE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_SLAVE_Write API failed!!!***");

    return err;
}
/***********************************************************************************************************************
* End of function iic_slave_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_read
 * Description  : This function performs a slave read operation
 * Arguments    : None
 * Return value : FSP_SUCCESS  Upon successful operation
 *                Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
static fsp_err_t iic_slave_read(void)
{
    fsp_err_t err   = FSP_SUCCESS;

    /* Clean g_iic_data buffer */
    memset(&g_iic_data[0], RESET_VALUE, sizeof(g_iic_data));

    /* Perform IIC Slave read operation */
    err = R_IIC_SLAVE_Read(&g_i2c_slave_ctrl, &g_iic_data[0], EXPECTED_MESSAGE_SIZE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n***R_IIC_SLAVE_Read API failed!!!***");

    return err;
}
/***********************************************************************************************************************
* End of function iic_slave_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_slave_callback
 * Description  : This function performs IIC Slave callback
 * Arguments    : p_args    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void iic_slave_callback(i2c_slave_callback_args_t *p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    switch (p_args->event)
    {
        case I2C_SLAVE_EVENT_RX_COMPLETE:
        {
            g_iic_rx_complete_flag = true;
            break;
        }
        case I2C_SLAVE_EVENT_RX_REQUEST:
        case I2C_SLAVE_EVENT_RX_MORE_REQUEST:
        {
            err = iic_slave_read();
            handle_error(err, "\r\niic_slave_read failed!!!\r\n");
            break;
        }
        case I2C_SLAVE_EVENT_TX_COMPLETE:
        {
            g_iic_tx_complete_flag = true;
            break;
        }
        case I2C_SLAVE_EVENT_TX_REQUEST:
        case I2C_SLAVE_EVENT_TX_MORE_REQUEST:
        {
            err = iic_slave_write();
            handle_error(err, "\r\niic_slave_write failed!!!\r\n");
            break;
        }
        default:
        {
            /* Do nothing */
        }
    }
}
/***********************************************************************************************************************
* End of function iic_slave_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: external_irq_callback
 * Description  : This function performs external IRQ callback
 * Arguments    : p_args    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    /* Suppress compiler messages about a parameter not being used in this function */
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set flag */
    g_sw_pushed_flag = true;
}
/***********************************************************************************************************************
* End of function external_irq_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: timer_sleep_callback
 * Description  : This function performs timer sleep callback
 * Arguments    : p_args    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void timer_sleep_callback(timer_callback_args_t *p_args)
{
    /* Suppress compiler messages about a parameter not being used in this function */
    FSP_PARAMETER_NOT_USED(p_args);

    /* Set flag */
    g_timer_sleep_flag = true;
}
/***********************************************************************************************************************
* End of function timer_sleep_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: iic_wakeup_callback
 * Description  : This function performs iic wakeup callback
 * Arguments    : p_args    Pointer to callback function
 * Return value : None
 **********************************************************************************************************************/
void iic_wakeup_callback(void)
{
    /* Set flag */
    g_iic_wu_detect_flag = true;
}
/***********************************************************************************************************************
* End of function iic_wakeup_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles error if any error occurred, closes all opened modules and traps the error.
 * Arguments    : err       error status
 *                err_str   error string
 * Return Value : None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Turn ON the error LED and turn OFF the state LED when an error occurred */
        if (g_bsp_leds.led_count > 1)
        {
            LED_TURN_ON(ERR_LED);
        }
        LED_TURN_OFF(STATE_LED);

        /* Print the error message */
        APP_ERR_PRINT(err_str);

        /* De-initialize the ICU module */
        icu_deinit();

        /* De-initialize the IIC Slave module */
        iic_slave_deinit();

        /* De-initialize the LPM module */
        lpm_deinit();

        /* De-initialize the GPT module */
        timer_deinit();

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
