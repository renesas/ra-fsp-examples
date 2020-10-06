/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : webSSI.c
* Version      : 1.00
* Device(s)    : Renesas
* Description  : Server Side Include file handler functions
******************************************************************************
* History      : DD.MM.YYYY Ver. Description
*              : 04.02.2010 1.00 First Release
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
#include "webSSI.h"
#include "webCGI.h"

/*****************************************************************************
Defines
******************************************************************************/

/*****************************************************************************
Constant Macros
******************************************************************************/

/*****************************************************************************
Function Macros
******************************************************************************/

/*****************************************************************************
Enumerated Types
******************************************************************************/

/*****************************************************************************
Typedefs
******************************************************************************/

/*****************************************************************************
Constant Data
******************************************************************************/

/*****************************************************************************
Function Prototypes
******************************************************************************/

/*****************************************************************************
External Variables
******************************************************************************/

/*****************************************************************************
Global Variables
******************************************************************************/

static const SSITAB gSsiTab;

/*****************************************************************************
Public Functions
******************************************************************************/

/*****************************************************************************
Function Name: ssiGetFunction
Description:   Function to check the table of embedded SSI functions for the
               given file name
Arguments:     IN  pszSsiFileName - Pointer to the SSI file name
Return value:  Pointer to the function to handle the request or NULL on error
*****************************************************************************/
PSVRFN ssiGetFunction(int8_t *pszSsiFileName)
{
    size_t st_index = gSsiTab.stNumber;

    while (st_index--)
    {
        if (!pathCompare((const char *) gSsiTab.pSsiList[st_index].pszSsiFileName, (const char *) pszSsiFileName))
        {
            return gSsiTab.pSsiList[st_index].pSsiFunction;
        }
    }

    return NULL;
}
/*****************************************************************************
End of function  ssiGetFunction
******************************************************************************/

/*****************************************************************************
Private Functions
******************************************************************************/

