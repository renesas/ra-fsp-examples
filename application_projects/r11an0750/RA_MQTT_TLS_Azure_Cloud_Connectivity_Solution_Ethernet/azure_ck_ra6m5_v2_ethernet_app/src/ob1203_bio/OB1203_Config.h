#ifndef OB1203_CONFIG_H
#define OB1203_CONFIG_H

#include "bsp_api.h"
#include "hal_data.h"
#include "rm_ob1203.h"
#include "rm_ob1203_api.h"
/* OB1203 Proximity mode */
extern rm_ob1203_mode_extended_cfg_t g_ob1203_sensor1_extended_cfg;
/* OB1203 Sensor */
extern const rm_ob1203_instance_t g_ob1203_sensor1;
extern rm_ob1203_instance_ctrl_t g_ob1203_sensor1_ctrl;
extern const rm_ob1203_cfg_t g_ob1203_sensor1_cfg;

/* OB1203 PPG mode */
extern rm_ob1203_mode_extended_cfg_t g_ob1203_sensor0_extended_cfg;
/* OB1203 Sensor */
extern const rm_ob1203_instance_t g_ob1203_sensor0;
extern rm_ob1203_instance_ctrl_t g_ob1203_sensor0_ctrl;
extern const rm_ob1203_cfg_t g_ob1203_sensor0_cfg;

#ifndef ob1203_comms_i2c_callback
void ob1203_comms_i2c_callback(rm_ob1203_callback_args_t *p_args);
#endif
#ifndef ob1203_irq_callback
void ob1203_irq_callback(rm_ob1203_callback_args_t *p_args);
#endif
#endif // OB1203_CONFIG_H
