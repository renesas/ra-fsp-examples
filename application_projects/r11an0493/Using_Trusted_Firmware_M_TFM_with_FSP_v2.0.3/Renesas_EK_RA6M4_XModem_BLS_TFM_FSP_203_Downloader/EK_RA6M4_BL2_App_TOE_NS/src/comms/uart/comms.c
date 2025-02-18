#include "../comms.h"

#ifdef COMMS_UART

static volatile uart_event_t g_uart0_event = 0;
static volatile bool g_timer0_event = false;

void g_timer0_callback(timer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    g_timer0_event = true;
}

void user_uart_callback(uart_callback_args_t *p_args)
{
    g_uart0_event = p_args->event;
}

fsp_err_t comms_open(void)
{
    fsp_err_t err;

    err = g_uart0.p_api->open(g_uart0.p_ctrl, g_uart0.p_cfg);
    if (FSP_SUCCESS == err)
    {
        err = g_timer0.p_api->open(g_timer0.p_ctrl, g_timer0.p_cfg);
    }

    return err;
}

void comms_send(uint8_t * p_src, uint32_t len)
{
    fsp_err_t err;

    g_uart0_event = 0;
    err = g_uart0.p_api->write(g_uart0.p_ctrl, p_src, len);
    if (FSP_SUCCESS == err)
    {
        while((UART_EVENT_TX_COMPLETE != g_uart0_event))
        {
            ;
        }
    }
}

fsp_err_t comms_read(uint8_t * p_dest, uint32_t *len, uint32_t timeout_milliseconds)
{
    fsp_err_t err;

    if (NO_TIMEOUT != timeout_milliseconds)
    {
        err = g_timer0.p_api->stop(g_timer0.p_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = g_timer0.p_api->periodSet(g_timer0.p_ctrl, (RAW_COUNT_MS * timeout_milliseconds));
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        g_timer0_event = false;
        err = g_timer0.p_api->start(g_timer0.p_ctrl);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        g_timer0_event = false;
    }

    g_uart0_event = 0;
    err = g_uart0.p_api->read(g_uart0.p_ctrl, p_dest, *len);
    if (FSP_SUCCESS == err)
    {
        while((UART_EVENT_RX_COMPLETE != g_uart0_event) && (true != g_timer0_event))
        {
            ;
        }
    }

    err = g_timer0.p_api->stop(g_timer0.p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (false == g_timer0_event)
    {
        return FSP_SUCCESS;
    }
    else
    {
        return FSP_ERR_TIMEOUT;
    }
}

#endif
