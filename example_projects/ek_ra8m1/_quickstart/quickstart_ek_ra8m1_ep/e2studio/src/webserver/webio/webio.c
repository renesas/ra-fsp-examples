/* webio.c
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
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
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include "bsp_api.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_DNS.h"
#include "NetworkBufferManagement.h"

#include "strstri.h"

#include "websys.h"     /* port dependent system files */
#include "webio.h"
#include "webfs.h"
#include "webCGI.h"

#include "common_utils.h"
#include "portable.h"


/* This file contains the main entry points for the webio library */
/* Comment this line out to turn ON module trace in this file */
#undef _TRACE_ON_

#ifndef _TRACE_ON_
#undef TRACE
#define TRACE(x)
#endif

/** 0.0.0.0 */
#define INADDR_ANY          ((uint32_t)0x00000000UL)

/* The web server "listen" socket */
socktype wi_listen;
socktype wi_highsocket;

/* Port number on which to listen. May be changed prior to calling webinit */
int   httpport = 80;
int   wi_running = FALSE;  /* TRUE while server is running */
char * wi_rootfile = "index.html";  /* File name to substitute for "/" */


/* Flag to permit connections by the localhost only (security). */
int   wi_localhost;

char *   g_ws_uri = NULL;                 /* URI from request (often inside rxbuf) */

#ifdef WI_THREAD
/* ++ REE/EDC */
TickType_t   wi_seltmo = portMAX_DELAY; /* on thread, block is OK */
/* -- REE/EDC */
#else
TickType_t   wi_seltmo = {0}; /* polled mode - no blocking */
#endif

void wi_set_language( wi_sess * sess );
void wi_badform(wi_sess * sess, char * errmsg);

uint32_t fi = 1;
SocketSet_t sel_recv = NULL;
SocketSet_t sel_send = NULL;

/* webinit()
 *
 * This should be the first call made to the web server. It initializes
 * the embedded file system starts a listen
 *
 * Parameters:
 * int stdfiles - flag to use system "fopen" file calls as well as embedded FS
 *
 * Returns 0 if OK, else negative error code.
 */
int
wi_init()
{
    struct freertos_sockaddr   wi_sin;
    static const TickType_t xReceiveTimeOut = portMAX_DELAY;
    WinProperties_t xWinProps;

    if(sel_recv == NULL)
    {
        sel_recv = FreeRTOS_CreateSocketSet();
        sel_send = FreeRTOS_CreateSocketSet();
    }

    /* Attempt to open the socket. */
    wi_listen = FreeRTOS_socket( FREERTOS_AF_INET,
                                        FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
                                        FREERTOS_IPPROTO_TCP);

    /* Check the socket was created. */
    configASSERT( wi_listen != FREERTOS_INVALID_SOCKET );

    /* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
    FreeRTOS_setsockopt() to change the buffer sizes from their default then do
    it here!.  (see the FreeRTOS_setsockopt() documentation. */

    /* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
    be used with FreeRTOS_setsockopt() to change the sliding window size from
    its default then do it here! (see the FreeRTOS_setsockopt()
    documentation. */

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt( wi_listen,
                         0,
                         FREERTOS_SO_RCVTIMEO,
                         &xReceiveTimeOut,
                         sizeof( xReceiveTimeOut ) );

    FreeRTOS_setsockopt( wi_listen,
                         0,
                         FREERTOS_SO_SNDTIMEO,
                         &xReceiveTimeOut,
                         sizeof( xReceiveTimeOut ) );



    /* Fill in the buffer and window sizes that will be used by the socket. */
    xWinProps.lTxBufSize = 4 * ipconfigTCP_MSS;
    xWinProps.lTxWinSize = 4;
    xWinProps.lRxBufSize = 2 * ipconfigTCP_MSS;
    xWinProps.lRxWinSize = 2;


    /* Set the window and buffer sizes. */
    FreeRTOS_setsockopt (wi_listen, 0, FREERTOS_SO_WIN_PROPERTIES, (void*) &xWinProps, sizeof(xWinProps));


    /* Set the listening port to 80. */
    wi_sin.sin_port = ( uint16_t ) 80;
    wi_sin.sin_port = FreeRTOS_htons( wi_sin.sin_port );

    /* Bind the socket to the port that the client RTOS task will send to. */
    FreeRTOS_bind( wi_listen, &wi_sin, sizeof( wi_sin ) );

    /* Set the socket into a listening state so it can accept connections.
    The maximum number of simultaneous connections is limited to 20. */
    FreeRTOS_listen( wi_listen, 15 );

/*    TURN_GREEN_OFF; */

    wi_running = TRUE;
    return 0;

}

