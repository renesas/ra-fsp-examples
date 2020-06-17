/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
{
    [0] = r_icu_isr, /* BLEIRQ (Only for BLE middleware use) */
    [1] = gpt_counter_overflow_isr, /* GPT1 COUNTER OVERFLOW (Overflow) */
    [2] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
    [3] = r_icu_isr, /* ICU IRQ4 (External pin interrupt 4) */
};
const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
{
    [0] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ8), /* BLEIRQ (Only for BLE middleware use) */
    [1] = BSP_PRV_IELS_ENUM(EVENT_GPT1_COUNTER_OVERFLOW), /* GPT1 COUNTER OVERFLOW (Overflow) */
    [2] = BSP_PRV_IELS_ENUM(EVENT_AGT0_INT), /* AGT0 INT (AGT interrupt) */
    [3] = BSP_PRV_IELS_ENUM(EVENT_ICU_IRQ4), /* ICU IRQ4 (External pin interrupt 4) */
};
#endif
