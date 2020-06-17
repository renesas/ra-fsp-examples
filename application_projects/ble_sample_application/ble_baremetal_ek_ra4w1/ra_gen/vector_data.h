/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (4)
#endif
/* ISR prototypes */
void r_icu_isr(void);
void gpt_counter_overflow_isr(void);
void agt_int_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_ICU_IRQ8 ((IRQn_Type) 0) /* BLEIRQ (Only for BLE middleware use) */
#define VECTOR_NUMBER_GPT1_COUNTER_OVERFLOW ((IRQn_Type) 1) /* GPT1 COUNTER OVERFLOW (Overflow) */
#define VECTOR_NUMBER_AGT0_INT ((IRQn_Type) 2) /* AGT0 INT (AGT interrupt) */
#define VECTOR_NUMBER_ICU_IRQ4 ((IRQn_Type) 3) /* ICU IRQ4 (External pin interrupt 4) */
typedef enum IRQn
{
    Reset_IRQn = -15,
    NonMaskableInt_IRQn = -14,
    HardFault_IRQn = -13,
    MemoryManagement_IRQn = -12,
    BusFault_IRQn = -11,
    UsageFault_IRQn = -10,
    SVCall_IRQn = -5,
    DebugMonitor_IRQn = -4,
    PendSV_IRQn = -2,
    SysTick_IRQn = -1,
    ICU_IRQ8_IRQn = 0, /* BLEIRQ (Only for BLE middleware use) */
    GPT1_COUNTER_OVERFLOW_IRQn = 1, /* GPT1 COUNTER OVERFLOW (Overflow) */
    AGT0_INT_IRQn = 2, /* AGT0 INT (AGT interrupt) */
    ICU_IRQ4_IRQn = 3, /* ICU IRQ4 (External pin interrupt 4) */
} IRQn_Type;
#endif /* VECTOR_DATA_H */