/*****************************************************************************
Function Name: ssiTimeAndDate
Description:   Function to format the current time and date for the
               timeanddate.ssi file
Arguments:     IN/OUT pSess - Pointer to the session data
               IN/OUT pEoFile - Pointer to the embedded file object
Return value:  0 for success or error code
*****************************************************************************/
static int ssiTimeAndDate(PSESS pSess, PEOFILE pEoFile)
{
//    DATE Date;
    (void) pEoFile;
#if 0
    if (control(g_rtc_handle, CTL_GET_DATE, &Date) == 0)
    {
        static char * const ppszWeekDay[] =
        {
            "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat", "Sun"
        };
        static char * const ppszMonth[] =
        {
            "None", "Jan", "Feb", "March", "April", "May", "June",
            "July", "Aug", "Sept", "Oct", "Nov", "Dec"
        };

        /* Format the date */
        wi_printf(pSess,
                  "%s, %u %s %u %.2u:%.2u:%.2u GMT",
                  ppszWeekDay[(int)Date.Field.WeekDay],
                  (int)Date.Field.Day,
                  ppszMonth[(int)Date.Field.Month],
                  (int)Date.Field.Year,
                  (int)Date.Field.Hour,
                  (int)Date.Field.Minute,
                  (int)Date.Field.Second);
        return 0;
    }
#endif
    return -1;
}
/*****************************************************************************
End of function  ssiTimeAndDate
******************************************************************************/
#if 0
/*****************************************************************************
Function Name: ssiShowMemorySize
Description:   Function to print a length of memory in engineering notation
Arguments:     IN/OUT  pSess - Pointer to the session data
               IN  stLength - The length to display
Return value:  none
*****************************************************************************/
static void ssiShowMemorySize(PSESS pSess, size_t stLength)
{
    float   fSize = (float)stLength;
    static int8_t * const pszEngMultChars = " kMGTPEZY";
    int8_t *  pszEngMult = pszEngMultChars;

    /* Scale for engineering multiplier */
    while (fSize > 1024.0f)
    {
        fSize /= 1024.0f;
        pszEngMult++;
    }

    /* Print the details */
    wi_printf(pSess,
              "%.3f%cB",
              fSize,
              *pszEngMult);
}
/*****************************************************************************
End of function  ssiShowMemorySize
******************************************************************************/
#endif
/*****************************************************************************
Function Name: ssiSystemInfo
Description:   SSI to get the system information
Arguments:     IN/OUT pSess - Pointer to the session data
               IN/OUT pEoFile - Pointer to the embedded file object
Return value:  0 for success or error code
*****************************************************************************/
static int ssiSystemInfo(PSESS pSess, PEOFILE pEoFile)
{
#if 0 // Knock ou system info
    int     iMaxTask = 100;
    int     iTaskCount = 0;
    _Bool   bfHeader = false;
    pEoFile;
    while (iMaxTask--)
    {
        TASKINFO    tcInfo;
        if (taskInfo((uint32_t)iMaxTask, &tcInfo))
        {
            static const char * const pszState[] =
            {
                "WAIT SINGLE",
                "WAIT ALL",
                "WAIT MUTEX",
                "RUNNING",
                "BUSY",
                "SUSPENDED",
                "UNKNOWN6",
                "UNKNOWN7"
            };
            if (!bfHeader)
            {
                wi_printf(pSess,
                        "<br><br>Task  ID %*s %*s Pri Stack<br>",
                        TC_MAX_TASK_NAME_SIZE, "Name",
                        12, "State");
                /* Print the header only once */
                bfHeader = true;
            }
            wi_printf(pSess,
                    "%8u %*s %*s %2u  %lu<br>",
                    tcInfo.uiTaskID,
                    TC_MAX_TASK_NAME_SIZE,
                    tcInfo.pszName,
                    12,
                    pszState[tcInfo.tState & 7],
                    tcInfo.uiPriority,
                    tcInfo.stFreeStack);
            iTaskCount++;
        }
    }
    wi_printf(pSess, "Found %d tasks<br><br>", iTaskCount);
    {
        SYSUSAGE    sysUsage;
        HEAPUSAGE   memUsage;
        MEMTYPE     memType;
        /* Save FPSCR and set to no exceptions */
        int         iFPSCR = get_fpscr();
        set_fpscr(SH2_FPSCR_NONE);
        /* For each heap */
        for (memType = 0; memType < NUMBER_OF_HEAPS; memType++) {
            /* Get the memory usage information */
            heapUsage(&memUsage, memType);
            /* Print the heap number */
            wi_printf(pSess, "Heap %d:<br><br>", memType);
            /* Print the heap usage */
            wi_printf(pSess, "    Free ");
            ssiShowMemorySize(pSess, memUsage.stTotalFree);
            wi_printf(pSess, ", Used ");
            ssiShowMemorySize(pSess, memUsage.stTotalAllocated);
            wi_printf(pSess, ", Total ");
            ssiShowMemorySize(pSess, memUsage.stTotalHeap);
            wi_printf(pSess, "<br>    Largest free segment ");
            ssiShowMemorySize(pSess, memUsage.stLargestFreeSegment);
            wi_printf(pSess, ", Overhead ");
            ssiShowMemorySize(pSess, memUsage.stOverhead);
            wi_printf(pSess, "<br>");
            /* Print the fragmentation */
            wi_printf(pSess, "    Fragmentation %.3f%%<br><br>",
                    (float)((1.0f - ((float)memUsage.stLargestFreeSegment /
                                     (float)memUsage.stTotalFree)) * 100));
        }
        /* Get the system usage information */
        systemUsage(&sysUsage);
        /* Print the results */
        wi_printf(pSess, "System:<br>    Files %lu/%lu<br>"
                "    Events %lu/%lu<br>    Timers %lu/%lu<br>",
                sysUsage.stFilesInUse,
                sysUsage.stFileTotal,
                sysUsage.stEventsInUse,
                sysUsage.stEventTotal,
                sysUsage.stTimersInUse,
                sysUsage.stTimerTotal);
        set_fpscr(iFPSCR);
    }
#else
//    UNUSED_PARAM(pSess);
//    UNUSED_PARAM(pEoFile);
    return 0;


#endif

}
/*****************************************************************************
End of function  ssiSystemInfo
******************************************************************************/

