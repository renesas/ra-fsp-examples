/* webfs.c
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

#include "board_cfg.h"
#include "common_utils.h"
#include "portable.h"


#include "websys.h"     /* port dependant system files */
#include "webio.h"
#include "webfs.h"

/* ++ REE/EDC */
//#include "liveFile.h"
#include "webSSI.h"
#include "webCGI.h"
/* -- REE/EDC */


/* This file contins webio file access routines. The external "wi_f" entry 
 * points have the same semantics as C buffered file IO (fopen, etc). These 
 * determine which of the actual file systems should be called and make 
 * the call.
 */

#ifdef WI_STDFILES
/* Voiding these pointers is ugly, but so are the Linux declarations. */
wi_filesys sysfs = {
   (void*)fopen,
   (void*)fread,
   (void*)fwrite,
   (void*)fclose,
   (void*)fseek,
   (void*)ftell,
   NULL,
   NULL
};
#endif   /* WI_STDFILES */

#ifdef WI_EMBFILES
wi_filesys emfs = {
   em_fopen,
   em_fread,
   em_fwrite,
   em_fclose,
   em_fseek,
   em_ftell,
   NULL,
   NULL
};
#endif   /* WI_EMBFILES */

/* Table of the supported file systems */
wi_filesys * wi_filesystems[] =    /* list of file systems */
{
#ifdef WI_EMBFILES
   &emfs,
#endif
#ifdef WI_STDFILES
   &sysfs,
#endif
   NULL     /* reserved for runtime entry */
};
/* ++ REE/EDC */
/* Function to set the eo_authenticate member variable to indicate if
   the file requires authorisation */
static void
em_check_authentication(void    *pvEfs,
                        PEOFILE pEoFile,
                        char    *name);
/* Pointer to an Embedded File System that can be loaded at run-time */
void *wi_pvEfs = NULL;
/* -- REE/EDC */

wi_file *      wi_allfiles;   /* list of all open files */


/* wi_fopen()
 * 
 * webio top level file open routine. This is just wrapper for the lower
 * level routine - either the Embedded FS, and the host system's native FS.
 *
 * Returns: 0 if OK else negative WIE_ error code.
 * 
 */

int
wi_fopen(wi_sess * sess, char * name, char * mode)
{
   wi_filesys *   fsys;    /* File system which has the file */
   wi_file *      newfile; /* transient file structure */
   void *         fd;      /* descriptor from fs */
   unsigned int   i;

   /* Loop through the FS list, trying an open on each */
   for(i = 0; i < sizeof(wi_filesystems)/sizeof(wi_filesys*); i++)
   {
      fsys = wi_filesystems[i];
      if(fsys == NULL)
         continue;
      fd = fsys->wfs_fopen(name, mode);
      if(fd)
      {
         /* Got an open - create a wi_file & fill it in. */
         newfile = wi_newfile(fsys, sess, fd);
         if(!newfile)
         {
            fsys->wfs_fclose(fd);
            return WIE_MEMORY;
         }
         return 0;
      }
   }
   return WIE_NOFILE;
}


int
wi_fread(char * buf, unsigned size1, unsigned size2, void * filep)
{
   int   bytes;
   WI_FILE * fd;
   fd = (WI_FILE *)filep;
   bytes = fd->wf_routines->wfs_fread(buf, size1, size2, fd->wf_fd);
   return bytes;
}

int
wi_fwrite(char * buf, unsigned size1, unsigned size2, void * filep)
{
   int   bytes;
   WI_FILE * fd;
   fd = (WI_FILE *)filep;
   bytes = fd->wf_routines->wfs_fwrite(buf, size1, size2, fd->wf_fd);
   return bytes;
}


int
wi_fclose(WI_FILE * fd)
{
   int   error;

   /* close file at lower level, get an error code */
   error = fd->wf_routines->wfs_fclose(fd->wf_fd);

   /* Delete our intermediate layer struct for this file. */
   wi_delfile(fd);

   return error;    /* return error from lower layer delete */
}


int
wi_fseek(WI_FILE * fd, long offset, int mode)
{
   return(fd->wf_routines->wfs_fseek(fd->wf_fd, offset, mode));
}


int
wi_ftell(WI_FILE * fd)
{
   return(fd->wf_routines->wfs_ftell(fd->wf_fd));
}

/***************** Optional embedded FS starts here *****************/
#ifdef USE_EMFILES
/* ++ REE/EDC */
/* Include the master list of encapsulated file systems generated with the
   EmbedFS utility */
#include "efsWebSites.h"
/* -- REE/EDC */

/* transient list of em_ files which are currently open */
EOFILE * em_openlist;

/* em_verify()
 * 
 * Make sure a passed fd is really an EOFILE.
 * 
 * Returns 0 if it is, or WIE_BADFILE if not.
 */