/* webpoll() - entry point for driving webio in a "polled" manner.
 * this checks for any work that needs to be done and returns. It
 * may be preempted, but is not re-entrant.
 *
 * Returns negative code on error, else number of open sessions.
 * Return of 0 means no sessions and no error.
 */

uint32_t trace_io = 0;

static void cleanup(SocketSet_t * p_sel_recv,  SocketSet_t * p_sel_send)
{
/*
    FreeRTOS_DeleteSocketSet(*p_sel_recv);
    FreeRTOS_DeleteSocketSet(*p_sel_send);

    *p_sel_recv = NULL;
    *p_sel_send = NULL;
*/

    (void)p_sel_recv;
    (void)p_sel_send;
}

int wi_poll()
{
   wi_sess * sess;
   wi_sess * next_sess;
   BaseType_t   sessions = (BaseType_t)FREERTOS_INVALID_SOCKET;

   int   recvs;
   int   sends;
   int   error = 0;
   char * data;

/* add listen sock to select list */
   FreeRTOS_FD_SET(wi_listen, sel_recv, eSELECT_READ);

   wi_highsocket = wi_listen;

   /* loop through list of open sessions looking for work */
   recvs = sends = 0;
   for(sess = wi_sessions; sess; sess = sess->ws_next)
   {
      if(sess->ws_socket == FREERTOS_INVALID_SOCKET)
         continue;

      /* If socket is reading, load for a select */
      if(sess->ws_state == WI_HEADER)
      {
         recvs++;
         FreeRTOS_FD_SET(sess->ws_socket, sel_recv, eSELECT_READ);

         if(sess->ws_socket > wi_highsocket)
             wi_highsocket = sess->ws_socket;
      }

      if((sess->ws_txbufs) ||
         (sess->ws_flags & WF_BINARY))
      {
         sends++;
         FreeRTOS_FD_SET(sess->ws_socket, sel_send, eSELECT_WRITE);

         if(sess->ws_socket > wi_highsocket)
             wi_highsocket = sess->ws_socket;
      }
   }

   wi_highsocket++;

   /* See if any of the sockets have input or ready to send */
   sessions = FreeRTOS_select( sel_recv, wi_seltmo  );

   if(sessions == (BaseType_t)FREERTOS_INVALID_SOCKET)
   {
      error = errno;
      /* ++ REE/EDC */
      TRACE(("select error %d\n", error ));
      /* -- REE/EDC */
      cleanup( &sel_recv, &sel_recv);
      return WIE_SOCKET;
   }

   /* see if we have a new connection request */
   if(FreeRTOS_FD_ISSET(wi_listen, sel_recv))
   {
      error = wi_sockaccept();

      if(error)
      {
         /* ++ REE/EDC */
         TRACE(("Socket accept error %d\n", error ));
         /* -- REE/EDC */
         cleanup( &sel_recv, &sel_recv);
         return error;
      }
   }

   sess = wi_sessions;

   while(sess)
   {
      next_sess = sess->ws_next;

      /* jump to here to accelerate things if a session changes state */
another_state:
      switch(sess->ws_state)
      {
      case WI_HEADER:
         /* See if there is data to read */
         if(FreeRTOS_FD_ISSET(sess->ws_socket, sel_recv))
         {
             error = recv(sess->ws_socket,
                         sess->ws_rxbuf + sess->ws_rxsize,
                         (size_t)(sizeof(sess->ws_rxbuf) - (size_t)(sess->ws_rxsize)),
                         0);

            /* ++ REE/EDC */
            /* Just to irritate engineering, lwIP returns 0 when some
               errors occur. Nice */
            if(error <= 0)
            /* -- REE/EDC */
            {


               error = errno;
               /* ++ REE/EDC */
               TRACE(("sock recv error %d\n", error ));
               wi_delsess(sess);
               sess = next_sess;
               /* -- REE/EDC */
               cleanup( &sel_recv, &sel_recv);
               return WIE_SOCKET;
            }
            /* ++ REE/EDC */
            #ifdef _TRACE_REQUEST_
            else
            {
                TRACE(("WI_HEADER: %s\r\n", sess->ws_rxbuf));
            }
            #endif
            /* -- REE/EDC */
            sess->ws_rxsize += error;
            /* ++ REE/EDC */
            sess->ws_last = (wi_sec)(cticks());
            /* -- REE/EDC */
         }
         if(sess->ws_rxsize)  /* unprocessed input http */
         {
            error = wi_parseheader( sess );  /* Make a best effort to process input */
            sessions++;
         }
         /* If the logic above pushed session into POSTRX (waiting for POST
          * operation) jump to POSTRX logic, else break.
          */
         if(sess->ws_state != WI_HEADER)
            goto another_state;
         break;

      case WI_POSTRX:
      /* ++ REE/EDC */
      /* Webio does not work with a push bigger than the rx buffer size.
         Here we address this issue by calling an enulated file function to
         deal with the data. This is to prevent the allocation of resources
         for a large file */
      {
         wi_file *   filst = sess->ws_filelist;
         EOFILE * eofile = (EOFILE*)filst->wf_fd;
         _Bool  eo_file_read = false;
         /* If there is space in the receive buffer */
         if (sizeof(sess->ws_rxbuf) - (size_t)(sess->ws_rxsize))
         {
             /* See if there is more to read */
             error = recv(sess->ws_socket,
                          sess->ws_rxbuf + sess->ws_rxsize,
                          sizeof(sess->ws_rxbuf) - (size_t)(sess->ws_rxsize),
                          0);
         }
         /* If there is an emulated file function */
         else if ((filst) && (eofile->eo_function))
         {
            /* Let the emulated file function deal with the rest of the data */
            error = 0;
            eo_file_read = true;
         }

         if(error < 0)
         {
            if(errno == EWOULDBLOCK)
               error = 0;
            else
            {
               error = errno;
               /* ++ REE/EDC */
               TRACE(("sock recv error %d\n", error ));
               /* -- REE/EDC */
               cleanup( &sel_recv, &sel_recv);
               return WIE_SOCKET;
            }
         }
         sess->ws_rxsize += error;
         /* ++ REE/EDC */
         sess->ws_last = (wi_sec)(cticks());
         /* -- REE/EDC */

         /* If we have all the content, parse the name/value pairs.
          * We check for ContentLength field or socket closed
          */
         data = sess->ws_data;
         if(data)
         {
            int   contentRx;
            if (eo_file_read)
            {
               /* Pretend all is OK so we can call the handling function */
               contentRx = sess->ws_contentLength;
            }
            else
            {
               contentRx = sess->ws_rxsize - (data - sess->ws_rxbuf);
            }
            if((contentRx >= sess->ws_contentLength) ||
               (error == ENOTCONN))
            {
               error = wi_buildform(sess, data);
               if(error)
               {
                  wi_senderr(sess, 400);  /* Bad request */
                  break;
               }
               sess->ws_state = WI_CONTENT;
               /* ++ REE/EDC */
               sess->ws_last = (wi_sec)(cticks());
               /* -- REE/EDC */
            }
         }
         if(sess->ws_state != WI_POSTRX)
            goto another_state;

         break;
      }
      /* -- REE/EDC */

      case WI_CONTENT:
         error = wi_readfile(sess);

         if(FreeRTOS_FD_ISSET(sess->ws_socket, sel_recv))
         {
             FreeRTOS_FD_CLR(sess->ws_socket, sel_recv, eSELECT_READ);
         }

         if(error)
         {
            sess->ws_state = WI_ENDING;
         }
         sessions++;
         if(sess->ws_state != WI_CONTENT)
            goto another_state;

         break;

      case WI_SENDDATA:
         /* ++ REE/EDC */
         if(sess->ws_txbufs || (sess->ws_flags & WF_BINARY))
         {
            if(FreeRTOS_FD_ISSET(sess->ws_socket, sel_recv))
            {
                error = wi_sockwrite(sess);

                if(error)
                {
                   sess->ws_state = WI_ENDING;
                }

                FreeRTOS_FD_CLR(sess->ws_socket, sel_send, eSELECT_WRITE);
            }
            sess->ws_last = (wi_sec)(cticks());
            sessions++;
         }
         if(sess->ws_state != WI_SENDDATA)
            goto another_state;
         break;
      case WI_ENDING:
         /* Don't delete session and break, else we'll get a fault
          * in the sess->ws_last test below.
          */

         wi_delsess(sess);
         sess = next_sess;
         continue;
      default:
         /* Ignore unhandled messages */
         break;
      }
      /* kill sessions with no recent activity */
      if((u_long)(sess->ws_last + (150 * TPS)) < cticks())
       {
         wi_delsess(sess);
      }

      sess = next_sess;
   }

   cleanup( &sel_recv, &sel_recv);
   return sessions;
}

