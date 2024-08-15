/***********************************************************************************************************************
* Copyright (c) 2012 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
* File Name    : liveFile.c
* Version      : 1.00
* Description  : Functions to create "live" files which return measurement data
*******************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 04.04.2011 1.00    First Release
******************************************************************************/

/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include "websys.h"
#include "webio.h"
#include "liveFile.h"
//#include "sysUsage.h"

/******************************************************************************
Global Variables
******************************************************************************/

static const LITAB gLiveTab;

/******************************************************************************
Public Functions
******************************************************************************/

/******************************************************************************
Function Name: liveFindFile
Description:   Function to open data from a live file
Arguments:     IN  pszFileName - Pointer to the file path and name
               OUT pEfsFile - Pointer to the encapsulated file information
Return value:  0 for success or error code
******************************************************************************/
EFSERR liveFindFile(int8_t *pszFileName, PEFS pEfsFile)
{
    size_t  stIndex = gLiveTab.stNumber;
    int8_t  pszCacheBuster[256];
    int8_t  *pszNoCache = strstr(pszFileName, " -nocache");
    int8_t  *pzsFile = pszFileName;
    /* If the file name has -nocache appended to the name then remove it */
    if (pszNoCache)
    {
        int8_t *pszTemp = pszCacheBuster;
        while (pszFileName < pszNoCache)
        {
            *pszTemp++ = *pszFileName++;
        }
        *pszTemp++ = '\0';
        pzsFile = pszCacheBuster;
    }
    while (stIndex--)
    {
        int8_t *pszLiveFileName = gLiveTab.pLiveFileList[stIndex].pszLiveFileName;
        if (!pathCompare(pszLiveFileName, pzsFile))
        {
            return 
                gLiveTab.pLiveFileList[stIndex].pLiveFunction(pszLiveFileName,
                                                              pEfsFile);
        }
    }
    return -1;
}
/*****************************************************************************
End of function  liveFindFile
******************************************************************************/

/******************************************************************************
Private Functions
******************************************************************************/

/******************************************************************************
Function Name: liveCreateFile
Description:   Function to creat the EFS object
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
               IN  pszData - Pointer to the data string to go into the file
Return value:  Fixed to 0 for success
******************************************************************************/
static int liveCreateFile(const int8_t *pszFileName, PEFS pEfsFile, char *pszData)
{
    /* Get the length of the data */
    pEfsFile->pszFileName = pszFileName;
    pEfsFile->pszFilePath = "/";
    pEfsFile->pbyFileData = (uint8_t*)pszData;
    pEfsFile->ulFileLength = (uint32_t)strlen(pszData);
    pEfsFile->bfDataAllocated = true;
    return 0;
}
/******************************************************************************
End of function  liveCreateFile
******************************************************************************/

/*****************************************************************************
Function Name: liveGetCpu
Description:   Function to get the system resource information for CPU
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetCpu(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_CPU_USAGE);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetCpu
******************************************************************************/

/*****************************************************************************
Function Name: liveGetEvents
Description:   Function to get the system resource information for Events
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetEvents(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_EVENT_USAGE);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetEvents
******************************************************************************/

/*****************************************************************************
Function Name: liveGetFiles
Description:   Function to get the system resource information for Files
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetFiles(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_FILES_USAGE);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetFiles
******************************************************************************/

/*****************************************************************************
Function Name: liveGetTimers
Description:   Function to get the system resource information for Timers
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetTimers(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_TIMER_USAGE);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetTimers
******************************************************************************/

/*****************************************************************************
Function Name: liveGetHeap0
Description:   Function to get the system resource information for Heap 0
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetHeap0(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_NUM + HEAP_WEB_SERVER);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetHeap0
******************************************************************************/

/*****************************************************************************
Function Name: liveGetHeap1
Description:   Function to get the system resource information for Heap 1
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetHeap1(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_NUM + HEAP_LWIP_PRIMARY);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetHeap1
******************************************************************************/

/*****************************************************************************
Function Name: liveGetHeap2
Description:   Function to get the system resource information for Heap 2
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetHeap2(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_NUM + HEAP_SDRAM);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetHeap2
******************************************************************************/

/*****************************************************************************
Function Name: liveGetHeap3
Description:   Function to get the system resource information for Heap 3
Arguments:     IN  pszFileName - Pointer to the file name
               OUT pEfsFile - Pointer to the embedded file system object
Return value:  0 for success -1 on error
*****************************************************************************/
static int liveGetHeap3(const int8_t *pszFileName, PEFS pEfsFile)
{
    /* Get the data */
    int8_t    *pszData = sriGet(SYRES_NUM + HEAP_SRAM);
    if (pszData)
    {
        return liveCreateFile(pszFileName, pEfsFile, pszData);
    }
    return -1;
}
/*****************************************************************************
End of function  liveGetHeap3
******************************************************************************/

/*****************************************************************************
Constant Data
******************************************************************************/

static const LIFNASS gpLiveFnAss[] =
{
    "sri_cpu.json",
    liveGetCpu,

    "sri_events.json",
    liveGetEvents,

    "sri_files.json",
    liveGetFiles,

    "sri_timers.json",
    liveGetTimers,

    "sri_heap0.json",
    liveGetHeap0,

    "sri_heap1.json",
    liveGetHeap1,

    "sri_heap2.json",
    liveGetHeap2,

    "sri_heap3.json",
    liveGetHeap3
    /* TODO: Add more live file names and handling functions */

};

static const LITAB gLiveTab =
{
    gpLiveFnAss,
    sizeof(gpLiveFnAss) / sizeof(LIFNASS)
};

/******************************************************************************
End  Of File
******************************************************************************/
