/* webclib.c
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
 * Portions Copyright (C) 2011(2014) Renesas Electronics Corporation.
 * All rights reserved.
 *
 * Use license: Modified from standard BSD license.
 * 
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation, advertising 
 * materials, Web server pages, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by John Bartas. The name "John Bartas" may not be used to 
 * endorse or promote products derived from this software without 
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "net_thread.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "common_utils.h"
#include "usr_app.h"

#include "websys.h"     /* port dependent system files */
#include "webio.h"
#include "webfs.h"

char * wi_checkip(u_long * out, char * input);

/* ++ REE/EDC */
/* Single character put routine to fill the tx buffer */
static int32_t
wi_putchar(char ch, void * pv_sess)
{
    wi_sess * sess = (wi_sess *)pv_sess;
    /* Add another buffer if there is not one */
   if((sess->ws_txtail == NULL) ||
      /* or it won't fit, */
      (1 >= (WI_TXBUFSIZE - sess->ws_txtail->tb_total)))
   {
      /* get another buffer */
      if(wi_txalloc(sess) == NULL)
         return -1;
   }
   /* Put the char in the buffer */
   sess->ws_txtail->tb_data[sess->ws_txtail->tb_total] = ch;
   sess->ws_txtail->tb_total++;
   return 0;
}
/* -- REE/EDC */

void
wi_printf(wi_sess * sess, const char * fmt, ...)
{
   /* ++ REE/EDC - replaced vsprintf with call to formatted writer to 
      eliminate the temporary buffer & simplify the function */
   va_list     ap;
   /* Check for null pointer */
   if (!sess)
      return;
   /* Since it's a huge pain to check the connection after each CGI write,
    * we may get sometimes handed a dying connection. Ignore these. 
    */
   if(sess->ws_state == WI_ENDING)
      return;
   va_start(ap, fmt);
   fmtOut(fmt, wi_putchar, sess, ap);
   va_end(ap);
   /* ++ REE/EDC */
}


int
wi_putlong(wi_sess * sess, u_long value)
{
   wi_printf(sess, "%lu", value);
   return 0;
}

int
wi_putstring(wi_sess * sess, char * string)
{
   wi_printf(sess, "%s", string);
   return 0;
}

/* wi_formvalue() - get a value from a form. form should be in the 
 * ws_formlist of the session passed. name of form control for 
 * which to get the value is also passed.
 *
 * Returns prt to value string in formif name is found,
 * returns NULL if name is not found.
 */

char *
wi_formvalue( wi_sess * sess, char * ctlname )
{
   int      i;
   int      namelen;
   struct wi_form_s * form;

   namelen = (int)(strlen(ctlname));
   for(form = sess->ws_formlist; form; form = form->next)
   {
      for(i = 0; i < form->paircount; i++)
         /* ++ REE/EDC */        
         if( strnicmp(form->pairs[i].name, ctlname, (size_t)namelen - 1) == 0 )
         /* -- REE/EDC */
         {
            if( (*form->pairs[i].value) == 0)
               return NULL;
            else
               return form->pairs[i].value;
         }
   }

   return NULL;
}

#if 0
/* Not used in this build */
/* wi_checkip() - helper for wi_formipaddr() */

char * wi_checkip(u_long * out, char * input)
{
    int         octet;  /* Counter, 1-4 octets */
    unsigned    value;
    char *      cp;
    u_long      ipval;

    ipval = 0;
    cp = input;

    /* Note - try for 5 values; however finding more than 4 
     * is error. Correct loop exit if via "!cp".
     */
    for(octet = 1; octet < 5; octet++)
    {
        value = (unsigned)(atoi(cp));
        if((value == 0) && (*cp != '0'))
            return ("All chars must be digits or dots");

        if(value > 255)
            return ("All values must be 0-255");

        ipval = (ipval << 8) + value;
        cp = strchr(cp, '.');
        if(!cp)
            break;
        cp++;
    }

    if((octet > 4) || (octet < 2))
        return ("Must be 1 to 3 dots");

    /* If there are missing ocets (e.g. "10.1") then shift the high and low 
     * values to allow the traditional shorthand.
     */
    if(octet == 2)
    {
        ipval = (ipval & 0x000000FF) + ((ipval & 0x0000FF00) << 16);
    }
    else if(octet == 3)
    {
        ipval = (ipval & 0x000000FF) + ((ipval & 0x00FFFF00) << 8);
    }

    *out = (u_long)(htonl(ipval));
    return NULL;        /* No error */
}

#endif

/* wi_formipaddr()
 *
 * Extract a form address from name/value pairs attached to the session. 
 *
 * Returns NULL if IP was parsed and placed in ipaddr, else returns
 * some text describng the problem.
 */

#if 0
/* Not used in this build */
char *
wi_formipaddr( wi_sess * sess, char * ipname, u_long * ipaddr)
{
   char * iptext = wi_formvalue(sess, ipname );

   if (!iptext)
      return "unknown form control name";

   return (wi_checkip(ipaddr, iptext));
}
#endif

/* wi_formipaddr()
 *
 * Extract an integer from name/value pairs attached to the session. 
 *
 * Returns 0 if int was parsed and placed in return_int, else returns
 * negative ENP_ error.
 */

int
wi_formint(wi_sess * sess, char * name, long * return_int )
{
   char * valuetext;

   valuetext = wi_formvalue( sess, name );
   if(valuetext == NULL)
      return WIE_BADPARM;

   *return_int = (long)atol(valuetext);
   if( (*return_int == 0) && (*valuetext != '0'))
      return WIE_BADPARM;
   else
      return 0;
}


int
wi_formbool(wi_sess * sess, char * name)
{
   char * valuetext;

   valuetext = wi_formvalue( sess, name );
   if(valuetext == NULL)
   {
      return 0;   /* Default: FALSE */
   }

   if( (((*valuetext) | 0x20) == 'y') ||      /* Yes */
       (((*valuetext) | 0x20) == 't') ||      /* True */
         (*valuetext == 'c') )                /* checked */
   {
      return TRUE;
   }
   if( stricmp(valuetext, "on") == 0)      /* on */
      return TRUE;

   return FALSE;
}