/*****************************************************************************
Function Name: ssiUsbDeviceInfo
Description:   SSI to get the attached USBN devices
Arguments:     IN/OUT pSess - Pointer to the session data
               IN/OUT pEoFile - Pointer to the embedded file object
Return value:  0 for success or error code
*****************************************************************************/
static int ssiUsbDeviceInfo(PSESS pSess, PEOFILE pEoFile)
{
#if 0 // knock out usb support
    int     iIndex = 0;
    DYNDEV  devInfomation;
    (void)pEoFile;
    while (R_DEVLINK_DevInfo(&devInfomation, iIndex))
    {
        wi_printf(pSess, "Link name: %s<br>", devInfomation.pszDeviceLinkName);
        if (strlen(devInfomation.Information.pszManufacturer))
        {
            wi_printf(pSess, "Manufacturer: %s<br>",
                      devInfomation.Information.pszManufacturer);
        }
        else
        {
            wi_printf(pSess, "Manufacturer: Unknown<br>");
        }
        if (strlen(devInfomation.Information.pszProduct))
        {
            wi_printf(pSess,
                      "Product: %s<br>",
                      devInfomation.Information.pszProduct);
        }
        else
        {
            wi_printf(pSess, "Product: Unknown<br>");
        }
        if (strlen(devInfomation.Information.pszSerialNumber))
        {
            wi_printf(pSess,
                      "Serial Number: %s<br>",
                      devInfomation.Information.pszSerialNumber);
        }
        else
        {
            wi_printf(pSess, "Serial Number: Unknown<br>");
        }
        wi_printf(pSess, "Vendor ID: 0x%.4X<br>",
                  devInfomation.Information.wVID);
        wi_printf(pSess, "Product ID: 0x%.4X<br>",
                  devInfomation.Information.wPID);
        wi_printf(pSess, "Device Class: 0x%.2X<br>",
                  devInfomation.Information.byDeviceClass);
        wi_printf(pSess, "Device Sub Class: 0x%.2X<br>",
                  devInfomation.Information.byDeviceSubClass);
        wi_printf(pSess, "Device Protocol: 0x%.2X<br>",
                  devInfomation.Information.byProtocolCode);
        if (devInfomation.p_device_driver)
        {
            wi_printf(pSess,
                      "Device Driver \"%s\" available for this device<br><br>",
                      devInfomation.p_device_driver->p_device_name);
        }
        else
        {
            wi_printf(pSess, "There is no device driver available<br><br>");
        }
        iIndex++;
    }
    if (!iIndex)
    {
        wi_printf(pSess, "No USB devices attached.<br>");
    }
#endif // Knock out USB support
    return 0;
}
/*****************************************************************************
End of function  ssiUsbDeviceInfo
******************************************************************************/

/******************************************************************************
Function Name: ssiSystemResourceList
Description:   SSI to generate the system resource drop down list options
Arguments:     IN/OUT pSess - Pointer to the session data
               IN/OUT pEoFile - Pointer to the embedded file object
Return value:  0 for success or error code
*****************************************************************************/
static int ssiSystemResourceList(PSESS pSess, PEOFILE pEoFile)
{
#if 0 // Knock out MEM VIEW support

    uint16_t memType;
    (void)pEoFile;

    /* Format the system resources available */
    wi_printf(pSess, "<option selected> CPU\r\n"
                     "<option> Events\r\n"
                     "<option> Files\r\n"
                     "<option> Timers\r\n");
    for (memType = 0; memType < NUMBER_OF_HEAPS; memType++)
    {
        wi_printf(pSess, "<option> Heap %d\r\n", memType);
    }
#endif // Knock out MEM VIEW support

    return 0;
}
/******************************************************************************
End of function  ssiSystemResourceList
******************************************************************************/

/*****************************************************************************
Constant Data
******************************************************************************/

static const SSIFNASS gpSsiFnAss[] =
{
    {
        (uint8_t *) "timeanddate.ssi",
        ssiTimeAndDate,
    },
    {
        (uint8_t *) "sysinfo.ssi",
         ssiSystemInfo,
    },
    {
        (uint8_t *) "usbdeviceinfo.ssi",
        ssiUsbDeviceInfo,
    },
    {
        (uint8_t *) "sri_options.ssi",
        ssiSystemResourceList,
    }

    /* TODO: Add more ssi file names and handling functions */
};

static const SSITAB gSsiTab =
{
    (PSSIFNASS)gpSsiFnAss,
    sizeof(gpSsiFnAss) / sizeof(SSIFNASS)
};

/******************************************************************************
End  Of File
******************************************************************************/
