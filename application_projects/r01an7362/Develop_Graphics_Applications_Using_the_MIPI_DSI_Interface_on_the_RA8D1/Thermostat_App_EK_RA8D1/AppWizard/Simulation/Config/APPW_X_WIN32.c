/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2004  SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

***** emWin - Graphical user interface for embedded applications *****
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : APPW_X_Win32.c
Purpose     : Windows file system access
---------------------------END-OF-HEADER------------------------------
*/

#if defined(WIN32)

#include <Windows.h>

#include "AppWizard.h"

/*********************************************************************
*
*         Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _Open
*/
static void * _Open(const char * pFilename) {
  HANDLE hFile;

  //
  // Open file
  //
  hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    return (void *)hFile;
  }
  return (void *)NULL;
}

/*********************************************************************
*
*       _Seek
*/
static int _Seek(const void * p, U32 Off) {
  HANDLE   hFile;
  
  hFile  = (HANDLE)p;
  //
  // Set file pointer to the required position
  //
  if (SetFilePointer(hFile, Off, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
    return 1;
  }
  return 0;
}

/*********************************************************************
*
*       _Read
*/
static U32 _Read(const void * p, void * pData, U32 NumBytes) {
  HANDLE   hFile;
  DWORD    NumBytesRead;
  
  hFile  = (HANDLE)p;
  //
  // Read data into buffer
  //
  ReadFile(hFile, pData, NumBytes, &NumBytesRead, NULL);
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}

/*********************************************************************
*
*       _Close
*/
static void _Close(const void * p) {
  HANDLE hFile;

  hFile = (HANDLE)p;
  CloseHandle(hFile);
}

/*********************************************************************
*
*       _Size
*/
static U32 _Size(const void * p) {
  U32 FileSize;
  HANDLE hFile;

  hFile = (HANDLE)p;
  FileSize = GetFileSize(hFile, NULL);
  return FileSize;
}

/*********************************************************************
*
*       _FileAccess
*/
static APPW_X_FILEACCESS _FileAccess = {
  _Open,
  _Seek,
  _Read,
  _Close,
  _Size,
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       APPW_X_FS_Init
*/
void APPW_X_FS_Init(void) {
  APPW_SetFileAccess(&_FileAccess);
}

#endif

/*************************** End of file ****************************/
