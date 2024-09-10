/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name: r_ble_cli.c
 * Version : 1.0
 * Description : Command Line Interface.
 **********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli/r_ble_cli.h"

#if (BLE_CFG_CMD_LINE_EN == 1)

#include "cli/console/r_ble_console.h"

/******************************************************************************
 Macro definitions
*******************************************************************************/

#define BLE_PRV_CLI_PROMPT ("$")

/******************************************************************************
 Function prototype declarations
*******************************************************************************/

/* ANSI Escape sequence */
static void move_cursor_pos(uint8_t n)          { R_BLE_CLI_Printf("\033[%dG", n); } /* Move cursor to the absolute horizontal position */
static void move_cursor_right(uint8_t n)        { R_BLE_CLI_Printf("\033[%dC", n); } /* Move cursor right n lines */
static void move_cursor_left(uint8_t n)         { R_BLE_CLI_Printf("\033[%dD", n); } /* Move cursor left n lines */
static void clear_screen_from_cursor_down(void) { R_BLE_CLI_Printf("\033[J"); }      /* Clear screen from cursor down */
static void clear_line_from_cursor_right(void)  { R_BLE_CLI_Printf("\033[K"); }      /* Clear line from cursor right */
static void save_cursor_position(void)          { R_BLE_CLI_Printf("\0337"); }       /* Save cursor position and attributes */
static void restore_cursor_position(void)       { R_BLE_CLI_Printf("\0338"); }       /* Restore cursor position and attributes */
extern void console_register_event_cb(ble_cli_event_cb_t cb);

/******************************************************************************
 Global variables
*******************************************************************************/

static char                     gs_lines[BLE_CLI_NUM_OF_HISTORY][BLE_CLI_LINE_LEN];
static uint16_t                 gs_line_pos;
static uint16_t                 gs_line_idx;
static uint16_t                 gs_history_idx;
static uint16_t                 gs_num_of_cmds;
static const st_ble_cli_cmd_t   **gsp_cmds;
static ble_cli_abort_cb_t       gs_cmd_abort;


/*********************************************************************************************************************
 * Function Name: matched_strlen
 * Description  : Returns number of characters matched from the top.
 *                e.g. p_str1=gatts, p_str2=gattc, then return 4.
 * Arguments    : p_str1: string 1
 *                p_str2: string 2
 * Return Value : Number of characters matched from the top.
 ********************************************************************************************************************/
static uint16_t matched_strlen(const char *p_str1, const char *p_str2)
{
    uint16_t i = 0;

    if ((NULL == p_str1) || (NULL == p_str2))
    {
        return 0;
    }

    while (('\0' != p_str1[i]) && ('\0' != p_str2[i]))
    {
        if (p_str1[i] != p_str2[i])
        {
            break;
        }

        i++;
    }

    return i;
}

/*********************************************************************************************************************
 * Function Name: split_line
 * Description  : Split command line input from the user by spaces.
 * Arguments    : p_line - Command line to split
 *                p_argc - Number of parts by p_argc
 *                p_argv - Each part string
 * Return Value : None
 ********************************************************************************************************************/
static void split_line(char *p_line, int *p_argc, char *p_argv[])
{
    char *p_tp;

    *p_argc = 0;

    p_argv[0] = strtok(p_line, " ");

    if (NULL != p_argv[0])
    {
        (*p_argc) += 1;
        p_tp = strtok(NULL, " ");

        while (NULL != p_tp)
        {
            p_argv[*p_argc] = p_tp;
            (*p_argc) += 1;
            p_tp = strtok(NULL, " ");
        }
    }
}

/*********************************************************************************************************************
 * Function Name: cmd_full_match
 * Description  : Return a completely matched command instance.
 * Arguments    : argc            - Number of splitted command line parts.
 *                p_argv          - Splitted command line parts.
 *                p_cur_cmds      - Registered commands.
 *                num_of_cur_cmds - Number of registered command.
 *                p_found_cmds    - Store the found command instance. Or Null if not found.
 *                p_cmd_pos       - The position of the command in the splitted line parts (~ argc - 1).
 * Return Value : None
 ********************************************************************************************************************/
