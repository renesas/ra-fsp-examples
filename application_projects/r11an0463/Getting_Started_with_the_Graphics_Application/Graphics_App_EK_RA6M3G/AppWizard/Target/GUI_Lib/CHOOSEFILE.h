/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2024  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : CHOOSEFILE.h
Purpose     : File dialog interface
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef CHOOSEFILE_H
#define CHOOSEFILE_H

#include "WM.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define CHOOSEFILE_FINDFIRST 0
#define CHOOSEFILE_FINDNEXT  1

#define CHOOSEFILE_FLAG_DIRECTORY (1 << 0)

#ifndef   CHOOSEFILE_MAXLEN
  #define CHOOSEFILE_MAXLEN 256
#endif

/*********************************************************************
*
*       CHOOSEFILE button indexes
*
*  Description
*    Button indexes used by the CHOOSEFILE dialog.
*/
#define CHOOSEFILE_BI_CANCEL 0   // Index of 'Cancel' button.
#define CHOOSEFILE_BI_OK     1   // Index of 'Ok' button.
#define CHOOSEFILE_BI_UP     2   // Index of 'Up' button.

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct CHOOSEFILE_INFO CHOOSEFILE_INFO;

/*********************************************************************
*
*       CHOOSEFILE_GET_DATA_FUNC
* 
*  Function description
*    A callback which has to pass information about the requested file
*    to the dialog.
* 
*  Parameters
*    pInfo: [IN/OUT] Pointer to a CHOOSEFILE_INFO structure which contains
*                    all details of the requested file and needs to be
*                    filled by the function.
* 
*  Return value
*    == 0: On success.
*    == 1: On error.
* 
*  Additional information
*    The following structure elements of pInfo have to be set by this function:
*      \item \bc{pInfo->Cmd:}     Determines if information about the first or
*                                 the next file should be returned.
*      \item \bc{pInfo->pRoot:}   Pointer to a string containing the path of the
*                                 directory to be used. The \c{GetData()} function
*                                 then has to use the following elements for providing
*                                 information about the requested file to the dialog:
*      \item \bc{pInfo->pAttrib:} Should point to a string which is shown in the 'Type'
*                                 column. Because the CHOOSEFILE dialog can be used with
*                                 any file system there are no special flags but a string
*                                 which should be passed by the application to the dialog.
*      \item \bc{pInfo->pName:}   Should point to a string which contains the file name
*                                 without path and extension. Shown in the 'Name' column
*                                 of the dialog.
*      \item \bc{pInfo->pExt:}    Should point to a string which contains the extension
*                                 of the file shown in the 'Type' column of the dialog.
*      \item \bc{pInfo->SizeL:}   Should be set to the lower 32 bit of the file length.
*      \item \bc{pInfo->SizeH:}   Should be set to the upper 32 bit of the file length
*                                 in case of file larger than 4,294,967,295 bytes.
*      \item \bc{pInfo->Flags:}   If the requested file is a directory this element has
*                                 to be set to CHOOSEFILE_FLAG_DIRECTORY. Otherwise it
*                                 has to be 0.
*/
typedef int CHOOSEFILE_GET_DATA_FUNC(CHOOSEFILE_INFO * pInfo);

struct CHOOSEFILE_INFO {
  int                        Cmd;                      // Command for GetData() function
  int                        Id;                       // Id of pressed button (for internal use only)
  const char               * pMask;                    // Mask to be used for searching files
  char                     * pName;                    // (for internal use only)
  char                     * pExt;                     // (for internal use only)
  char                     * pAttrib;                  // (for internal use only)
  WM_TOOLTIP_HANDLE          hToolTip;                 // (for internal use only)
  U32                        SizeL;                    // FileSize low word
  U32                        SizeH;                    // FileSize high word
  U32                        Flags;                    // File flags
  char                       pRoot[CHOOSEFILE_MAXLEN]; // Buffer used internally and for passing result
  CHOOSEFILE_GET_DATA_FUNC * pfGetData;                // Pointer to GetData() function
};

/*********************************************************************
*
*       Functions
*
**********************************************************************
*/
WM_HWIN CHOOSEFILE_Create(WM_HWIN           hParent,  // Parent window
                          int               xPos,     // xPosition in window coordinates
                          int               yPos,     // yPosition in window coordinates
                          int               xSize,    // xSize in pixels
                          int               ySize,    // ySize in pixels
                          const char      * apRoot[], // Pointers to root strings
                          int               NumRoot,  // Number of roots
                          int               SelRoot,  // Root to be selected at first
                          const char      * sCaption, // Shown in title bar
                          int               Flags,    // Flags for FRAMEWINDOW
                          CHOOSEFILE_INFO * pInfo     // Pointer to CHOOSEFILE_INFO structure
                          );

void    CHOOSEFILE_Callback            (WM_MESSAGE * pMsg);
void    CHOOSEFILE_EnableToolTips      (void);
void    CHOOSEFILE_SetButtonText       (WM_HWIN hWin, unsigned ButtonIndex, const char * pText);
void    CHOOSEFILE_SetDefaultButtonText(unsigned ButtonIndex, const char * pText);
void    CHOOSEFILE_SetDelim            (char Delim);
void    CHOOSEFILE_SetToolTips         (const TOOLTIP_INFO * pInfo, int NumItems);
void    CHOOSEFILE_SetTopMode          (unsigned OnOff);

#if defined(__cplusplus)
  }
#endif

#endif /* GUI_WINSUPPORT */

#endif /* CHOOSEFILE_H */

/*************************** End of file ****************************/
