#include "OB1203_Config.h"

#if BSP_CFG_RTOS
#if BSP_CFG_RTOS == 1
TX_SEMAPHORE g_ob1203_sensor0_semaphore_handle;
CHAR g_ob1203_sensor0_semaphore_name[] = "g_ob1203_sensor0 semaphore";
#elif BSP_CFG_RTOS == 2
SemaphoreHandle_t g_ob1203_sensor0_semaphore_handle;
StaticSemaphore_t g_ob1203_sensor0_semaphore_memory;
#endif

/* Semaphore to wait for callback  */
rm_ob1203_semaphore_t g_ob1203_sensor0_semaphore =
{
    .p_semaphore_handle = &g_ob1203_sensor0_semaphore_handle,
#if BSP_CFG_RTOS == 1 // ThreadX
    .p_semaphore_name = &g_ob1203_sensor0_semaphore_name[0],
#elif BSP_CFG_RTOS == 2 // FreeRTOS
    .p_semaphore_memory = &g_ob1203_sensor0_semaphore_memory,
#endif
};
#endif

#if BSP_CFG_RTOS
#if BSP_CFG_RTOS == 1
TX_SEMAPHORE g_ob1203_sensor1_semaphore_handle;
CHAR g_ob1203_sensor1_semaphore_name[] = "g_ob1203_sensor1 semaphore";
#elif BSP_CFG_RTOS == 2
SemaphoreHandle_t g_ob1203_sensor1_semaphore_handle;
StaticSemaphore_t g_ob1203_sensor1_semaphore_memory;
#endif

/* Semaphore to wait for callback  */
rm_ob1203_semaphore_t g_ob1203_sensor1_semaphore =
{
    .p_semaphore_handle = &g_ob1203_sensor1_semaphore_handle,
#if BSP_CFG_RTOS == 1 // ThreadX
    .p_semaphore_name = &g_ob1203_sensor1_semaphore_name[0],
#elif BSP_CFG_RTOS == 2 // FreeRTOS
    .p_semaphore_memory = &g_ob1203_sensor1_semaphore_memory,
#endif
};
#endif

extern const rm_comms_instance_t g_comms_i2c_device6;
extern const rm_comms_instance_t g_comms_i2c_device3;
  /*-----------OB1203 sensor0-------*/
extern rm_ob1203_api_t const g_ob1203_on_ob1203_ppg;
rm_ob1203_mode_extended_cfg_t g_ob1203_sensor0_extended_cfg =
{ .p_api = (void*) &g_ob1203_on_ob1203_ppg,
  .ppg_prox_gain = RM_OB1203_PPG_PROX_GAIN_1,
  .led_order = RM_OB1203_LED_IR_FIRST_RED_SECOND,
  .ppg_sensor_mode = RM_OB1203_PPG_SENSOR_MODE_PPG2,
  .ppg_interrupt_type = RM_OB1203_PPG_INTERRUPT_TYPE_DATA,
  .ppg_ir_led_current = 0x366,
  .ppg_red_led_current = 0x1B3,
  .ppg_power_save_mode = RM_OB1203_POWER_SAVE_MODE_DISABLE,
  .ppg_ir_led_ana_can = RM_OB1203_ANALOG_CANCELLATION_DISABLE,
  .ppg_red_led_ana_can = RM_OB1203_ANALOG_CANCELLATION_DISABLE,
  .ppg_num_averaged_samples = RM_OB1203_NUM_AVERAGED_SAMPLES_8,
  .ppg_width_period = RM_OB1203_PPG_WIDTH_130US_PERIOD_1P25MS,
  .ppg_fifo_rollover = RM_OB1203_FIFO_ROLLOVER_ENABLE,
  .ppg_fifo_empty_num = 0x0C, };

rm_ob1203_instance_ctrl_t g_ob1203_sensor0_ctrl;

const rm_ob1203_cfg_t g_ob1203_sensor0_cfg =
{
#if BSP_CFG_RTOS
    .p_semaphore = &g_ob1203_sensor0_semaphore,
#endif
  .semaphore_timeout = 0xFFFFFFFF,
  .p_comms_instance = &g_comms_i2c_device3,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_external_irq14)
    .p_irq_instance = NULL,
    .p_irq_callback = NULL,
#else
  .p_irq_instance = &g_external_irq14,
  .p_irq_callback = ob1203_irq_callback,
#endif
#undef RA_NOT_DEFINED
  .p_comms_callback = ob1203_comms_i2c_callback,
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = (void*) &g_ob1203_sensor0_extended_cfg, };

const rm_ob1203_instance_t g_ob1203_sensor0 =
{ .p_ctrl = &g_ob1203_sensor0_ctrl, .p_cfg = &g_ob1203_sensor0_cfg, .p_api = &g_ob1203_on_ob1203, };
/*-----------OB1203 sensor1------*/
extern rm_ob1203_api_t const g_ob1203_on_ob1203_proximity;
rm_ob1203_mode_extended_cfg_t g_ob1203_sensor1_extended_cfg =
{ .p_api = (void*) &g_ob1203_on_ob1203_proximity,
  .ppg_prox_gain = RM_OB1203_PPG_PROX_GAIN_1,
  .led_order = RM_OB1203_LED_IR_FIRST_RED_SECOND,
  .prox_sleep = RM_OB1203_SLEEP_AFTER_INTERRUPT_DISABLE,
  .prox_interrupt_type = RM_OB1203_PROX_INTERRUPT_TYPE_NORMAL,
  .prox_interrupt_persist = 0x02,
  .prox_led_current = 0x100,
  .prox_ana_can = RM_OB1203_ANALOG_CANCELLATION_DISABLE,
  .prox_dig_can = 0x100,
  .prox_num_led_pulses = RM_OB1203_NUM_LED_PULSES_8,
  .prox_upper_threshold = 0x0600,
  .prox_lower_threshold = 0x0000,
  .prox_width_period = RM_OB1203_PROX_WIDTH_42US_PERIOD_100MS,
  .prox_moving_average = RM_OB1203_MOVING_AVERAGE_DISABLE,
  .prox_hysteresis = 0x00, };

rm_ob1203_instance_ctrl_t g_ob1203_sensor1_ctrl;

const rm_ob1203_cfg_t g_ob1203_sensor1_cfg =
{
#if BSP_CFG_RTOS
    .p_semaphore = &g_ob1203_sensor1_semaphore,
#endif
  .semaphore_timeout = 0xFFFFFFFF,
  .p_comms_instance = &g_comms_i2c_device6,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_external_irq14)
    .p_irq_instance = NULL,
    .p_irq_callback = NULL,
#else
  .p_irq_instance = &g_external_irq14,
  .p_irq_callback = ob1203_irq_callback,
#endif
#undef RA_NOT_DEFINED
  .p_comms_callback = ob1203_comms_i2c_callback,
#if defined(NULL)
    .p_context           = NULL,
#else
  .p_context = &NULL,
#endif
  .p_extend = (void*) &g_ob1203_sensor1_extended_cfg, };

const rm_ob1203_instance_t g_ob1203_sensor1 =
{   .p_ctrl = &g_ob1203_sensor1_ctrl, 
    .p_cfg = &g_ob1203_sensor1_cfg, 
    .p_api = &g_ob1203_on_ob1203, };