static void cmd_full_match(int argc, char *p_argv[],
                           const st_ble_cli_cmd_t *p_cur_cmds[], uint16_t num_of_cur_cmds,
                           const st_ble_cli_cmd_t **p_found_cmd,
                           uint16_t *p_cmd_pos)
{
    const st_ble_cli_cmd_t * const *p_cmds = p_cur_cmds;
    uint16_t len = num_of_cur_cmds;

    *p_found_cmd = NULL;
    *p_cmd_pos   = 0;

    for (uint16_t a = 0; a < argc; a++)
    {
        for (uint16_t c = 0; c < len; c++)
        {
            if ((strncmp(p_cmds[c]->p_name, p_argv[a], strlen(p_argv[a])) == 0) &&
                (strlen(p_cmds[c]->p_name) == strlen(p_argv[a])))
            {
                *p_found_cmd = p_cmds[c];
                *p_cmd_pos = a;

                if (NULL != p_cmds[c]->p_cmds)
                {
                    len = p_cmds[c]->num_of_cmds;
                    p_cmds = p_cmds[c]->p_cmds;
                }
                break;
            }
        }
    }
}

/*********************************************************************************************************************
 * Function Name: cmd_part_match
 * Description  : Return partially matched command instances.
 * Arguments    : argc                - Number of splitted command line parts.
 *                p_argv              - Splitted command line parts.
 *                p_cur_cmds          - Registered commands.
 *                num_of_cur_cmds     - Number of registered command.
 *                p_found_cmds        - Store the found command instances. Or Null if not found.
 *                p_num_of_found_cmds - Number of found commands.
 *                p_cmd_pos           - The position of the command in the splitted line parts (~ argc - 1).
 * Return Value : None
 ********************************************************************************************************************/
static void cmd_part_match(int argc, char *p_argv[],
                           const st_ble_cli_cmd_t *p_cur_cmds[], uint16_t num_of_cur_cmds,
                           const st_ble_cli_cmd_t *p_found_cmds[], uint16_t *p_num_of_found_cmds,
                           uint16_t *p_cmd_pos)
{
    const st_ble_cli_cmd_t * const *p_cmds = p_cur_cmds;
    uint16_t len = num_of_cur_cmds;

    *p_num_of_found_cmds = 0;
    *p_cmd_pos = 0;

    for (uint16_t a = 0; a < argc; a++)
    {
        bool found = false;

        for (uint16_t c = 0; c < len; c++)
        {
            if ((argc - 1) == a)
            {
                if (strncmp(p_cmds[c]->p_name, p_argv[a], strlen(p_argv[a])) == 0)
                {
                    p_found_cmds[*p_num_of_found_cmds] = p_cmds[c];
                    (*p_num_of_found_cmds) = (uint16_t)((*p_num_of_found_cmds) + 1);
                }
            }
            else
            {
                if ((strncmp(p_cmds[c]->p_name, p_argv[a], strlen(p_argv[a])) == 0) &&
                    (strlen(p_cmds[c]->p_name) == strlen(p_argv[a])))
                {
                    len = p_cmds[c]->num_of_cmds;
                    p_cmds = p_cmds[c]->p_cmds;
                    found = true;
                    break;
                }
            }
        }

        if (false == found)
        {
            return;
        }
    }
}

/*********************************************************************************************************************
 * Function Name: push_chars
 * Description  : Push characters on the current line buffer. If the buffer is full then discord the characters.
 * Arguments    : p_str - string to push
 *                len   - length of the string
 * Return Value : false if the buffer is full, otherwise return true.
 ********************************************************************************************************************/
static bool push_chars(const char *p_str, size_t len)
{
    bool pushed = true;

    for (uint16_t i = 0; i < len; i++)
    {
        if (gs_line_pos < (uint16_t)(BLE_CLI_LINE_LEN - 1))
        {
            gs_lines[gs_line_idx][gs_line_pos++] = p_str[i];
        }
        else
        {
            pushed = false;
        }
    }

    return pushed;
}

