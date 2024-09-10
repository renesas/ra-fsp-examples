/* webfs.h
 *
 * Part of the Webio Open Source lightweight web server.
 *
 * Copyright (c) 2007 by John Bartas
 * Portions Copyright (C) 2010 Renesas Electronics Corporation.
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

#ifndef _WEBFS_H_
#define _WEBFS_H_    1
/* ++ REE/EDC */
#include "websys.h"
#include "efsFile.h"
#include "fmtout.h"
/* -- REE/EDC */

/* wi_file_s - wrapper for a lower layer FILE descriptor. One of these 
 * is maintained by the server for each open file.
 */
typedef struct wi_file_s
{
   struct wi_file_s *      wf_next;       /* list link */
   void *                  wf_fd;         /* lower layer descriptor */
   struct wi_filesys_s *   wf_routines;   /* routines to use */
   struct wi_sess_s *      wf_sess;       /* session for this file */
   /* ++ REE/EDC */
   char     wf_data[WI_FSBUFSIZE];    /* data buffer for reading file */
   /* -- REE/EDC */
   int      wf_inbuf;               /* number of bytes in wf_data */
   int      wf_nextbuf;             /* next byte to process in wf_data */
} wi_file;

extern   wi_file * wi_allfiles;
/* ++ REE/EDC */
extern   void    * wi_pvEfs;
/* -- REE/EDC */
#define  WI_FILE   wi_file

typedef struct wi_filesys_s
{
   WI_FILE *   (*wfs_fopen) (char * name, char * mode);
   int         (*wfs_fread) (char * buf, unsigned size1, unsigned size2, void * fd);
   int         (*wfs_fwrite)(char * buf, unsigned size1, unsigned size2, void * fd);
   int         (*wfs_fclose)(void * fd);
   int         (*wfs_fseek) (void * fd, long offset, int mode);
   int         (*wfs_ftell) (void * fd);
   int         (*wfs_fauth) (void * fd, char * name, char * pw, wi_sess * sess);  /* Optional, for authentication */
   int         (*wfs_push) (void * fd, wi_sess * sess);  /* Optional, server push */
} wi_filesys;


extern   wi_filesys *   wi_filesystems[];

extern   wi_file *      wi_files;   /* list of open files */

/* Top level file access routines. These just map to lower routines */
extern   int      wi_fopen(wi_sess * sess, char * name, char * mode);
extern   int      wi_fread(char * buf, unsigned size1, unsigned size2, void * fd);
extern   int      wi_fwrite(char * buf, unsigned size1, unsigned size2, void * fd);
extern   int      wi_fclose(WI_FILE * fd);
extern   int      wi_fseek(WI_FILE * fd, long offset, int mode);
extern   int      wi_ftell(WI_FILE * fd);

/* Misc. wi_file utility routines */
extern   wi_file *   wi_newfile(wi_filesys * fsys, wi_sess * sess, void * fd);
extern   int         wi_delfile(wi_file * delfile);
extern   int         wi_movebinary(wi_sess * sess, wi_file * fi);


/***************** Optional embedded FS starts here *****************/
#ifdef USE_EMFILES

/* ++ REE/EDC */
/* removed typedef struct em_file_s & master list of embedded files */
/* -- REE/EDC */

#define     EMF_SSI     0x0001         /* SSI code-generated file */
#define     EMF_FORM    0x0002         /* Form handler */
#define     EMF_HTML    0x0004         /* HTML file which may have SSIs */
#define     EMF_DATA    0x0008         /* Raw data file (no SSIs) */
#define     EMF_PUSH    0x0010         /* Server side push routine */
#define     EMF_AUTH    0x0020         /* Accessing file requires auth. */
#define     EMF_CEXP    0x0040         /* file just prints C expression. */

extern   int         em_push(void * fd, wi_sess * sess);
extern   wi_sess *   em_lookupsess(void * fd);
extern   WI_FILE *   em_fopen(char * name, char * mode);
extern   int         em_fread(char * buf, unsigned size1, unsigned size2, void * fd);
extern   int         em_fwrite(char * buf, unsigned size1, unsigned size2, void * fd);
extern   int         em_fclose(void * fd);
extern   int         em_fseek(void * fd, long offset, int mode);
extern   int         em_ftell(void * fd);

extern   wi_filesys emfs;

/* ++ REE/EDC */
/* Define pointers for clarity of more complex type declarations */
typedef wi_sess * PSESS;
typedef struct em_open_s *PEOFILE;
/* Define the type of a generic function to handle both SSI and CGI requests
   The extra parameters for the CGI request are contained in the session struct */
typedef int(*PSVRFN)(PSESS pSess, PEOFILE pEoFile);
typedef int(*const PCSVRFN)(PSESS pSess, PEOFILE pEoFile);
/* -- REE/EDC */

typedef struct em_open_s
{
   struct em_open_s * eo_next;
   /* ++ REE/EDC - removed em_files */
   EFS         eo_file;         /* file data retrieved by efsFindFile() */
   int         eo_authenticate; /* non zero when file requires authentication */
   PSVRFN      eo_function;     /* function pointer for SSI and CGI */
   /* -- REE/EDC */
   u_long      eo_position;   /* file position pointer */
   wi_sess *   eo_sess;       /* session (for pass to code) */
} EOFILE;

extern   EOFILE * em_openlist;
extern   int         em_verify(EOFILE * fd);

/* ++ REE/EDC - removed requirement for dynamic data buffer by putting data 
   directly into tx buffer (#define DDB_SIZE 1000) */
/* -- REE/EDC */
#ifndef SEEK_SET
#define SEEK_SET        0               /* seek to an absolute position */
#define SEEK_CUR        1               /* seek relative to current position */
#define SEEK_END        2               /* seek relative to end of file */
#endif  /* ifndef SEEK_SET */

/* ++ REE/EDC - Special file handling functions replaced with one single
   generic type (PSVRFN) */
/* -- REE/EDC */

#endif  /* USE_EMFILES */


#endif /* _WEBFS_H_ */