#ifdef WI_THREAD

/* wi_thread() - entry point for driving webio froma single thread.  It
 * is essentially an infinite loop while drives wi_poll.
 *
 * This should never return unless the web server is shut down.
 *
 * Returns: 0 if normal shutdown, else negative error code.
 */

int   wi_thread()
{
   int   sessions = 0;

   while(wi_running)
   {
      sessions = wi_poll();
      if( sessions < 0 )
      {
         dtrap();    /* restart the server thread?? */
      }

      vTaskDelay(1UL);
   }

   return sessions;
}

#endif  /* WI_THREAD */

uint8_t spacer[128] = "";

int
wi_sockaccept()
{
   struct freertos_sockaddr sa;
   socktype    newsock = (socktype)(-1);
   wi_sess *   newsess;
   socklen_t   sasize;
   int         error = 0;

   sasize = sizeof(struct freertos_sockaddr);
   newsock = FreeRTOS_accept(wi_listen, &sa, &sasize);

   /* ++ REE/EDC */
   if((int32_t)newsock >= 0)
   {
   /* -- REE/EDC */
      if(sasize != sizeof(struct freertos_sockaddr))
      {
         dtrap();
         return WIE_SOCKET;
      }

#if 0

      /* If the localhost-only flag is set, reject all other hosts */
      if(wi_localhost)
      {
         /* see if remote host is 127.0.0.1 or other version of self */
         if(FreeRTOS_htonl(sa.sin_addr) != 0x7F000001)
         {
            struct freertos_sockaddr local;
            socklen_t slen;

            /* wasn't loopback, check for local IP.
             * First get socket's local IP
             */
            memset(&local, 0, sizeof(local));
            slen = sizeof(local);


            /*
            if( getsockname(newsock, (struct sockaddr *)&local, &slen) < 0)
            {
                closesocket(newsock);
                return -1;
            }
            */
            if(sa.sin_addr != local.sin_addr)
            {
               closesocket(newsock);
               return 0;   /* not an error */
            }
         }
      }
      /* Set every socket to non-blocking. */
      {
         unsigned long  ulMode = 1;
         error = ioctlsocket(newsock, (long)FIONBIO, &ulMode);
      }

#endif
      if(error)
      {
         dtrap();
         /* panic("blocking socket"); */
         closesocket(newsock);
         return WIE_SOCKET;
      }
      /* -- REE/EDC */
      /* now that we have a new socket connection, make a session
       * object for it
       */
      newsess = wi_newsess();
      if(!newsess)
         return WIE_MEMORY;

      newsess->ws_socket = newsock;
      newsess->ws_client_ip = sa;
      return 0;
   /* ++ REE/EDC */
   }
   /* -- REE/EDC */
   return WIE_SOCKET;
}