/*********************************************************************************************************************
 * Function Name: key_execute
 * Description  : Search a command instance which completely matched with the input string,
 *                and if found execute it or if not display error.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
static void key_execute(void)
{
    char *p_argv[BLE_CLI_NUM_OF_COMPLEMENT];
    int   argc = 0;

    R_BLE_CLI_Printf("\n");

    /* Spilt lines into command and arguments. */
    char line[BLE_CLI_LINE_LEN];
    strcpy(line, gs_lines[gs_line_idx]);

    split_line(line, &argc, p_argv);

    if (argc > 0)
    {
        /* Search command. */
        const st_ble_cli_cmd_t *p_found_cmd = NULL;
        uint16_t  cmd_pos = 0;

        cmd_full_match(argc, p_argv,
                       gsp_cmds, gs_num_of_cmds,
                       &p_found_cmd,
                       &cmd_pos);


        /* Execute command. */
        if (NULL != p_found_cmd)
        {
            if (((argc - cmd_pos) > 1) && (strcmp(p_argv[cmd_pos + 1], "help") == 0))
            {
                if (NULL != p_found_cmd->p_help)
                {
                    R_BLE_CLI_Printf("%s\n", p_found_cmd->p_help);
                }
                else
                {
                    R_BLE_CLI_Printf("no help.\n");
                }
            }
            else if (NULL != p_found_cmd->exec)
            {
                gs_cmd_abort = p_found_cmd->abort;
                p_found_cmd->exec(argc - cmd_pos, &p_argv[cmd_pos]);
            }
            else
            {
                R_BLE_CLI_Printf("no exec.\n");
            }
        }
        else
        {
            R_BLE_CLI_Printf("%s: command not found\n", gs_lines[gs_line_idx]);
        }


        /* Save the executed command as history. */
        gs_line_idx = (gs_line_idx + 1) & (BLE_CLI_NUM_OF_HISTORY - 1);
        memset(gs_lines[gs_line_idx], 0x00, BLE_CLI_LINE_LEN);
        gs_history_idx = BLE_CLI_NUM_OF_HISTORY;
    }


    if (NULL == gs_cmd_abort)
    {
        R_BLE_CLI_Printf("%s ", BLE_PRV_CLI_PROMPT);
    }

    gs_line_pos = 0;
}

/*********************************************************************************************************************
 * Function Name: key_history
 * Description  : Shows command history.
 * Arguments    : data - UP or DOWN key.
 * Return Value : None
 ********************************************************************************************************************/
static void key_history(uint8_t data)
{
    uint8_t num_of_skip = 0;

    do
    {
        if (KEY_UP == data)
        {
            if (BLE_CLI_NUM_OF_HISTORY == gs_history_idx)
            {
                gs_history_idx = gs_line_idx;
            }

            gs_history_idx = (gs_history_idx - 1) & (BLE_CLI_NUM_OF_HISTORY - 1);
        }
        else /* if (data == KEY_DOWN) */
        {
            if (BLE_CLI_NUM_OF_HISTORY == gs_history_idx)
            {
                gs_history_idx = gs_line_idx;
            }
            else
            {
                gs_history_idx = (gs_history_idx + 1) & (BLE_CLI_NUM_OF_HISTORY - 1);
            }
        }

        if ((strlen(gs_lines[gs_history_idx]) == 0) ||
            (gs_line_idx == gs_history_idx))
        {
            num_of_skip++;
        }
        else
        {
            memset(gs_lines[gs_line_idx], 0x00, BLE_CLI_LINE_LEN);
            memcpy(gs_lines[gs_line_idx], gs_lines[gs_history_idx], BLE_CLI_LINE_LEN);
            move_cursor_pos(0);
            clear_screen_from_cursor_down();
            R_BLE_CLI_Printf("%s ", BLE_PRV_CLI_PROMPT);
            R_BLE_CLI_Printf("%s", gs_lines[gs_line_idx]);
            gs_line_pos = (uint16_t)strlen(gs_lines[gs_line_idx]);
            break;
        }

    } while (num_of_skip <= (BLE_CLI_NUM_OF_HISTORY - 1));
}

