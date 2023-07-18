/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (2)
        #endif
        /* ISR prototypes */
        void gpt_counter_overflow_isr(void);
        void r_icu_isr(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_GPT0_COUNTER_OVERFLOW ((IRQn_Type) 0) /* GPT0 COUNTER OVERFLOW (Overflow) */
        #define GPT0_COUNTER_OVERFLOW_IRQn          ((IRQn_Type) 0) /* GPT0 COUNTER OVERFLOW (Overflow) */
        #define VECTOR_NUMBER_ICU_IRQ9 ((IRQn_Type) 1) /* ICU IRQ9 (External pin interrupt 9) */
        #define ICU_IRQ9_IRQn          ((IRQn_Type) 1) /* ICU IRQ9 (External pin interrupt 9) */
        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */