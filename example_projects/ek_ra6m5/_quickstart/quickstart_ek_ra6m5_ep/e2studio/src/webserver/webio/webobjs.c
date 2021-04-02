/* webobjs.c
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
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "queue.h"

#include "websys.h"     /* port dependant system files */
#include "webio.h"
#include "webfs.h"

#include "common_utils.h"
#include "portable.h"

/* This file contins the code to manage webio objects, such as sessions,
 * tx buffers, and forms. It also contains heap "wrappers" with error
 * checking.
 */

wi_sess * wi_sessions;     /* Master list of sessions */

u_long   wi_blocks = 0;
u_long   wi_bytes = 0;
u_long   wi_maxbytes = 0;
u_long   wi_totalblocks = 0;


/* Webio's heap system allocates a bit more memory from the system
 * heap than the size passsed. The extra space contains the ascii for
 * pattern "MEMM". The front marker also has a length field so we
 * can find the back marker.
 */

int   wi_marker = 0x4D454D4D;    /* MEMM */

struct memmarker
{
   int   marker;
   int   msize;   /* size of this block */
};

char *
wi_alloc(int bufsize)
{
   char * buffer;
   struct memmarker * mark;
   int   totalsize;

   totalsize = bufsize + (int)sizeof(struct memmarker) + 4;
   /* ++ REE/EDC */
   buffer = WI_MALLOC((size_t)totalsize);
   memset(buffer, 0, (size_t)totalsize);
   /* -- REE/EDC */

   mark = (struct memmarker * )buffer;
   mark->marker = wi_marker;        /* Mark beginning of buffer */
   mark->msize = bufsize;
   buffer = (char*)(mark + 1);      /* get return value */
   *(int*)(buffer + bufsize) = wi_marker;    /* Mark end of buffer */

   wi_blocks++;
   wi_totalblocks++;
   wi_bytes += (u_long)bufsize;
   if(wi_bytes > wi_maxbytes)
      wi_maxbytes = wi_bytes;

   return buffer;
}

void
wi_free(void * buff)
{
   struct memmarker * mark;
   char * cp;

   /* first, find the lead marker and check for overwritting */
   mark = (struct memmarker *)buff;
   mark--;        /* marker is prepended to buffer */

   /* check for corruption of pre-buffer area */
   if(mark->marker != wi_marker)
      panic("wi_free: pre");
   
   /* check for corruption of post-buffer area */
   cp = (char*)buff;
   if( *(int*)(cp + mark->msize) != wi_marker)
      panic("wi_free: post");
   
   wi_blocks--;
   wi_bytes -= (u_long)(mark->msize);

   WI_FREE( (void*)mark );
}


/* txbuf constructor */

txbuf *
wi_txalloc(wi_sess * websess)
{
   txbuf * newtx;

   newtx = (txbuf*)wi_alloc( sizeof(txbuf) );
   WI_TRACE_ALLOC(newtx);
   if(!newtx)
      return NULL;

   /* Install new TX buffer at end of session chain */
   if(websess->ws_txtail)
      websess->ws_txtail->tb_next = newtx;   /* add to existing tail */

   websess->ws_txtail = newtx;      /* new buffer is new tail */

   /* If empty, also make it head */
   if(websess->ws_txbufs == NULL)
      websess->ws_txbufs = newtx;

   newtx->tb_session = websess;     /* backpointer to session */

   return newtx;
}

/* txbuf destructor */

void
wi_txfree(txbuf * oldtx)
{
   wi_sess *   websess;
   txbuf *     tmptx;
   txbuf *     last;

   /* This has most likely been unlinked already, but try to find it
    * in the sesion queue just in case.
    */
   websess = oldtx->tb_session;
   last = NULL;
   for(tmptx = websess->ws_txbufs; tmptx; tmptx = tmptx->tb_next)
   {
      if(tmptx == oldtx)
      {
         if(last)
            last->tb_next = oldtx->tb_next;
         else
            websess->ws_txbufs = oldtx->tb_next;

         break;
      }
   }

   wi_free(oldtx);
   WI_TRACE_FREE(oldtx);

   return;
}