int
em_verify(EOFILE * fd)
{
   EOFILE *    eofile;

   /* verify file pointer is valid */
   for(eofile = em_openlist; eofile;eofile = eofile->eo_next)
   {
      if(eofile == fd)
         break;
   }
   if(!eofile)
      return WIE_BADFILE;

   return 0;
}

/* em_lookupsess()
 * 
 * Lookup web session based on an emf fd 
 *
 * returns session, or NULL if not found.
 */

wi_sess * 
em_lookupsess(void * fd)
{
   wi_sess *   sess;

   for(sess = wi_sessions; sess; sess = sess->ws_next)
      if(sess->ws_filelist->wf_fd == fd)
         return sess;

   return NULL;
}

WI_FILE *
em_fopen(char * name, char * mode)
{
   /* ++ REE/EDC - replaced John Bartas's compiled file system with a number
      of encapsulated file systems contained in the gEFSL data struct */
   size_t  st_number = gEFSL.stNumberOfElements;
   EFSERR  efs_error = EFS_FILE_NOT_FOUND;
   void    *pvEfs = NULL;
   /* The encapsulated file system file information structure */
   EFS     eo_file;
   EOFILE *eofile;
   void   *eo_function = NULL;
   /* All files are RO,otherwise return NULL */
   if( *mode != 'r' )
      return NULL;
   /* First check the externally loaded website */
   if (wi_pvEfs)
   {
       efs_error = efsFindFile(wi_pvEfs, (int8_t *)(name), &eo_file);
       if (EFS_OK == efs_error)
       {
           pvEfs = wi_pvEfs;
       }
   }
   /* For each of the embedded file systems */
   while ((efs_error) && (st_number--))
   {
      /* Look to see if the file exists */
      efs_error = efsFindFile(gEFSL.ppvEfs[st_number],
                              (int8_t *)(name),
                              &eo_file);
      if(efs_error == EFS_OK)
      {
         break;
      }
#ifdef _DEBUG_
      else if(efs_error < EFS_DIRECTORY_NOT_FOUND)
      {
         static const struct _ERRSTR
         {
            const EFSERR  efs_error;
            const   char * const err_string;
         }
         err[] =
         {
            EFS_BINARY_NOT_FOUND, "EFS_BINARY_NOT_FOUND",
            EFS_BINARY_ENDIAN_ERROR, "EFS_BINARY_ENDIAN_ERROR",
            EFS_BINARY_ALIGNMENT_ERROR, "EFS_BINARY_ALIGNMENT_ERROR"
         };
         int i = sizeof(err) / sizeof(struct _ERRSTR);
         TRACE(("em_fopen: **Error: Open failed with error %d ", efs_error));
         /* see if there is an error string for the code */
         while(i--)
         {
            if(err[i].efs_error == efs_error)
            {
               TRACE(("%s", err[i].err_string));
            }
         }
         TRACE(("\r\n"));
      }
#endif
   }
   /* If an encapsulated file was not found - check for a live file */
   if(efs_error)
   {
       /* stubbed out */
//       efs_error = liveFindFile(name, &eo_file);
   }

#if 1 // RC2020
   /* If an encapsulated file or live file have not been found */
   if(efs_error)
   {
      /* Try looking for an SSI files */
      eo_function = ssiGetFunction((int8_t *)(name));
      /* An SSI file was not found, try CGI files */
      if (!eo_function)
      {
         eo_function = cgiGetFunction((int8_t *)(name));
         if (!eo_function)
            return NULL;
      }
      /* A CGI file was not found, fail open */
      if (!eo_function)
         return NULL;
   }
   /* We're going to open file. Allocate the transient control structure */
   eofile = (EOFILE *)wi_alloc(sizeof(EOFILE));
   WI_TRACE_ALLOC(eofile);
   if(!eofile)
      return NULL;
   /* Either a data file was found or a function to handle the file */
   if (eo_function)
   {
       /* Set the function pointer to the handling function */
       eofile->eo_function = eo_function;
       memset(&eofile->eo_file, 0, sizeof(EFS));
       eofile->eo_authenticate = 0;
   }
   else
   {
       /* An encapsulate file was found */
       eofile->eo_file = eo_file;
       eofile->eo_function = NULL;
       if (pvEfs)
       {
           em_check_authentication(pvEfs, eofile, name);
       }
       else if (st_number < gEFSL.stNumberOfElements)
       {
           em_check_authentication(gEFSL.ppvEfs[st_number],
                                   eofile,
                                   name);
       }
   }

#endif
   /* Set the file position index */
   /* -- REE/EDC */
   eofile->eo_position = 0;

   /* Add new open struct to open files list */
   eofile->eo_next = em_openlist;
   em_openlist = eofile;

   return ( (WI_FILE*)eofile);
}