/* ++ REE/EDC */
/* wi_set_language()
 *
 * Takes a look at the languages in the request and chooses a supported
 * one.
 *
 * Returns: none
 */

void wi_set_language( wi_sess * sess )
{
   static const struct language
   {
       const char * const language_code;
       wilang             ws_language;
       char * const ws_html_folder;
   }
   languages[] =
   {
        {
         "en",    WI_DEFAULT, NULL,
        },
        {
            "en-gb", WI_DEFAULT, NULL,
        },
        {
            "en-us", WI_DEFAULT, NULL,
        },
        {
            "en-au", WI_DEFAULT, NULL,
        },
        {
            "en-ca", WI_DEFAULT, NULL,
        },
        {
            "en-nz", WI_DEFAULT, NULL,
        },
        {
            "en-ie", WI_DEFAULT, NULL,
        },
        {
            "en-za", WI_DEFAULT, NULL,
        },
        {
            "en-jm", WI_DEFAULT, NULL,
        },
        {
            "en-bz", WI_DEFAULT, NULL,
        },
        {
            "en-tt", WI_DEFAULT, NULL,
        },
        {
            "ja", WI_DEFAULT, "ja\\"
        }
           /* TODO: Add any other supported languages in here */
   };
const char * const pszLangKeyword = "Accept-Language:";
/* look for the language keyword in the header */
char *   search = strstr(sess->ws_rxbuf, pszLangKeyword);
    if(search)
   {
      char  languagelist[WI_LANG_BUFFER];
      int   index = 0;
      search += strlen(pszLangKeyword);
      /* Copy the requested languages into the language list */
      while(index < (WI_LANG_BUFFER - 1))
      {
         if(search[index] == '\r')
         {
            languagelist[index] = 0;
            break;
         }
         else
         {
            languagelist[index] = search[index];
         }
         index++;
      }
      /* Terminate the list */
      languagelist[WI_LANG_BUFFER - 1] = '\0';
      index = sizeof(languages) / sizeof(struct language);
      /* Check the table of supported languages for a match */
      while(index--)
      {
         if(strstri(languagelist, languages[index].language_code))
         {
            /* Set the selected language */
            sess->ws_language = languages[index].ws_language;
            sess->ws_html_folder = languages[index].ws_html_folder;
            break;
         }
      }
   }
   else
   {
      /* Use the default language */
      sess->ws_language = WI_DEFAULT;
      sess->ws_html_folder = NULL;
   }
}
/* -- REE/EDC */