/* wi_sess constructor */

wi_sess *
wi_newsess(void)
{
   wi_sess * newsess;

   newsess = (wi_sess *)wi_alloc( sizeof(wi_sess) );
   WI_TRACE_ALLOC(newsess);
   if(!newsess)
   {
      /* ++ REE/EDC */
      TRACE(("wi_newsess: out of memory.\n"));
      /* -- REE/EDC */
      return NULL;
   }
   newsess->ws_socket = (socktype)INVALID_SOCKET;
   newsess->ws_state = WI_HEADER;
   /* ++ REE/EDC */
   newsess->ws_last = (wi_sec)(cticks());
   /* -- REE/EDC */

   /* Add new session to master list */
   newsess->ws_next = wi_sessions;
   wi_sessions = newsess;

   /* All new sessions strt out ready to read their socket */
   newsess->ws_flags |= WF_READINGCMDS;

   return newsess;
}


/* wi_sess destructor */

void
wi_delsess(wi_sess * oldsess)
{
   wi_sess * tmpsess;
   wi_sess * lastsess;

   if(oldsess->ws_socket != (socktype)(INVALID_SOCKET))
   {
      closesocket(oldsess->ws_socket);
      oldsess->ws_socket = 0;
   }

   /* Unlink from master session list */
   lastsess = NULL;
   for(tmpsess = wi_sessions; tmpsess; tmpsess = tmpsess->ws_next)
   {
      if(tmpsess == oldsess)     /* Found session to unlink? */
      {
         if(lastsess)
            lastsess->ws_next = tmpsess->ws_next;
         else
            wi_sessions  = tmpsess->ws_next;
         break;
      }
      lastsess= tmpsess;
   }

   /* Make sure there are no dangling resources */
   if(oldsess->ws_txbufs)
   {
      while(oldsess->ws_txbufs)
      {
         wi_txfree(oldsess->ws_txbufs);
      }
   }
   if(oldsess->ws_filelist)
   {
      if(oldsess->ws_filelist)
      {
         wi_fclose( oldsess->ws_filelist);
      }
   }

  /* ++ REE/EDC */
   if(oldsess->ws_formlist)
   {
      while(oldsess->ws_formlist)
      {
         wi_form *next = oldsess->ws_formlist->next;
         wi_free(oldsess->ws_formlist);
         WI_TRACE_FREE(oldsess->ws_formlist);
         oldsess->ws_formlist = next;
      }
   }
  /* -- REE/EDC */

   wi_free(oldsess);    /* free the actual memory */
   WI_TRACE_FREE(oldsess);

   return;
}

/* wi_file constructor */

wi_file *
wi_newfile(wi_filesys * fsys, wi_sess * sess, void * fd)
{
   wi_file *      newfile;

   newfile = (wi_file *)wi_alloc( sizeof(wi_file));
   WI_TRACE_ALLOC(newfile);
   if(!newfile)
      return NULL;

   newfile->wf_fd = fd;
   newfile->wf_routines = fsys;
   newfile->wf_sess = sess;

   /* Put new file at front of session file list */
   newfile->wf_next = sess->ws_filelist;
   sess->ws_filelist = newfile;

   return newfile;
}


/* wi_file destructor */

int
wi_delfile(wi_file * delfile)
{
   wi_sess *   sess;
   wi_file *   tmpfi;
   wi_file *   last;

   /* unlink file from session list */
   sess = delfile->wf_sess;
   last = NULL;
   for(tmpfi = sess->ws_filelist; tmpfi; tmpfi = tmpfi->wf_next)
   {
      if(tmpfi == delfile)
      {
         if(last)
            last->wf_next = delfile->wf_next;
         else
            sess->ws_filelist = delfile->wf_next;
         break;
      }
      last = tmpfi;
   }

   wi_free(delfile);
   WI_TRACE_FREE(delfile);

   return 0;
}