/*********************************************************************************************************************
 * Function Name: key_complement
 * Description  : Complement partially input command with registered command name.
 *                Or if multiple command are found, show the commands.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
static void key_complement(void) // @suppress("Function length")
{
    /* Spilt lines into command and arguments. */
    char  *p_argv[BLE_CLI_NUM_OF_COMPLEMENT];
    int    argc    = 0;
    bool   space;
    size_t len;
    char   line[BLE_CLI_LINE_LEN];

    strcpy(line, gs_lines[gs_line_idx]);
    split_line(line, &argc, p_argv);

    len = strlen(gs_lines[gs_line_idx]);
    space = (0 != len) && (' ' == gs_lines[gs_line_idx][len - 1]);


    /* Search candidates. */
    uint16_t num_of_found_cmds = 0;
    const st_ble_cli_cmd_t *p_found_cmds[BLE_CLI_NUM_OF_COMPLEMENT] = { NULL };
    uint16_t  cmd_pos = 0;

    if (0 == argc)
    {
        for (uint16_t i = 0; i < gs_num_of_cmds; i++)
        {
            p_found_cmds[i] = gsp_cmds[i];
        }

        num_of_found_cmds = gs_num_of_cmds;
    }
    else if (space)
    {
        cmd_full_match(argc, p_argv,
                       gsp_cmds, gs_num_of_cmds,
                       &p_found_cmds[0],
                       &cmd_pos);

        if (NULL != p_found_cmds[0])
        {
            const st_ble_cli_cmd_t *p_cmd = p_found_cmds[0];

            for (uint16_t i = 0; i < p_cmd->num_of_cmds; i++)
            {
                p_found_cmds[i] = p_cmd->p_cmds[i];

            }

            num_of_found_cmds = p_cmd->num_of_cmds;
        }
    }
    else
    {
        cmd_part_match(argc, p_argv,
                       gsp_cmds, gs_num_of_cmds,
                       p_found_cmds, &num_of_found_cmds,
                       &cmd_pos);
    }

    if (0 == num_of_found_cmds)
    {
        return;
    }


    /* Find common characteristic string in the found commands. */
    size_t matched_len = strlen(p_found_cmds[0]->p_name);

    for (uint16_t i = 1; i < num_of_found_cmds; i++)
    {
        uint16_t len_temp = matched_strlen(p_found_cmds[0]->p_name,
                                      p_found_cmds[i]->p_name);

        if (matched_len > len_temp)
        {
            matched_len = len_temp;
        }
    }


    /* Complement the input command with the common characteristic string. */
    size_t input_len = space ? 0 : strlen(p_argv[argc - 1]);

    if (input_len < matched_len)
    {
        push_chars(&p_found_cmds[0]->p_name[input_len], matched_len - input_len);
    }

    /* If only one command is found, fully complement the command. */
    if (1 == num_of_found_cmds)
    {
        push_chars(" ", 1);
    }
    /* If multiple commands are found, print all found candidates. */
    else
    {
        R_BLE_CLI_Printf("\n");

        for (uint16_t i = 0; i < num_of_found_cmds; i++)
        {
            R_BLE_CLI_Printf("%s ", p_found_cmds[i]->p_name);
        }
    }


    /* Print current complemented command. */
    R_BLE_CLI_Printf("\n");
    move_cursor_left(BLE_CLI_LINE_LEN);
    clear_screen_from_cursor_down();
    R_BLE_CLI_Printf("%s ", BLE_PRV_CLI_PROMPT);
    R_BLE_CLI_Printf("%s", gs_lines[gs_line_idx]);
}