/* wi_parseheader()
 *
 * Make a best effort to process input. This is most often an http
 * header command (e.g. "GET foo.html"). This may be called with
 * only a partial header in sess->ws_rxbuf, or when the socket
 * is already in use for a previous command on a persistent
 * Connection.
 *
 * Returns: 0 if no error (incomplete header is not an error)
 * else negative error code.
 */

int
wi_parseheader( wi_sess * sess )
{
   char *   cp;
   char *   cl;
   char *   rxend;
   char *   pairs;
   u_long   cmd;
   int      error;

   /* First find end of HTTP header */
   /* ++ REE/EDC */
   rxend = strstr(sess->ws_rxbuf, "\r\n\r\n\0" );
   /* \0 is required as a string terminator or an additional 0x3A is inserted.
    * Suspect this is GNU compiler feature due to preceding char being high-ascii,
    * which /sometimes/ makes strcpy fail.
    */
   /* -- REE/EDC */

   if(!rxend)
      return 0;   /* no header yet - wait some more */
   /* ++ REE/EDC */
   /* check the request and set the language */
   wi_set_language(sess);
   /* -- REE/EDC */
   sess->ws_data = rxend + 4;

   /* extract the basic http comand */
   cmd = (u_long)(sess->ws_rxbuf[0]);
   cmd <<= 8;
   cmd |= (u_long)(sess->ws_rxbuf[1]);
   cmd <<= 8;
   cmd |= (u_long)(sess->ws_rxbuf[2]);
   cmd <<= 8;
   cmd |= (u_long)(sess->ws_rxbuf[3]);

   switch(cmd)
   {
   case H_GET:
   case H_POST:
      sess->ws_cmd = (httpcmds)(cmd);
      break;
   case H_PUT:
   default:

      if(cmd == H_PUT)
      {
          /* Deal with PUT operations in another path */
          return ( wi_putfile(sess) );
      }
      else
      {
          dtrap();

          /* unsupported command - send eror and clean up */
          wi_senderr(sess, 501);
          sess->ws_flags &= ~WF_READINGCMDS;
          sess->ws_state = WI_ENDING;
          return -1;
      }
   }


   /* Fall to here for GET or POST. Extract the URL */
   cp = wi_nextarg(&sess->ws_rxbuf[3]);
   if(!cp)
   {
      wi_senderr(sess, 400);  /* Bad request */
      return WIE_CLIENT;
   }
   if(*cp == '/')
   {
      if(*(cp+1) == ' ')
         sess->ws_uri = wi_rootfile;
      else
         sess->ws_uri = cp+1;    /* strip leading slash */
   }
   else
      sess->ws_uri = cp;

   /* Extract other useful fields from header  */
   sess->ws_auth = wi_getline("Authorization:", cp);
   sess->ws_referer = wi_getline("Referer:", cp);
   sess->ws_host = wi_getline("Host:", cp);

   cl = wi_getline("Content-Length:", cp);
   if(cl)
      sess->ws_contentLength = atoi(cl);
   else
      sess->ws_contentLength = 0;  /* unset */

   /* Check for name/value pairs and build form if found */
   if(cmd == H_GET)
   {
      pairs = strchr(cp, '?');
      if(pairs)
      {
         *pairs++ = 0;     /* Null terminate URI field */
         error = wi_buildform(sess, pairs);
         if(error)
         {
            wi_senderr(sess, 400);  /* Bad request */
            return WIE_CLIENT;
         }
      }
   }
   else if(cmd != H_GET) /* POST command */
   {
      if(cmd != H_POST)
      {
         wi_senderr(sess, 400);  /* Bad request */
         return WIE_CLIENT;
      }
      /* fall to header parse logic, get name/values from body later */
   }

   /* insert the null terminators in any strings in the rxbuf */
   if((sess->ws_uri > sess->ws_rxbuf) && (sess->ws_uri < rxend))
      wi_argterm(sess->ws_uri);      /* Null terminate the URI */
   if((sess->ws_auth > sess->ws_rxbuf) && (sess->ws_auth < rxend))
      wi_argterm(sess->ws_auth);     /* etc */
   if((sess->ws_referer > sess->ws_rxbuf) && (sess->ws_referer < rxend))
      wi_argterm(sess->ws_referer);
   if((sess->ws_uri > sess->ws_host) && (sess->ws_host < rxend))
      wi_argterm(sess->ws_host);

   /* ++ REE/EDC */
   /* Use sess->ws_uri pointing to modifiable RAM, not R/O Flash */
   if(g_ws_uri == NULL)
   {
       g_ws_uri = WI_MALLOC(strlen(sess->ws_uri)+1);
       sprintf(g_ws_uri,sess->ws_uri);
       sess->ws_uri = g_ws_uri;
   }

   if((sess->ws_language)
   /* Only html files currently have multi-lingual support */
   &&(strstri(sess->ws_uri, ".html")))
   {
      char uri[WI_MAXURLSIZE];
      /* The html files are expected to only be in the root folder
         of the website. Append the language folder name for the
         supported language */
      strcpy(uri, sess->ws_html_folder);
      strcat(uri, sess->ws_uri);
      error = wi_fopen(sess, uri, "r");
   }
   else
   {

       error = wi_fopen(sess, sess->ws_uri, "r");
   }
   /* -- REE/EDC */
   if(error)
   {
      wi_senderr(sess, 404);  /* File not found */
      return error;
   }

   if((sess->ws_filelist == NULL) ||
      (sess->ws_filelist->wf_routines == NULL) ||
      (sess->ws_filelist->wf_fd == NULL))
   {
      dtrap();
      return WIE_BADFILE;
   }

   if(sess->ws_filelist->wf_routines->wfs_fauth)
   {
      int      admit;  /* 1 if OK, 0 if authentication fails */

      if(sess->ws_auth == NULL)  /* No auth info in http header */
      {
         admit = sess->ws_filelist->wf_routines->wfs_fauth(
            sess->ws_filelist->wf_fd, "", "", sess);
      }
      else     /* Have auth info, parse it and check */
      {
         char name[32];
         char pass[32];

         wi_decode_auth(sess, name, sizeof(name), pass, sizeof(pass));

         admit = sess->ws_filelist->wf_routines->wfs_fauth(
            sess->ws_filelist->wf_fd, name, pass, sess);
      }

      if(!admit)
      {
         wi_senderr(sess, 401);  /* Send "Auth required" reply  */
         wi_fclose(sess->ws_filelist);
         return WIE_PERMIT;
      }
   }


   /* Try to figure out if file may contain SSI or other content
    * requiring server parsing. If not, mark it as binary. This
    * will allow faster sending of images and other large binaries.
    */
   wi_setftype(sess);

   sess->ws_flags &= ~WF_HEADERSENT;   /* header not sent yet */

   if(cmd == H_GET)
   {
      /* start loading file to return. */
      sess->ws_state = WI_CONTENT;
      error = wi_readfile(sess);
      return error;
   }
   else  /* POST, wait for data */
   {
      sess->ws_state = WI_POSTRX;
      return 0;
   }
}