int
em_fread(char * buf, unsigned size1, unsigned size2, void * fd)
{
   /* ++ REE/EDC - replaced John Bartas's compiled file system with a number
      of encapsulated file systems contained in the gEFSL data struct */
   size_t      datalen;    /* length of data to move */
   EOFILE *    eofile;
   PEFS        peo_file;
   int         error;

   eofile = (EOFILE *)fd;
   error = em_verify(eofile);
   if(error)
      return error;

   peo_file = &eofile->eo_file;

   /* TODO: Server push */
   datalen = size1 * size2;
   if(datalen > (peo_file->ulFileLength - eofile->eo_position))
      datalen =  peo_file->ulFileLength - eofile->eo_position;

   /* Check for position at End of File - EOF */
   if(datalen == 0)
      return 0;

   memcpy(buf, &peo_file->pbyFileData[eofile->eo_position], datalen);
   eofile->eo_position += datalen;

   return (int)datalen;
   /* -- REE/EDC */
}


int
em_fwrite(char * buf, unsigned size1, unsigned size2, void * fd)
{
   int      error;

   dtrap();

   USE_ARG(buf);
   USE_ARG(size1);
   USE_ARG(size2);
   error = em_verify((EOFILE*)fd);
   if(error)
      return error;
   return 0;
}

int
em_fclose(void * voidfd)
{
   EOFILE *    passedfd;
   EOFILE *    tmpfd;
   EOFILE *    last;

   passedfd = (EOFILE *)voidfd;

   /* verify file pointer is valid */
   last = NULL;
   for(tmpfd = em_openlist; tmpfd; tmpfd = tmpfd->eo_next)
   {
      if(tmpfd == passedfd)  /* If we found it, unlink */
      {
         if(last)
            last->eo_next = passedfd->eo_next;
         else
            em_openlist = passedfd->eo_next;
         break;
      }
      last = tmpfd;
   }

   if(tmpfd == NULL)       /* fd not in list? */
      return WIE_BADFILE;
   /* ++ REE/EDC */
   if(passedfd->eo_file.bfDataAllocated)
   {
      free((void*)passedfd->eo_file.pbyFileData);
   }
   /* -- REE/EDC */
   wi_free(passedfd);
   WI_TRACE_FREE(passedfd);
   return 0;
}


int
em_fseek(void * fd, long offset, int mode)
{
   EOFILE *    emf;
   long    error;
   long    newpos;
   uint32_t    size;       /* Total size of em file */

   emf = (EOFILE *)fd;
   error = em_verify(emf);
   if (error)
      return error;

   /* Get file size into local variable */
   /* ++ REE/EDC */
   size = emf->eo_file.ulFileLength;
   /* -- REE/EDC */

   /* Figure out where new position should be */
   switch (mode)
   {
   case SEEK_SET:
      newpos = offset;
      break;
   case SEEK_END:
      newpos = (long)((long)(size) + offset);
      break;
   case SEEK_CUR:
      newpos = (long)((long)(emf->eo_position) + offset);
      break;
   default:
      panic("em_fseek");
      newpos = 0;
      break;
   }

   /* Sanity check new position */
   if ((newpos < 0) || (newpos > (long)(size)))
      return WIE_BADPARM;

   emf->eo_position = (u_long)(newpos);
   return 0;
}


int
em_ftell(void * fd)
{
   EOFILE *    emf;
   int         error;

   emf = (EOFILE *)fd;
   error = em_verify(emf);
   if(error)
      return -1;
   /* ++ REE/EDC */
   return (int)(emf->eo_position);
   /* -- REE/EDC */
}

int
em_push(void * fd, wi_sess * sess)
{
   int         error;
   EOFILE *    emf;
   /* ++ REE/EDC */

   emf = (EOFILE *)fd;

   error = em_verify(emf);
   if(error)
      return error;

   /* Check for a PUSH function */
   if (emf->eo_function)
   {
      error = emf->eo_function(sess, emf);
   }
   /* -- REE/EDC */

   return error;
}

/* ++ REE/EDC - a file placed in the root folder of the website
   (along with index.html) contains the names of the files that 
   require authentication */
static void
em_check_authentication(void    *pvEfs,
                        PEOFILE pEoFile,
                        char    *name)
{
   EFS     eo_file;
   pEoFile->eo_authenticate = 0;
   /* Look for the file htaccess.txt in the root of the encapsulated
      file system */
   if (!efsFindFile(pvEfs,
                    (int8_t *)("\\htaccess.txt"),
                    &eo_file))
   {
       /* If the file name is in this file... */
       if (strstr((char*)eo_file.pbyFileData,
                  name))
       {
          pEoFile->eo_authenticate = 1;
       }
   }
}
/* -- REE/EDC */

#endif  /* USE_EMFILES */