/*********************************************************************************************************************
 * Function Name: key_abort
 * Description  : Abort currently executing command.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
static void key_abort(void)
{
    if (NULL != gs_cmd_abort)
    {
        gs_cmd_abort();
        gs_cmd_abort = NULL;
        R_BLE_CLI_Printf("\n");
        R_BLE_CLI_Printf("%s ", BLE_PRV_CLI_PROMPT);
    }
}

/*********************************************************************************************************************
 * Function Name: key_move
 * Description  : Move the cursor left or right
 * Arguments    : data - LEFT or RIGHT key
 * Return Value : None
 ********************************************************************************************************************/
static void key_move(uint8_t data)
{
    if (KEY_LEFT == data)
    {
        if (gs_line_pos > 0)
        {
            move_cursor_left(1);
            gs_line_pos--;
        }
    }
    else /* if (data == KEY_RIGHT) */
    {
        if ('\0' != gs_lines[gs_line_idx][gs_line_pos])
        {
            move_cursor_right(1);
            gs_line_pos++;
        }
    }
}

/*********************************************************************************************************************
 * Function Name: key_backspace
 * Description  : Remove a character to the left of the cursor.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
static void key_backspace(void)
{
    if (0 == gs_line_pos)
    {
        return;
    }

    uint8_t diff;
    diff = (uint8_t)(BLE_CLI_LINE_LEN - gs_line_pos);
    memmove(&gs_lines[gs_line_idx][gs_line_pos-1], &gs_lines[gs_line_idx][gs_line_pos], diff);
    gs_lines[gs_line_idx][BLE_CLI_LINE_LEN-1] = '\0';
    gs_line_pos--;
    move_cursor_left(1);
    save_cursor_position();
    clear_line_from_cursor_right();
    R_BLE_CLI_Printf("%s", &gs_lines[gs_line_idx][gs_line_pos]);
    restore_cursor_position();
}

/*********************************************************************************************************************
 * Function Name: key_delete
 * Description  : Remove a character to the right of the cursor.
 * Arguments    : None
 * Return Value : None
 ********************************************************************************************************************/
static void key_delete(void)
{
    uint8_t diff;
    diff = (uint8_t)(BLE_CLI_LINE_LEN - gs_line_pos);
    memmove(&gs_lines[gs_line_idx][gs_line_pos], &gs_lines[gs_line_idx][gs_line_pos+1], diff);
    save_cursor_position();
    clear_screen_from_cursor_down();
    R_BLE_CLI_Printf("%s", &gs_lines[gs_line_idx][gs_line_pos]);
    restore_cursor_position();
}

/*********************************************************************************************************************
 * Function Name: key_char
 * Description  : Push a character on the current buffer.
 * Arguments    : data - character
 * Return Value : None
 ********************************************************************************************************************/
static void key_char(uint8_t data)
{
    /* The cursor is located at rightmost of the line. */
    if ('\0' == gs_lines[gs_line_idx][gs_line_pos])
    {
        bool pushed = false;

        pushed = push_chars((const char *)&data, 1);

        if (pushed)
        {
            R_BLE_CLI_Printf("%c", data);
        }
    }
    /* The cursor is located at middle of the line. */
    else
    {
        uint16_t diff;
        diff = (uint16_t)(BLE_CLI_LINE_LEN - gs_line_pos);
        memmove(&gs_lines[gs_line_idx][gs_line_pos+1], &gs_lines[gs_line_idx][gs_line_pos], (size_t)(diff - 1));
        push_chars((const char *)&data, 1);
        R_BLE_CLI_Printf("%c", data);
        save_cursor_position();
        R_BLE_CLI_Printf("%s", &gs_lines[gs_line_idx][gs_line_pos]);
        restore_cursor_position();
    }
}

ble_status_t R_BLE_CLI_Init(void) // @suppress("API function naming")
{
    console_init();
    return BLE_SUCCESS;
}

ble_status_t R_BLE_CLI_Terminate(void) // @suppress("API function naming")
{
    console_terminate();
    return BLE_SUCCESS;
}