/* wi_badform()
 *
 * Called when a cgi function returns error text. send the text to the
 * client and close the session.
 */

static const char * const badformhead = "<html><head><title>Form Error</title> \
 <link href=\"praemio-style-main.css\" rel=\"stylesheet\" type=\"text/css\"></head> \
 <body onLoad=\"javascript:{ if(parent.frames[0]&&parent.frames['navig'].Go) parent.frames['navig'].Go()}\" > \
 <center><br><br><br><h2>";

static const char * const badformtail = "</h2></body></html>";

void wi_badform(wi_sess * sess, char * errmsg)
{
   wi_printf(sess, badformhead );
   wi_printf(sess, "Error in form: %s <br>", errmsg);
   wi_printf(sess, badformtail );
   wi_sockwrite(sess);

   return;
}


/* wi_readfile()
 *
 * Read file from disk or script into txbufs. Allocate txbufs as we go
 * sess->ws_fd should have an open fd.
 *
 * BE VERY CAREFULL if you decide to edit this logic. The core loop is
 * rather convoluted since it's handling file reads & closes,
 * chained txbufs, an exception for fast binary transfer, and calls to
 * wi_ssi(). SSI are especially tricky since wi_ssi() will recursively
 * call back to wi_readfile()
 *
 * Returns: 0 if no error, else negative WIE_ error code.
 *
 */

int
wi_readfile(struct wi_sess_s * sess)
{
   int         error = 0;
   int         len;
   int         toread;
   wi_file *   filst;     /* info about current file */

   /* start loading file to return. */
   filst = sess->ws_filelist;
/* ++ REE/EDC */
#if USE_EMFILES
   /* Check for embedded form handlers */
   if(filst->wf_routines == &emfs)
   {
      EOFILE * eofile = (EOFILE*)filst->wf_fd;

      /* Check for a CGI handling function */
      if (eofile->eo_function)
      {
         error = eofile->eo_function(sess, eofile);

         if(error)
         {
            wi_badform(sess, sess->ws_form_error);
            return WIE_BADPARM;
         }
         if(sess->ws_filelist == NULL)    // done with request
            return 0;
         else
             filst = sess->ws_filelist;    // re-set local variable
      }
   }
#endif
/* -- REE/EDC */
readmore:
   toread = (int)(sizeof(filst->wf_data)) - filst->wf_inbuf;
   /* ++ REE/EDC */
   len = wi_fread( &filst->wf_data[filst->wf_inbuf], 1, (unsigned int)toread, filst );
   /* -- REE/EDC */
   if(len <= 0)
   {
      wi_fclose(filst);

      /* See if there is another input file "outside" the current one.
       * This happens if the file we just closed was an SSI
       */
      if(sess->ws_filelist)
         return 0;
      else
         goto readdone;
   }
   /* ++ REE/EDC */
   sess->ws_last = (wi_sec)(cticks());
   /* -- REE/EDC */
   filst->wf_inbuf += len;

   /* fast path for binary files. We've read first buffer from file
    * now - just jump to the sending code.
    */
   if(sess->ws_flags & WF_BINARY)
      goto readdone;

   /* Copy the file into a send buffer while searching for SSI strings */
   for(len = filst->wf_nextbuf; len < filst->wf_inbuf; len++)
   {
      if((filst->wf_data[len + 4] == '#') &&
         (filst->wf_data[len + 1] == '!'))
      {
         char * ssi_end;

         /* got complete SSI string? */
         ssi_end = strstr( &filst->wf_data[len], "-->");

         if(ssi_end)
         {
            int ssi_len = (ssi_end - &filst->wf_data[len]) + 3;

            filst->wf_nextbuf = len;      /* Set address of SSI text */

            if(strncmp( &filst->wf_data[len], "<!--#include", 12) == 0)
            {
               /* Call routine to process SSI string in file */
               error = wi_ssi(sess);
            }
            else if(strncmp( &filst->wf_data[len], "<!--#exec ", 10) == 0)
            {
               /* Call routine to process SSI string in file */
               error = wi_exec(sess);
            }

            /* Save location where SSI ends */
            len += ssi_len;

            /* break if SSI changed the current file. */
            if(sess->ws_filelist != filst)
            {
               filst->wf_nextbuf = len;
               return 0;
            }
            filst->wf_nextbuf = 0;  /* force jump to readmore */
         }
         else  /* end not found - SSI text may end in next block */
         {
            dtrap();
         }
      }

      /* Make sure we have space for char in txbuf */
      if((sess->ws_txbufs == NULL) ||
         (sess->ws_txtail->tb_total >= WI_TXBUFSIZE))
      {
         if(wi_txalloc(sess) == NULL)
         {
            wsBreakPoint();
            return WIE_MEMORY;
         }
      }
      sess->ws_txtail->tb_data[sess->ws_txtail->tb_total++] = filst->wf_data[len];
   }

   /* See if we need to do more reading */
   if((filst->wf_nextbuf == 0) && (len > 0))
   {
      filst->wf_inbuf = 0;    /* no unread data in read buffer */
      goto readmore;
   }

readdone:

   /* Done with loading data, begin send process */
   sess->ws_state = WI_SENDDATA;
   error = wi_sockwrite(sess);

   return error;
}