ble_status_t R_BLE_CLI_RegisterCmds(const st_ble_cli_cmd_t * const p_cmds[], uint16_t num_of_cmds) // @suppress("API function naming")
{
    if((NULL == p_cmds) || (0 == num_of_cmds))
    {
        return BLE_ERR_INVALID_ARG;
    }

    gsp_cmds       = (const st_ble_cli_cmd_t **)p_cmds;
    gs_num_of_cmds = num_of_cmds;

    return BLE_SUCCESS;
}

ble_status_t R_BLE_CLI_RegisterEventCb(ble_cli_event_cb_t cb)
{
    console_register_event_cb(cb);
    return BLE_SUCCESS;
}

bool R_BLE_CLI_Process_With_Retval(void)
{
    uint8_t data;
    bool escape;
    bool ret;

    ret = console_getc(&data, &escape);

    if (false == ret)
    {
        return false;
    }

    /* During abort callback is exist, only Ctrl-C and Ctrl-D is accepted. */
    if ((NULL != gs_cmd_abort) && (KEY_CTRLC != data) && (KEY_CTRLD != data))
    {
        return false;
    }

    if (escape)
    {
        switch (data)
        {
            case KEY_LEFT:
            case KEY_RIGHT:
            {
                key_move(data);
            } break;

            case KEY_UP:
            case KEY_DOWN:
            {
                key_history(data);
            } break;

            default:
            {
                /* Do nothing. */
            } break;
        }
    }
    else
    {
        switch (data)
        {
            case KEY_CTRLC:
            case KEY_CTRLD:
            {
                key_abort();
                return true;
            } break;

            case KEY_LF:
            case KEY_CR:
            {
                key_execute();
                return true;
            } break;

            case KEY_BS:
            {
                key_backspace();
            } break;

            case KEY_DEL:
            {
                key_delete();
            } break;

            case KEY_TAB:
            {
                key_complement();
            } break;

            default:
            {
                key_char(data);
            }
        }
    }

    return false;
}

void R_BLE_CLI_Process(void) // @suppress("API function naming")
{
    (void)R_BLE_CLI_Process_With_Retval();
}

void R_BLE_CLI_SetCmdComp(void) // @suppress("API function naming")
{
    if (NULL != gs_cmd_abort)
    {
        gs_cmd_abort = NULL;
        R_BLE_CLI_Printf("\n");
        R_BLE_CLI_Printf("%s ", BLE_PRV_CLI_PROMPT);
    }
}

void R_BLE_CLI_PrintUnrecognized(void)
{
    R_BLE_CLI_Printf("%s: unrecognized operands\n", gs_lines[gs_line_idx]);
    R_BLE_CLI_SetCmdComp();
}

void R_BLE_CLI_PrintError(ble_status_t ret)
{
    R_BLE_CLI_Printf("command error. result : 0x%04x\n", ret);
    R_BLE_CLI_SetCmdComp();
}

#else /* (BLE_CFG_CMD_LINE_EN == 1) */

ble_status_t R_BLE_CLI_Init(void)
{
    return BLE_ERR_UNSUPPORTED;
}

ble_status_t R_BLE_CLI_Terminate(void)
{
    return BLE_ERR_UNSUPPORTED;
}

ble_status_t R_BLE_CLI_RegisterCmds(const st_ble_cli_cmd_t * const p_cmds[], uint16_t num_of_cmds)
{
    (void)p_cmds;
    (void)num_of_cmds;

    return BLE_ERR_UNSUPPORTED;
}

ble_status_t R_BLE_CLI_RegisterEventCb(ble_cli_event_cb_t cb)
{
    (void)cb;
    return BLE_ERR_UNSUPPORTED;
}

void R_BLE_CLI_Process(void)
{
}

void R_BLE_CLI_SetCmdComp(void)
{
}

void R_BLE_CLI_PrintUnrecognized(void)
{
}

void R_BLE_CLI_PrintError(ble_status_t ret)
{
    (void)ret;
}

#endif /* (BLE_CFG_CMD_LINE_EN == 1) */