/* wi_socketwrite()
 *
 * This is called when a session has read all the data to send
 * from files/scripts,and is ready to send it to socket.
 *
 * Returns: 0 if no error, else negative WIE_ error code.
 */

int
wi_sockwrite(wi_sess * sess)
{
   txbuf *  txbuf_local;
   int      error;
   int      tosend;
   int      contentlen = 0;
   int      ctlof = 0;


   if(sess->ws_flags & WF_BINARY)
   {
      error = wi_movebinary(sess, sess->ws_filelist);
      return error;
   }

   if((sess->ws_flags & WF_HEADERSENT) == 0)   /* header sent yet? */
   {
      /* Build and prepend OK header - first calculate length. */
      for(txbuf_local = sess->ws_txbufs; txbuf_local; txbuf_local = txbuf_local->tb_next)
         contentlen += txbuf_local->tb_total;

      error = wi_replyhdr(sess, contentlen);
      if(error)
         return WIE_SOCKET;
   }

   while(sess->ws_txbufs)
   {
       txbuf_local = sess->ws_txbufs;
      tosend = txbuf_local->tb_total - txbuf_local->tb_done;


      error = send(sess->ws_socket, &txbuf_local->tb_data[txbuf_local->tb_done], (size_t)tosend, 0);

      ctlof += error;


      if(error != tosend)
      {
         error = errno;
         if(error == EWOULDBLOCK)
         {
             txbuf_local->tb_done = 0;
            return 0;
         }
         dtrap();
         return WIE_SOCKET;
      }
      /* Fall to here if we sent the whole txbuf. Unlink & free it */
      sess->ws_txbufs = txbuf_local->tb_next;
      txbuf_local->tb_next = NULL;
      wi_txfree(txbuf_local);

      /* ++ REE/EDC */
      sess->ws_last = (wi_sec)(cticks());
      /* -- REE/EDC */
   }

   /* fall to here when all txbufs are sent. */
   error = wi_txdone(sess);

   return error;
}

/* wi_redirect()
 *
 * This is used by forms processing routines to return the passed file in
 * file in reply to a GET or POST request.
 *
 * Returns: 0 if no error, else negative WIE_ error code.
 *
 */

int
wi_redirect(wi_sess * sess, char * filename)
{
   int      error;
   char *   pairs;
   struct wi_file_s *ws_filelist;

   sess->ws_referer = sess->ws_uri;
   sess->ws_uri = filename;
   /* ++ REE/EDC */
   sess->ws_last = (wi_sec)(cticks());
   /* -- REE/EDC */

   /* unlink the completed form file from the session */
   ws_filelist = sess->ws_filelist->wf_next;
   wi_fclose(sess->ws_filelist);
   sess->ws_filelist = ws_filelist;

   /* parse any name/value pairs appended to file name */
   pairs = strchr(filename, '?');
   if(pairs)
   {
      *pairs++ = 0;     /* Null terminate URI field */
      error = wi_buildform(sess, pairs);
      if(error)
      {
         dtrap();       /* bad html from caller? */
         return error;
      }
   }

   /* Find and open new file to return, */
   error = wi_fopen(sess, filename, "rb");
   if(error)
   {
      wi_senderr(sess, 404);  /* File not found */
      return error;
   }

   sess->ws_state = WI_CONTENT;
   sess->ws_cmd = H_GET;
   sess->ws_last = (wi_sec)(cticks());
   sess->ws_flags &= ~WF_HEADERSENT;

   return 0;   /* OK return */
}



/* wi_putfile()
 *
 * This is called when a session receives a PUT command.
 *
 *
 * Returns: 0 if no error, else negative WIE_ error code.
 *
 */

int
wi_putfile( wi_sess * sess)
{

   dtrap();
   USE_ARG(sess);

   return 0;      /* No Error */
}

