/***********************************************************************************************************************
* Copyright (c) 2011 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
 /******************************************************************************
 * File Name    : cgiMsExplore.c
 * Version      : 1.00
 * Description  : Function to render a web page to browse a mass storage device
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

#include <ctype.h>
#include <fcntl.h>
#include "websys.h"
#include "webCGI.h"
#include "malloc.h"

/******************************************************************************
 Defines
 ******************************************************************************/

/* Fifteen entries per page including the parent directory */
#define BD_MAX_FILE_ENTRIES         (12)
#define BD_MIN_FILE_NAME_PADDIND    (49)
#define BD_TEMP_STRING_SZIE         (1024 * 3)
/* Simple cache to remember the drive and path a client was at the last 
 time the browser page was loaded */
#define BD_CLIENT_PATH_CACHE_SIZE   (16)
#define BD_CLIENT_PATH_LENGTH       (1024)

#define MAX_BUFFER_SIZE_PRV_        (256 * 1024)

/******************************************************************************
 Constant Macros
 ******************************************************************************/

/******************************************************************************
 Function Macros
 ******************************************************************************/

#undef TRACE
#ifdef _TRACE_ON_                   /* Trace ON */
    #define TRACE(_x_)                  Trace _x_
#else                               /* _NO_TRACE_ON_ */
    #define TRACE(_x_)                  /* TRACE REMOVED */
#endif 

/******************************************************************************
 Enumerated Types
 ******************************************************************************/

typedef enum _DSORT
{
    DIR_SORT_BY_NAME_A_Z = 0,
    DIR_SORT_BY_NAME_Z_A,
    DIR_SORT_BY_DATE_RECENT,
    DIR_SORT_BY_DATE_OLD,
    DIR_SORT_BY_SIZE_LARGE,
    DIR_SORT_BY_SIZE_SMALL,
    DIR_SORT_NUM
} DSORT, *PDSORT;

typedef enum _SESRT
{
    DIR_SORT_BY_NONE = 0, DIR_SORT_BY_NAME, DIR_SORT_BY_DATE, DIR_SORT_BY_SIZE
} SESRT, *PSESRT;

/******************************************************************************
 Typedefs
 ******************************************************************************/

typedef struct _DLST
{
    struct _DLST *pNext;
    FATENTRY     fatEntry;
} DLST, *PDLST;

typedef _Bool (* const PDCMP) (PDLST pDLST, PDLST pNew);


/* This is the data that is required by almost every function used to generate
 the HTML file browser */
typedef struct _HTDIR
{
    /* Pointer to the session */
    PSESS    pSess;
    /* The index in the directory */
    int      iIndex;
    /* The number of entries to print */
    int      iEntryCount;
    /* Pointer to the current drive */
    PDRIVE   pDrive;
    /* The current drive letter */
    char     chDrive;
    /* The current directory */
    char     *pszDir;
    /* The current directory path with coded to be URL safe */
    char     *pszEncodedPath;
    /* The type of sorting required */
    DSORT    dirSort;
    /* The number of characters to pad between the file name and the date */
    size_t   stFileNamePadding;
    /* The last FAT eror code */
    FATERR   fatError;
    /* The current FAT entry */
    FATENTRY fatEntry;
    /* Flag to get the previous page */
    _Bool    bfGetPrev;
    /* Flag to get the next page */
    _Bool    bfGetNext;
    /* The sorted lists */
    PDLST    pDirList;
    PDLST    pFileList;
} HTDIR, *PHTDIR;

typedef struct _MSTEST
{
    os_task_t *uiTaskID;
    char      chDrive;
    uint32_t   iTestSize;
    _Bool     bfResultsValid;
    char      pszFileName[64];
    char      pszWriteSpeed[64];
    char      pszReadSpeed[64];
    char      pszTestResult[64];
} MSTEST, *PMSTEST;

/******************************************************************************
 Constant Data
 ******************************************************************************/

/* Nothing inserted */
static const char * const gcpszHeader = "<table width=\"100%%\" height=\"315\" border=\"0\">\r\n<tr>\r\n"
        "<td height=\"300\" colspan=\"2\" valign=\"top\">\r\n<pre>\r\n";

/* Single character insert of the drive letter */
static const char * const gcpszActiveDrive = "<img src=\"images/drive_magnify.png\" width=\"16\" height=\"16\"> <a h"
        "ref=\"#\" onclick=\"callFunc('ms_explore', 'ms_explore.cgi',"
        " 'drv,%c,%d')\">%c:/</A> ";
/* 1. Drive letter
 2. Sort type
 3. Drive letter */
static const char * const gcpszAvailableDrive = "<img src=\"images/drive.png\" width=\"16\" height=\"16\"> <a h"
        "ref=\"#\" onclick=\"callFunc('ms_explore', 'ms_explore.cgi',"
        " 'drv,%c,%d')\">%c:/</A> ";

/* Single string for the current folder */
static const char * const gcpszCurrentFolder = "\r\n<img src=\"images/folder-open.png\" width=\"16\" height=\"16\">"
        " <strong>%c:%s</strong>\r\n";

/* Nothing inserted */
static const char * const gcpszTableHeader = "<img src=\"images/folder-open.png\" width=\"16\" height=\"16\">"
        " <a href=\"#\" onclick=\"callFunc('ms_explore', 'ms_explore.cgi', 'na"
        /* Name field size = 53 */
        ",%c:%s,%d,%d')\">Name</A>%.*s "
        "   <a href=\"#\" onclick=\"callFunc('ms_explore', 'ms_explore.cgi', 'cr"
        /* Date field size = 13 */
        ",%c:%s,%d,%d')\">Date</A>         <a href=\"#\" onclick=\"callFunc('"
        "ms_explore', 'ms_explore.cgi', 'sz,%c:%s,%d,%d')\">Size</A>\r\n<hr>\r\n";

/* 1. Parent directory path string 
 2. onClick string
 3. 17 character wide date string */
static const char * const gcpszParentDirectory = "<img src=\"images/arrow_up.png\" width=\"16\" height=\"16\""
        "> <a href=\"#\"%s>Parent Directory</A>%.*s"
        " %s            -\r\n";

/* 1. page_white_text or folder
 2. Full drive path and file name
 3. File name
 4. Padding = maxlen - file name length
 5. 40 spaces
 6. 17 character date
 7. 12 character file size
 */
static const char * const gpszEntry = "<img src=\"images/%s.png\" width=\"16\" height=\"16\"> <a "
        "href=\"%s\"%s>%s</A> %.*s %s %s\r\n";

/* 1. Previous button (if required)
 2. Next button (if required)
 Note: The entire button string needs to be pre-formatted */
static const char * const gcpszTableFooter = "</pre>\r\n<hr>\r\n</td>\r\n</tr>\r\n<tr>\r\n<td width=\"50%%\""
        ">\r\n<div align=\"center\">\r\n%s\r\n</div></td>\r\n<td width="
        "\"50%%\">\r\n<div align=\"center\">\r\n%s\r\n</div></td>\r\n</"
        "tr>\r\n";

/* Nothing */
static const char * const gcpszNoDisks = "<table width=\"100%%\" height=\"250\" border=\"0\">\r\n<tr>\r\n"
        "<td height=\"62\">&nbsp;</td>\r\n<td>&nbsp;</td>\r\n<td>&nbsp;"
        "</td>\r\n</tr>\r\n<tr>\r\n<td height=\"200\">&nbsp;</td>\r\n<t"
        "d><div align=\"center\">\r\n<p><strong><img src=\"images/page_"
        "error.gif\" width=\"42\" height=\"52\" /></strong></p>\r\n<p><"
        "strong>No mass storage devices found.</strong></p>\r\n</div></"
        "td>\r\n<td>&nbsp;</td>\r\n</tr>\r\n<tr>\r\n<td>&nbsp;</td>\r\n"
        "<td>&nbsp;</td>\r\n<td>&nbsp;</td>\r\n</tr>\r\n</table>\r\n";

static const char * const gcpszNoFatLib = "<table width=\"100%%\" height=\"250\" border=\"0\">\r\n<tr>\r\n"
        "<td height=\"62\">&nbsp;</td>\r\n<td>&nbsp;</td>\r\n<td>&nbsp;"
        "</td>\r\n</tr>\r\n<tr>\r\n<td height=\"200\">&nbsp;</td>\r\n<t"
        "d><div align=\"center\">\r\n<p><strong><img src=\"images/page_"
        "error.gif\" width=\"42\" height=\"52\" /></strong></p>\r\n<p>"
        "<a href=\"FullFatEval.pdf\" target=\"_blank\">No FAT library "
        "installed.</a></p>\r\n</div></"
        "td>\r\n<td>&nbsp;</td>\r\n</tr>\r\n<tr>\r\n<td>&nbsp;</td>\r\n"
        "<td>&nbsp;</td>\r\n<td>&nbsp;</td>\r\n</tr>\r\n</table>\r\n";

static const char * const pcpszTestInProgress = "<div><p align=\"center\"><img src=\"images/spinner3-black.gif\""
        " width=\"36\" height=\"36\" /></p></div>\r\n";

static const char * const pcpszTestResults = "<div><p>File: %s</p></div>\r\n"
        "<div><p>%s</p></div>\r\n"
        "<div><p>%s</p></div>\r\n"
        "<div><p>%s</p></div>\r\n";

/* This is a lazy way but it means the ws_printf function can be used.
 Max file name length is 255 */
static const char * const gpszPad = "                                                               "
        "                                                               "
        "                                                               "
        "                                                               ";

/******************************************************************************
 Function Prototypes
 ******************************************************************************/

extern int stricmp (const char *s1, const char *s2);

static void taskReadWritePerfTest (PMSTEST pMsTest);
static void bdSearch (PSESS pSess, char *pchDrive, char *pszDir, int *piIndex, PDSORT pDirSort);
static void bdCache (PSESS pSess, char *pchDrive, char *pszDir, int *piIndex, PDSORT pDirSort);
static void bdSetArgumentFlags (char *pszArgument, PHTDIR pHtDir, PSESRT pSortSelect);
static DSORT bdModifySortType (DSORT dirSort, SESRT sortSelect);
static int bdGeneratePage (PHTDIR pHtDir);
static void cgiMakeWRTestFileName (char *pszDestFileName, uint32_t fFileSize, char chDrive);

int cgiMsExplore (PSESS pSess, PEOFILE pEoFile);
int cgiMsTest (PSESS pSess, PEOFILE pEoFile);
void cgiShowDataRate (char *pszDest, float fTransferTime, size_t stLength, _Bool bfDirection);
float bdCompareDate (PDLST pDLST, PDLST pNew);

/******************************************************************************
 External Variables
 ******************************************************************************/

/******************************************************************************
 Global Variables
 ******************************************************************************/

/* Memory for the desired file path */
static char gpszPath[BD_TEMP_STRING_SZIE];
static char gpszEncodedPath[BD_TEMP_STRING_SZIE];

/* Temporary memory for file paths etc.. */
static char gpszTemp1[BD_TEMP_STRING_SZIE];
static char gpszTemp2[BD_TEMP_STRING_SZIE];
static DIR gs_dir;
static char dir_init = 0u;

/* TODO: use cookie */
static struct _CACHE
{
    /* The tag by which the client is identified */
    unsigned long ulAddress;
    /* Least Recently Used count to identify which entry to overwrite */
    unsigned long ulLRU;
    /* The information that is cached */
    char          pszDir[BD_CLIENT_PATH_LENGTH];
    char          chDrive;
    int           iIndex;
    DSORT         dirSort;
} gpClientPathCache[BD_CLIENT_PATH_CACHE_SIZE];

/******************************************************************************
 Public Functions
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiMsExplore
 Description:   Function to browse the MS drives attached to the
 Parameters:    IN  pSess - Pointer to the session
 IN  pEoFile - Pointer to the file object
 Return value:  0 for success
 *****************************************************************************/
int cgiMsExplore (PSESS pSess, PEOFILE pEoFile)
{
    (void) pEoFile;

    /* ensure that directory object is initialised */
    if (0u == dir_init)
    {
        memset(&gs_dir, 0x30u, sizeof(DIR));
        dir_init = 1u;
    }

    if (!R_FAT_LoadLibrary())
    {
        HTDIR htDir;

        memset(&htDir, 0, sizeof(HTDIR));
        memset(&(htDir.pDirList->fatEntry), 0, sizeof(FATENTRY));
        memset(&(htDir.pFileList->fatEntry), 0, sizeof(FATENTRY));

        /* Ask the disk manager to check to see if there are any new drives */
        dskNewPnPDrive();
        htDir.pDrive = dskGetFirstDrive((int8_t *) &htDir.chDrive);

        if (htDir.pDrive)
        {
            char *pszArgument;

            pszArgument = cgiGetArgument(pSess, true);
            strcpy(gpszPath, "/");

            if (pszArgument)
            {
                DSORT NewDirSort;

                /* If the page is being loaded for the first time */
                if (cgiArgument(pszArgument, "onLoad"))
                {
                    /* Search the cache to see if we have a drive and path */
                    bdSearch(pSess, &htDir.chDrive, gpszPath, &htDir.iIndex, &htDir.dirSort);
                }
                else if (cgiArgument(pszArgument, "drv"))
                {
                    /* Get the drive letter from the argument */
                    htDir.chDrive = *(pszArgument + 4);
                }

                /* The basis of most of the navigation */
                else if ((cgiArgument(pszArgument, "cd")) || (cgiArgument(pszArgument, "nx"))
                        || (cgiArgument(pszArgument, "pv")) || (cgiArgument(pszArgument, "na"))
                        || (cgiArgument(pszArgument, "cr")) || (cgiArgument(pszArgument, "sz")))
                {
                    char *pszScan;
                    SESRT sortSelect = DIR_SORT_BY_NONE;

                    /* Set the argument flags */
                    bdSetArgumentFlags(pszArgument, &htDir, &sortSelect);

                    /* Get the drive letter from the argument */
                    pszArgument += 3;
                    htDir.chDrive = *pszArgument;

                    /* Point at the start of the path */
                    pszArgument += 2;

                    /* Terminate at the next occurrence of , */
                    pszScan = cgiSplitArgument(pszArgument);

                    /* Copy in the file path */
                    strcpy(gpszPath, pszArgument);

                    /* Remove any %20's from the string */
                    cgiDecodeString(gpszPath);

                    /* Get the sort type */
                    sscanf(pszScan, "%d", (int *) &htDir.dirSort);

                    /* dirSort references a table of function pointers. Make sure
                     that the value obtained does not exceed the table length */
                    htDir.dirSort = (htDir.dirSort >= DIR_SORT_NUM) ? DIR_SORT_NUM : htDir.dirSort;

                    /* Modify the previous directory sort type */
                    NewDirSort = bdModifySortType(htDir.dirSort, sortSelect);

                    /* If there has been a new sort selection */
                    if (NewDirSort != htDir.dirSort)
                    {
                        /* The index will be set to 0 by the memset */
                        htDir.dirSort = NewDirSort;
                    }
                    else
                    {
                        /* For the previous and next functions the final par is
                         the current index */
                        if ((htDir.bfGetPrev) || (htDir.bfGetNext))
                        {
                            /* Terminate at the next occurrence of , */
                            pszScan = cgiSplitArgument(pszScan);
                            /* Get the index into the directory */
                            sscanf(pszScan, "%d", &htDir.iIndex);
                        }
                    }
                }
            }

            /* Set the directory path strings */
            htDir.pszDir = gpszPath;
            cgiEncodeString(gpszEncodedPath, sizeof(gpszEncodedPath), gpszPath);
            htDir.pszEncodedPath = gpszEncodedPath;
            htDir.iEntryCount = BD_MAX_FILE_ENTRIES;

            /* Set the session */
            htDir.pSess = pSess;

            /* Generate the page */
            if (bdGeneratePage(&htDir))
            {
                /* This failure may be because the client has a cached
                 drive and path which is no longer valid. Try again
                 but for the default settings */
                memset(&htDir, 0, sizeof(HTDIR));
                htDir.pDrive = dskGetFirstDrive((int8_t *) &htDir.chDrive);
                strcpy(gpszPath, "/");
                htDir.pszDir = gpszPath;
                cgiEncodeString(gpszEncodedPath, sizeof(gpszEncodedPath), gpszPath);
                htDir.pszEncodedPath = gpszEncodedPath;
                htDir.iEntryCount = BD_MAX_FILE_ENTRIES;

                /* Set the session */
                htDir.pSess = pSess;

                /* Cache the clients drive and path */
                bdCache(pSess, &htDir.chDrive, gpszPath, &htDir.iIndex, &htDir.dirSort);
                if (bdGeneratePage(&htDir))
                {
                    /* On error show the no disk message */
                    wi_printf(pSess, gcpszNoDisks);
                }
            }
        }
        else
        {
            wi_printf(pSess, gcpszNoDisks);
        }
    }
    else
    {
        wi_printf(pSess, gcpszNoFatLib);
    }
    return 0;
}
/*****************************************************************************
 End of function  cgiMsExplore
 ******************************************************************************/

/*****************************************************************************
 Function Name: cgiMsTest
 Description:   Function to test the MS device
 Parameters:    IN  pSess - Pointer to the session
                IN  pEoFile - Pointer to the file object
 Return value:  0 for success
 *****************************************************************************/
int cgiMsTest (PSESS pSess, PEOFILE pEoFile)
{
    (void) pEoFile;

    static MSTEST msTest =
    {
        0, 'A', 0,
        false, "", "", "", ""
    };

    if (NULL == msTest.uiTaskID)
    {
        char *pszArgument;

        /* Get the argument */
        pszArgument = cgiGetArgument(pSess, true);

        /* Check to see if this is a test start request */
        if ((pszArgument) && (isdigit(*pszArgument)))
        {
            int iDrive = 0;
            msTest.iTestSize = 0;
            msTest.chDrive = 'A';

            /* Get the test size 0..19 */
            sscanf(pszArgument, "%ld", &msTest.iTestSize);

            /* Point a the drive letter index */
            pszArgument = cgiSplitArgument(pszArgument);

            /* Add on the index to get the drive letter */
            sscanf(pszArgument, "%d", &iDrive);
            msTest.chDrive = (char) (msTest.chDrive + iDrive);
            msTest.bfResultsValid = false;
            strcpy(msTest.pszFileName, "None!");
            msTest.pszWriteSpeed[0] = '\0';
            msTest.pszReadSpeed[0] = '\0';
            msTest.pszTestResult[0] = '\0';

            /* Check that A FAT library is included */
            if (R_FAT_LoadLibrary())
            {
                sprintf(msTest.pszTestResult, "No FAT library is installed!");
                msTest.bfResultsValid = true;
            }
            /* Check that the drive that has been selected is valid */
            else if (!dskGetDrive(msTest.chDrive))
            {
                sprintf(msTest.pszTestResult, "Invalid drive letter %c!", msTest.chDrive);
                msTest.bfResultsValid = true;
            }
            else
            {
                /* Create the test task */
                msTest.uiTaskID = R_OS_CreateTask("Drive Test", (os_task_code_t) taskReadWritePerfTest,
                        &msTest, R_OS_ABSTRACTION_PRV_LARGE_STACK_SIZE, TASK_WR_PERF_TEST_PRI);

                wi_printf(pSess, pcpszTestInProgress);
            }
        }

        if (msTest.bfResultsValid)
        {
            wi_printf(pSess, pcpszTestResults, msTest.pszFileName, msTest.pszWriteSpeed, msTest.pszReadSpeed,
                    msTest.pszTestResult);
        }
    }
    else
    {
        wi_printf(pSess, pcpszTestInProgress);
    }

    return 0;
}
/*****************************************************************************
 End of function  cgiMsTest
 ******************************************************************************/

/******************************************************************************
 Private Functions
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiMakeWRTestFileName
 Description:   Function to make a file name based on the file size
 Arguments:     OUT  pszDestFileName - Pointer to the destination file name
 string
 IN   fFileSize - The desired file size
 IN   chDrive - The drive to test
 Return value:  none
 ******************************************************************************/
static void cgiMakeWRTestFileName (char *pszDestFileName, uint32_t fFileSize, char chDrive)
{
    const char *pszEngMult = "#kMGTPEZY";
    static const char * const pszFormat = "%c:\\%d%c";
    while (fFileSize >= 1024)
    {
        fFileSize /= 1024;
        pszEngMult++;
    }

    sprintf(pszDestFileName, pszFormat, chDrive, (int) fFileSize, *pszEngMult);
}
/******************************************************************************
 End of function  cgiMakeWRTestFileName
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiFillFileData
 Description:   Function to fill the file data buffer with known data
 Arguments:     OUT pdwFileData - Pointer to the file data
 IN  stLength - The length of the file in DWORDS
 Return value:  none
 ******************************************************************************/
static void cgiFillFileData (uint32_t *pdwFileData, size_t stLength)
{
    uint32_t dwData = 0UL;
    while (stLength--)
    {
        *pdwFileData++ = dwData++;
    }
}
/******************************************************************************
 End of function  cgiFillFileData
 ******************************************************************************/

/******************************************************************************
 Function Name: cmdCheckFileData
 Description:   Function to check the file data
 Arguments:     OUT pdwFileData - Pointer to the file data
 IN  stLength - The length of the file in DWORDS
 Return value:  true if the data matches
 ******************************************************************************/
static _Bool cgiCheckFileData (uint32_t *pdwFileData, size_t stLength)
{
    uint32_t dwData = 0UL;
    int iCount = 0;
    while (stLength--)
    {
        if ( *pdwFileData != dwData)
        {
            return false;
        }
        pdwFileData++;
        dwData++;
        iCount++;
    }
    return true;
}
/******************************************************************************
 End of function  cgiCheckFileData
 ******************************************************************************/

/******************************************************************************
 Function Name: cgiShowDataRate
 Description:   Function to print the data transfer rate (in engineering units)
 Arguments:     IN  pszDest - Pointer to the destination string
 IN  fTransferTime - The transfer time in seconds
 IN  stLength - The length of data transferred
 IN  bfDirection - The direction of the transfer
 Return value:  none
 ******************************************************************************/
void cgiShowDataRate (char *pszDest, float fTransferTime, size_t stLength, _Bool bfDirection)
{
    if (fTransferTime > 0.0f)
    {
        float fTransferRate;
        const char *pszEngMult = "#kMGTPEZY";
        static const char *ppszDir[] =
        { "Read", "Wrote" };

        /* Calculate the transfer rate */
        fTransferRate = (float) stLength / fTransferTime;

        /* Scale for engineering multiplier */
        while (fTransferRate > 1000.0f)
        {
            fTransferRate /= 1000.0f;
            pszEngMult++;
        }

        /* Print the details */
        sprintf(pszDest, "%s %u bytes at %.2f %c Bytes per Second\r\n", ppszDir[bfDirection], stLength, fTransferRate,
                *pszEngMult);
    }
    else
    {
        sprintf(pszDest, "Invalid transfer time\r\n");
    }
}
/******************************************************************************
 End of function  cgiShowDataRate
 ******************************************************************************/

/******************************************************************************
 Function Name: taskReadWritePerfTest
 Description:   Command to perform a write read performance test
 Arguments:     IN/OUT pMsTest - Pointer to the MS test data
 Return value:  none
 ******************************************************************************/
static void taskReadWritePerfTest (PMSTEST pMsTest)
{
    uint32_t fTestFileSize = 1.0f;
    uint32_t byte_count = 0u;

    uint32_t uiTestFileSize;
    uint32_t buffer_size;
    uint32_t bytes_remaining = 0u;
    uint8_t *pbyFileData;
    uint32_t count = 0;

    /* Calculate the test file size - always a multiple of four bytes */
    fTestFileSize = fTestFileSize * 1024 * 1024 * (pMsTest->iTestSize + 1);

    /* Limit to a maximum size */
    if (fTestFileSize > (1024 * 1024 * 20))
    {
        fTestFileSize = (1024 * 1024 * 20);
    }

    /* Limit to a minimum file size */
    if (fTestFileSize < sizeof(uint32_t))
    {
        fTestFileSize = sizeof(uint32_t);

    }
    uiTestFileSize = (uint32_t) (fTestFileSize);
    uiTestFileSize += (uiTestFileSize % sizeof(uint32_t));
    bytes_remaining = uiTestFileSize;

    /* Make a file name */
    cgiMakeWRTestFileName(pMsTest->pszFileName, fTestFileSize, pMsTest->chDrive);

    if (uiTestFileSize > MAX_BUFFER_SIZE_PRV_)
    {
        buffer_size = MAX_BUFFER_SIZE_PRV_;
    }
    else
    {
        buffer_size = uiTestFileSize;
    }

    /* Allocate the data for the file. */
    pbyFileData = (uint8_t *) R_OS_AllocMem((size_t) buffer_size, R_REGION_LARGE_CAPACITY_RAM);

    if (pbyFileData)
    {
        int iFile;

        /* Open the file for read & write */
        iFile = open(pMsTest->pszFileName, O_WRONLY | O_TRUNC, _IONBF);
        if (iFile > 0)
        {
            int iResult;
            uint32_t chunk_size;
            TMSTMP perfTimer;
            float write_time;

            /* Seek to the beginning of the file */
            {
                FILESEEK fileSeek;
                fileSeek.lOffset = 0L;
                fileSeek.iBase = SEEK_SET;
                control(iFile, CTL_FILE_SEEK, &fileSeek);
            }

            timerStartMeasurement( &perfTimer);

            do
            {
                if (bytes_remaining > MAX_BUFFER_SIZE_PRV_)
                {
                    chunk_size = MAX_BUFFER_SIZE_PRV_;
                }
                else
                {
                    chunk_size = bytes_remaining;
                }

                /* Fill with known data */
                cgiFillFileData((uint32_t *) pbyFileData, (size_t) (chunk_size / sizeof(uint32_t)));

                /* write the data */
                iResult = write(iFile, pbyFileData, chunk_size);
                if (iResult >= 0)
                {
                    bytes_remaining -= (uint32_t) iResult;
                }
                count++;
                byte_count += (uint32_t) iResult;
            } while ((bytes_remaining > 0) && (iResult >= 0));

            /* Stop the timer */
            ptimerStopMeasurement( &perfTimer, &write_time);

            if (iResult >= 0)
            {
                _Bool data_validated = true;
                bytes_remaining = uiTestFileSize;
                float read_time;

                close(iFile);

                /* Print the data rate */
                cgiShowDataRate(pMsTest->pszWriteSpeed, write_time, (size_t) byte_count,
                true);

                byte_count = 0u;

                /* open the file for reading */
                iFile = open(pMsTest->pszFileName, O_RDONLY, _IONBF);

                /* Seek to the beginning of the file */
                {
                    FILESEEK fileSeek;
                    fileSeek.lOffset = 0L;
                    fileSeek.iBase = SEEK_SET;
                    control(iFile, CTL_FILE_SEEK, &fileSeek);
                }

                /* Initialise the file buffer memory */
                memset(pbyFileData, 0, (size_t) buffer_size);

                /* Read the data back in */
                timerStartMeasurement( &perfTimer);

                do
                {
                    if (bytes_remaining > MAX_BUFFER_SIZE_PRV_)
                    {
                        chunk_size = MAX_BUFFER_SIZE_PRV_;
                    }
                    else
                    {
                        chunk_size = bytes_remaining;
                    }

                    iResult = read(iFile, pbyFileData, chunk_size);

                    if (iResult > 0)
                    {
                        /* only output the first failed data match */
                        if (data_validated)
                        {
                            if ( !cgiCheckFileData((uint32_t *) pbyFileData, (size_t) (chunk_size / sizeof(uint32_t))))
                            {
                                data_validated = false;
                            }
                        }

                        bytes_remaining -= (uint32_t) iResult;
                        byte_count += (uint32_t) iResult;
                    }
                } while ((bytes_remaining > 0) && (iResult >= 0));

                ptimerStopMeasurement( &perfTimer, &read_time);

                if (iResult >= 0)
                {
                    /* Print the data rate */
                    cgiShowDataRate(pMsTest->pszReadSpeed, read_time, (size_t) byte_count, false);

                    /* Check that the data is as expected */
                    if (data_validated)
                    {
                        sprintf(pMsTest->pszTestResult, "File data OK\r\n");
                    }
                    else
                    {
                        sprintf(pMsTest->pszTestResult, "Error in file data\r\n");
                    }
                }
                else
                {
                    sprintf(pMsTest->pszTestResult, "Error in read (%d)\r\n", errno);
                }
            }
            else
            {
                sprintf(pMsTest->pszTestResult, "Error in write (%d) %d %ld\r\n", errno, iResult, count);
            }

            close(iFile);
        }
        else
        {
            sprintf(pMsTest->pszTestResult, "Failed to open file\r\n");
        }

        R_OS_FreeMem((void *) pbyFileData);
    }
    else
    {
        sprintf(pMsTest->pszTestResult, "Failed to allocate memory for the file data\r\n");
    }

    /* Show that the result strings have been set */
    pMsTest->bfResultsValid = true;
    /* Show that the test task has completed and another can be started */
    pMsTest->uiTaskID = NULL;

    R_OS_DeleteTask(NULL);
}
/******************************************************************************
 End of function  taskReadWritePerfTest
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdSearch
 Description:   Function to search the cache for a client IP address
 Parameters:    IN  pSess - Pointer to the session
 OUT pchDrive - Pointer to the drive
 OUT pszDir - Pointer to the directory path
 OUT piIndex - Pointer to the index
 OUT pDirSort - Pointer to the directoy sort
 Return value:  none
 *****************************************************************************/
static void bdSearch (PSESS pSess, char *pchDrive, char *pszDir, int *piIndex, PDSORT pDirSort)
{
    int iCount = BD_CLIENT_PATH_CACHE_SIZE;
    while (iCount--)
    {
        if (pSess->ws_client_ip.sin_addr.s_addr == gpClientPathCache[iCount].ulAddress)
        {
            *pchDrive = gpClientPathCache[iCount].chDrive;
            strcpy(pszDir, gpClientPathCache[iCount].pszDir);
            *piIndex = gpClientPathCache[iCount].iIndex;
            *pDirSort = gpClientPathCache[iCount].dirSort;
            gpClientPathCache[iCount].ulLRU++;
            break;
        }
    }
}
/*****************************************************************************
 End of function  bdSearch
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCache
 Description:   Function to cache the drive and path
 Parameters:    IN  pSess - Pointer to the session
 IN  pchDrive - Pointer to the cached drive
 IN  pszDir - Pointer to the chached directory path
 IN  piIndex - Pointer to the index within the directory
 IN  pDirSort - Pointer to the directory sort type
 Return value:  none
 *****************************************************************************/
static void bdCache (PSESS pSess, char *pchDrive, char *pszDir, int *piIndex, PDSORT pDirSort)
{
    int iCount = BD_CLIENT_PATH_CACHE_SIZE;
    unsigned long ulLRU = -1UL;
    _Bool bfFound = false;
    int iLRU = 0;

    while (iCount--)
    {
        /* Look for the least recently used */
        if (gpClientPathCache[iCount].ulLRU <= ulLRU)
        {
            ulLRU = gpClientPathCache[iCount].ulLRU;
            iLRU = iCount;
        }

        /* If it has not yet been found */
        if ((!bfFound)
        /* And the client addresses match */
        && (pSess->ws_client_ip.sin_addr.s_addr == gpClientPathCache[iCount].ulAddress))
        {
            gpClientPathCache[iCount].chDrive = *pchDrive;
            strncpy(gpClientPathCache[iCount].pszDir, pszDir,
            BD_CLIENT_PATH_LENGTH);
            gpClientPathCache[iCount].iIndex = *piIndex;
            gpClientPathCache[iCount].dirSort = *pDirSort;
            bfFound = true;
        }
    }

    /* If the client address was not found */
    if (!bfFound)
    {
        /* Save in the least recently used cache entry */
        gpClientPathCache[iLRU].chDrive = *pchDrive;
        strncpy(gpClientPathCache[iLRU].pszDir, pszDir, BD_CLIENT_PATH_LENGTH);
        gpClientPathCache[iLRU].ulAddress = pSess->ws_client_ip.sin_addr.s_addr;
        gpClientPathCache[iLRU].iIndex = *piIndex;
        gpClientPathCache[iLRU].dirSort = *pDirSort;
    }
}
/*****************************************************************************
 End of function  bdCache
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdSetArgumentFlags
 Description:   Function to set the flags based on the argument
 Parameters:    IN  pszArgument - Pointer to the argument string
 OUT pHtDir - Pointer to html dir data
 OUT pSortSelect - Pointer to the sort selection type
 Return value:  none
 *****************************************************************************/
static void bdSetArgumentFlags (char *pszArgument, PHTDIR pHtDir, PSESRT pSortSelect)
{
    /* Set the flags for the previous and next functions */
    if (cgiArgument(pszArgument, "nx"))
    {
        pHtDir->bfGetNext = true;
    }
    if (cgiArgument(pszArgument, "pv"))
    {
        pHtDir->bfGetPrev = true;
    }
    if (cgiArgument(pszArgument, "na"))
    {
        *pSortSelect = DIR_SORT_BY_NAME;
    }
    if (cgiArgument(pszArgument, "cr"))
    {
        *pSortSelect = DIR_SORT_BY_DATE;
    }
    if (cgiArgument(pszArgument, "sz"))
    {
        *pSortSelect = DIR_SORT_BY_SIZE;
    }
}
/*****************************************************************************
 End of function  bdSetArgumentFlags
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdModifySortType
 Description:   Function to modify the sort type
 Parameters:    IN  dirSort - The directoy sort type
 IN  sortSelect - The sort select type
 Return value:  The new sort type
 *****************************************************************************/
static DSORT bdModifySortType (DSORT dirSort, SESRT sortSelect)
{
    switch (sortSelect)
    {
        case DIR_SORT_BY_NAME:
            if (dirSort == DIR_SORT_BY_NAME_A_Z)
            {
                dirSort = DIR_SORT_BY_NAME_Z_A;
            }
            else
            {
                dirSort = DIR_SORT_BY_NAME_A_Z;
            }
        break;

        case DIR_SORT_BY_DATE:
            if (dirSort == DIR_SORT_BY_DATE_RECENT)
            {
                dirSort = DIR_SORT_BY_DATE_OLD;
            }
            else
            {
                dirSort = DIR_SORT_BY_DATE_RECENT;
            }
        break;

        case DIR_SORT_BY_SIZE:
            if (dirSort == DIR_SORT_BY_SIZE_LARGE)
            {
                dirSort = DIR_SORT_BY_SIZE_SMALL;
            }
            else
            {
                dirSort = DIR_SORT_BY_SIZE_LARGE;
            }
        break;
        default:

        break;


    }
    return dirSort;
}
/*****************************************************************************
 End of function  bdModifySortType
 ******************************************************************************/

/******************************************************************************
 Function Name: bdAddEntry
 Description:   Function to add an entry to the directory list
 Arguments:     IN  ppDLST - Pointer to the directory list pointer to add the
 entry to
 IN  pDLST - Pointer to the entry to add
 IN  pfnCompare - Pointer to a function to compare the new entry
 with the current item on the list
 Return value:  none
 ******************************************************************************/
static void bdAddEntry (PDLST *ppDLST, PDLST pDLST, PDCMP pfnCompare)
{
    while ((*ppDLST) && (pfnCompare(*ppDLST, pDLST)))
    {
        ppDLST = &(*ppDLST)->pNext;
    }

    pDLST->pNext = *ppDLST;
    *ppDLST = pDLST;
}
/******************************************************************************
 End of function  bdAddEntry
 ******************************************************************************/

/******************************************************************************
 Function Name: bdRemoveEntry
 Description:   Function to remove an entry from the list
 Arguments:     IN  ppDLST - Pointer to the directory list pointer to remove the
 entry from
 IN  pDLST - Pointer to the transfer request to remove
 Return value:  none
 ******************************************************************************/
static void bdRemoveEntry (PDLST *ppDLST, PDLST pDLST)
{
    /* Search the list for the request */
    while ((*ppDLST != NULL) && (*ppDLST != pDLST))
    {
        ppDLST = &( *ppDLST)->pNext;
    }

    /* If it has been found */
    if (*ppDLST)
    {
        /* Delete from the list */
        *ppDLST = (*ppDLST)->pNext;
        pDLST->pNext = NULL;
    }
}
/******************************************************************************
 End of function  bdRemoveEntry
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareNameAZ
 Description:   Function to perform the DIR_SORT_BY_NAME_A_Z sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareNameAZ (PDLST pDLST, PDLST pNew)
{
    if (stricmp(pDLST->fatEntry.FileName, pNew->fatEntry.FileName) < 0)
    {
        return true;
    }

    return false;
}
/*****************************************************************************
 End of function  bdCompareNameAZ
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareNameZA
 Description:   Function to perform the DIR_SORT_BY_NAME_Z_A sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareNameZA (PDLST pDLST, PDLST pNew)
{
    if (stricmp(pDLST->fatEntry.FileName, pNew->fatEntry.FileName) > 0)
    {
        return true;
    }

    return false;
}
/*****************************************************************************
 End of function  bdCompareNameZA
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareDate
 Description:   Function to compare the date
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  signed representation of the difference in date
 *****************************************************************************/
float bdCompareDate (PDLST pDLST, PDLST pNew)
{
    DATE List, New;
    List.Field.Year = pDLST->fatEntry.CreateTime.Year;
    List.Field.Month = (unsigned) (pDLST->fatEntry.CreateTime.Month & 0xf);
    List.Field.Day = (unsigned) (pDLST->fatEntry.CreateTime.Day & 0x1f);
    List.Field.Hour = (unsigned) (pDLST->fatEntry.CreateTime.Hour & 0x1f);
    List.Field.Minute = (unsigned) (pDLST->fatEntry.CreateTime.Minute & 0x3f);
    List.Field.Second = (unsigned) (pDLST->fatEntry.CreateTime.Second & 0x3f);
    List.Field.WeekDay = 0;
    New.Field.Year = pNew->fatEntry.CreateTime.Year;
    New.Field.Month = (unsigned) (pNew->fatEntry.CreateTime.Month & 0xf);
    New.Field.Day = (unsigned) (pNew->fatEntry.CreateTime.Day & 0x1f);
    New.Field.Hour = (unsigned) (pNew->fatEntry.CreateTime.Hour & 0x1f);
    New.Field.Minute = (unsigned) (pNew->fatEntry.CreateTime.Minute & 0x3f);
    New.Field.Second = (unsigned) (pNew->fatEntry.CreateTime.Second & 0x3f);
    New.Field.WeekDay = 0;
    return (float) (List.llCode - New.llCode);
}
/*****************************************************************************
 End of function  bdCompareDate
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareDateRecent
 Description:   Function to perform the DIR_SORT_BY_DATE_RECENT sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareDateRecent (PDLST pDLST, PDLST pNew)
{
    if (bdCompareDate(pDLST, pNew) > 0)
    {
        return false;
    }

    return true;
}
/*****************************************************************************
 End of function  bdCompareDateRecent
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareDateOld
 Description:   Function to perform the DIR_SORT_BY_DATE_OLD sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareDateOld (PDLST pDLST, PDLST pNew)
{
    if (bdCompareDate(pDLST, pNew) < 0)
    {
        return false;
    }

    return true;
}
/*****************************************************************************
 End of function  bdCompareDateOld
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareSizeLarge
 Description:   Function to perform the DIR_SORT_BY_SIZE_LARGE sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareSizeLarge (PDLST pDLST, PDLST pNew)
{
    if (pDLST->fatEntry.Filesize > pNew->fatEntry.Filesize)
    {
        return true;
    }

    return false;
}
/*****************************************************************************
 End of function  bdCompareSizeLarge
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdCompareSizeSmall
 Description:   Function to perform the DIR_SORT_BY_SIZE_SMALL sort
 Parameters:    IN  pDLST - Pointer to the current list entry
 IN  pNew - Pointer to the new entry
 Return value:  false when the new entry should be inserted into the list
 *****************************************************************************/
static _Bool bdCompareSizeSmall (PDLST pDLST, PDLST pNew)
{
    if (pDLST->fatEntry.Filesize < pNew->fatEntry.Filesize)
    {
        return true;
    }

    return false;
}
/*****************************************************************************
 End of function  bdCompareSizeSmall
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdAddToList
 Description:   Function to add an enty to the list
 Parameters:    IN  ppDLST - Pointer to the list to add the entry to
 IN  pFatEntry - Pointer to the FAT entry to add
 IN  dirSort - The directory sorting method
 Return value:  pointer to the new entry on success. NULL on error
 *****************************************************************************/
static PDLST bdAddToList (PDLST *ppDLST, PFATENTRY pFatEntry, DSORT dirSort)
{
    PDLST pDLST = (PDLST) malloc(sizeof(DLST));
    if (pDLST)
    {
        static const PDCMP ppfnCompare[DIR_SORT_NUM] =
        {
        /* DIR_SORT_BY_NAME_A_Z */
        bdCompareNameAZ,
        /* DIR_SORT_BY_NAME_Z_A */
        bdCompareNameZA,
        /* DIR_SORT_BY_DATE_RECENT */
        bdCompareDateRecent,
        /* DIR_SORT_BY_DATE_OLD */
        bdCompareDateOld,
        /* DIR_SORT_BY_SIZE_LARGE */
        bdCompareSizeLarge,
        /* DIR_SORT_BY_SIZE_SMALL */
        bdCompareSizeSmall };
        /* Take a copy of the FAT entry */
        pDLST->fatEntry = *pFatEntry;
        /* Add to the appropriate place in the list */
        bdAddEntry(ppDLST, pDLST, ppfnCompare[dirSort]);
        return pDLST;
    }

    return NULL;
}
/*****************************************************************************
 End of function  bdAddToList
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdFreeList
 Description:   Function to free the list
 Parameters:    IN  ppDLST - Pointer to the list to add the entry to
 Return value:  none
 *****************************************************************************/
static void bdFreeList (PDLST *ppDLST)
{
    while (*ppDLST)
    {
        PDLST pFree = *ppDLST;
        bdRemoveEntry(ppDLST, pFree);
        free(pFree);
    }
}
/*****************************************************************************
 End of function  bdFreeList
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdFormatDate
 Description:   Function to format the date string
 Parameters:    OUT pszDest - Pointer to the destination string
 IN  pFatEntry - Pointer to the entry
 Return value:  none
 *****************************************************************************/
static void bdFormatDate (char *pszDest, PFATENTRY pFatEntry)
{
    static const char * const ppszMonth[] =
    { "---", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    sprintf(pszDest, "%.2d-%s-%.4d %.2d:%.2d", pFatEntry->CreateTime.Day, ppszMonth[pFatEntry->CreateTime.Month],
            pFatEntry->CreateTime.Year, pFatEntry->CreateTime.Hour, pFatEntry->CreateTime.Minute);
}
/*****************************************************************************
 End of function  bdFormatDate
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdChangeDirectoryAction
 Description:   Function to format the button text required for a directory
 change
 Parameters:    OUT pszDest - Pointer to the destination string
 IN  chDrive - The driver letter
 IN  pszDir - The directory path
 IN  dirSort - The list sort method
 Return value:  none
 *****************************************************************************/
static void bdChangeDirectoryAction (char *pszDest, char chDrive, char *pszDir, DSORT dirSort)
{
    sprintf(pszDest, " onclick=\"callFunc('ms_explore', 'ms_explore.cgi'"
            ", 'cd,%c:%s,%d')\"", chDrive, pszDir, dirSort);
}
/*****************************************************************************
 End of function  bdChangeDirectoryAction
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdDirectoryIndexAction
 Description:   Function to format the button text requires for a page index
 position change
 Parameters:    OUT pszDest - Pointer to the destination string
 IN  chDrive - The driver letter
 IN  pszDir - The directory path
 IN  dirSort - The list sort method
 IN  iIndex - The current index position
 IN  bfPrevious - true for previous false for next
 IN  bfEnabled - true if enabled
 Return value:  none
 *****************************************************************************/
static void bdDirectoryIndexAction (char *pszDest, char chDrive, char *pszDir, DSORT dirSort, int iIndex,
        _Bool bfPrevious, _Bool bfEnabled)
{
    static const char * const ppszPrevNext[] =
    { "nx", "pv" };
    static const char * const ppszLabel[] =
    { "   Next   ", " Previous " };
    char *pszEnabled = "";

    if (!bfEnabled)
    {
        pszEnabled = " disabled=true";
    }

    sprintf(pszDest, "<input type=\"button\" value=\"%s\" onclick=\""
            "callFunc('ms_explore', 'ms_explore.cgi', '%s,%c:%s,%d,%d')\"%s/>", ppszLabel[bfPrevious],
            ppszPrevNext[bfPrevious], chDrive, pszDir, dirSort, iIndex, pszEnabled);
}
/*****************************************************************************
 End of function  bdDirectoryIndexAction
 ******************************************************************************/

/*****************************************************************************
 Function Name: bgParentDirectory
 Description:   Function to format the parent directory link
 Parameters:    IN  pHtDir - Pointer to html directory information
 IN  pszDate - Pointer to the date string
 Return value:  0 for success otherwize error code
 *****************************************************************************/
static void bgParentDirectory (PHTDIR pHtDir, char *pszDate)
{
    char *pszScan;

    /* Need to remove the end folder "/Directory/Path/" */
    strcpy(gpszTemp1, pHtDir->pszDir);
    pszScan = gpszTemp1 + (strlen(gpszTemp1) - 1);

    /* If it is not the root directory */
    if (pszScan != gpszTemp1)
    {
        /* Search for the next \ from the end & terminate the string to make
         the parent file path  "/Directory" */
        while (pszScan >= gpszTemp1)
        {
            if (( *pszScan == '/') || ( *pszScan == '\\'))
            {
                *pszScan = '\0';
                break;
            }

            pszScan--;
        }
    }

    /* In the case of working back to the root */
    if (strlen(gpszTemp1) == 0)
    {
        /* Set back to the root */
        strcpy(gpszTemp1, "/");
    }

    /* Make the string URL safe */
    cgiEncodeString(gpszTemp2, sizeof(gpszTemp2), gpszTemp1);

    /* Format the button click message */
    bdChangeDirectoryAction(gpszTemp1, pHtDir->chDrive, gpszTemp2, pHtDir->dirSort);

    /* Format  parent directory line */
    wi_printf(pHtDir->pSess, gcpszParentDirectory, gpszTemp1, pHtDir->stFileNamePadding - 15, gpszPad, pszDate);
}
/*****************************************************************************
 End of function  bgParentDirectory
 ******************************************************************************/

/*****************************************************************************
 Function Name: bgAttachedDrives
 Description:   Function to format the drive bar at the top of the page
 Parameters:    IN  pHtDir - Pointer to html directory information
 Return value:  none
 *****************************************************************************/
static void bgAttachedDrives (PHTDIR pHtDir)
{
    char chDrive = 'A';

    while (chDrive <= 'Z')
    {
        PDRIVE pDrive = dskGetDrive(chDrive);

        if (pDrive)
        {
            if (chDrive == pHtDir->chDrive)
            {
                wi_printf(pHtDir->pSess, gcpszActiveDrive, chDrive, pHtDir->dirSort, chDrive);
            }
            else
            {
                wi_printf(pHtDir->pSess, gcpszAvailableDrive, chDrive, pHtDir->dirSort, chDrive);
            }
        }

        chDrive++;
    }
}
/*****************************************************************************
 End of function  bgAttachedDrives
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdEntry
 Description:   Function to format the file or directory index
 Parameters:    IN  pHtDir - Pointer to html directory information
 IN  pFatEntry - Pointer to the FAT entry to print
 Return value:  none
 *****************************************************************************/
static void bdEntry (PHTDIR pHtDir, PFATENTRY pFatEntry)
{
    char pszDate[32];
    char pszSize[32];
    char *pszImage;
    int iPad;

    /* Format the date string */
    bdFormatDate(pszDate, pFatEntry);

    if (pFatEntry->Attrib & FAT_ATTR_DIR)
    {
        pszImage = "folder";
        strcpy(pszSize, "           -");

        /* Make the new directory path */
        strcpy(gpszTemp1, pHtDir->pszEncodedPath);

        /* If there is not a / on the end */
        if (*(gpszTemp1 + (strlen(gpszTemp1) - 1)) != '/')
        {
            /* Add one */
            strcat(gpszTemp1, "/");
        }

        /* Add the file name on the end */
        cgiEncodeString(gpszTemp2, sizeof(gpszTemp2), pFatEntry->FileName);
        strcat(gpszTemp1, gpszTemp2);

        /* Set the onClick function for the change directory command */
        bdChangeDirectoryAction(gpszTemp2, pHtDir->chDrive, gpszTemp1, pHtDir->dirSort);

        /* The link becomes # */
        strcpy(gpszTemp1, "#");
    }
    else
    {
        pszImage = "page_white_text";
        cgiFileSize(pszSize, (float) pFatEntry->Filesize);

        /* If the last character of the path is not a slash then add one */
        strcpy(gpszTemp1, pHtDir->pszDir);
        if (*(gpszTemp1 + (strlen(gpszTemp1) - 1)) != '/')
        {
            strcat(gpszTemp1, "/");
        }

        strcat(gpszTemp1, pFatEntry->FileName);

        /* Encode the file name for the link */
        cgiEncodeString(gpszTemp2, sizeof(gpszTemp2), gpszTemp1);

        /* Format a link to the file */
        sprintf(gpszTemp1, "/%c:%s", pHtDir->chDrive, gpszTemp2);

        /* There is no onClick function */
        gpszTemp2[0] = '\0';
    }

    /* Calculate the padding */
    iPad = (int) (pHtDir->stFileNamePadding - strlen(pFatEntry->FileName));

    /* Make sure that any strange characters in the file name are replaced with a printable one */
    cgiReplaceUnprintableChars(pFatEntry->FileName, '#');

    /* Format the entry */
    wi_printf(pHtDir->pSess, gpszEntry, pszImage, gpszTemp1, gpszTemp2, pFatEntry->FileName, iPad, gpszPad, pszDate,
            pszSize);
}
/*****************************************************************************
 End of function  bdEntry
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdGetDirectoryDate
 Description:   Function to get the directory date stamp
 Parameters:    IN  pHtDir - Pointer to html directory information
 OUT pszDirDate - Pointer to the directory date
 Return value:  none
 *****************************************************************************/
static void bdGetDirectoryDate (PHTDIR pHtDir, char *pszDirDate)
{
    char *pszSplit;

    strcpy(gpszTemp1, pHtDir->pszDir);
    pszSplit = gpszTemp1 + strlen(gpszTemp1);

    while (pszSplit >= gpszTemp1)
    {
        if ((*pszSplit == '/') || (*pszSplit == '\\'))
        {
            char *pszRootDir = gpszTemp1;

            /* Split the file directory from the path */
            *pszSplit = '\0';
            pszSplit++;

            if (!*pszRootDir)
            {
                pszRootDir = "/";
            }

            /* Find the directory */
            if (!R_FAT_FindFirst(&gs_dir, &(pHtDir->fatEntry), pszRootDir, "*"))
            {
                while (true)
                {
                    if (!R_FAT_FindNext( &gs_dir, &(pHtDir->fatEntry)))
                    {
                        if (!strcmp(pszSplit, (pHtDir->fatEntry).FileName))
                        {
                            bdFormatDate(pszDirDate, &(pHtDir->fatEntry));
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            break;
        }
        pszSplit--;
    }
}
/*****************************************************************************
 End of function  bdGetDirectoryDate
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdBuildDirectoryList
 Description:   Function to build a
 Parameters:    IN  pHtDir - Pointer to html directory information
 Return value:  0 for success or -1 on error
 *****************************************************************************/
static int bdBuildDirectoryList (PHTDIR pHtDir)
{
    int iResult = 0;
    int first_entry = 1;

    /* Build the list of the entire directory */
    while (true)
    {
        /* only continue if this is the first entry or an object was previously found and no error */
        if (( !pHtDir->fatError) && ((0 != pHtDir->fatEntry.FileName[0]) || first_entry))
        {
            first_entry = 0;

            /* Skip the references to the parent directory */
            if ((strcmp(pHtDir->fatEntry.FileName, ".")) && (strcmp(pHtDir->fatEntry.FileName, ".."))

            /* Skip if the file is hidden */
            && (!(pHtDir->fatEntry.Attrib & FAT_ATTR_HIDDEN))

            /* Skip if it is a system file */
            && (!(pHtDir->fatEntry.Attrib & FAT_ATTR_SYSTEM)))
            {
                if (pHtDir->fatEntry.Attrib & FAT_ATTR_DIR)
                {
                    DSORT l_dir_sort;

                    /* Directories can't be sorted any other way than alpha forward or reverse */
                    l_dir_sort = (pHtDir->dirSort > DIR_SORT_BY_NAME_Z_A) ? DIR_SORT_BY_NAME_A_Z : pHtDir->dirSort;
                    if (!bdAddToList(&pHtDir->pDirList, &pHtDir->fatEntry, l_dir_sort))
                    {
                        /* Out of memory */
                        iResult = -1;
                        break;
                    }
                }
                else
                {
                    if (!bdAddToList(&pHtDir->pFileList, &pHtDir->fatEntry, pHtDir->dirSort))
                    {
                        /* Out of memory */
                        iResult = -1;
                        break;
                    }
                }
            }
        }
        else
        {
            pHtDir->fatError = 0;
            break;
        }

        /* find next file / directory */
        pHtDir->fatError = R_FAT_FindNext(&gs_dir, &pHtDir->fatEntry);
    }
    return iResult;
}
/*****************************************************************************
 End of function  bdBuildDirectoryList
 ******************************************************************************/

/*****************************************************************************
 Function Name: bgDirectory
 Description:   Function to format the directoy listing itself
 Parameters:    IN  pHtDir - Pointer to html directory information
 IN  iIndex - The index of the firs entry to list
 IN  iEntryCount - The number of entries to print
 Return value:  true if a next button is reqired
 *****************************************************************************/
static _Bool bgDirectory (PHTDIR pHtDir, int iIndex, int iEntryCount)
{
    _Bool bfNext = false;
    PDLST pSearch;

    /* Go through the directory list */
    pSearch = pHtDir->pDirList;
    while (pSearch)
    {
        /* Don't print until the index is zero */
        if (!iIndex)
        {
            /* Count the number of entries that need to be shown */
            if (iEntryCount)
            {
                /* Count the number of entries */
                iEntryCount--;

                /* Format this entry */
                bdEntry(pHtDir, &pSearch->fatEntry);
            }
            else
            {
                /* There are more entries and a next button is required */
                bfNext = true;
                break;
            }
        }
        else
        {
            /* Count until desired position in the directory has been reached */
            iIndex--;
        }

        pSearch = pSearch->pNext;
    }

    if (iEntryCount)
    {
        /* Go through the file list */
        pSearch = pHtDir->pFileList;

        while (pSearch)
        {
            /* Don't print until the index is zero */
            if (!iIndex)
            {
                /* Count the number of entries that need to be shown */
                if (iEntryCount)
                {
                    /* Count the number of entries */
                    iEntryCount--;

                    /* Format this entry */
                    bdEntry(pHtDir, &pSearch->fatEntry);
                }
                else
                {
                    /* There are more entries and a next button is required */
                    bfNext = true;
                    break;
                }
            }
            else
            {
                /* Count until desired position in the directory has been reached */
                iIndex--;
            }

            pSearch = pSearch->pNext;
        }
    }

    /* Return the next button status */
    return bfNext;
}
/*****************************************************************************
 End of function  bgDirectory
 ******************************************************************************/

/*****************************************************************************
 Function Name: bgGetMaxNameLength
 Description:   Function to get the maximum name length
 Parameters:    IN  pHtDir - Pointer to html directory information
 IN  iIndex - The desired position in the list
 IN  iEntryCount - The number of entrys to check
 Return value:  The maximum name length
 *****************************************************************************/
static size_t bgGetMaxNameLength (PHTDIR pHtDir, int iIndex, int iEntryCount)
{
    size_t stMaxNameLength = 0;
    PDLST pSearch;

    /* Go through the directory list */
    pSearch = pHtDir->pDirList;

    while (pSearch)
    {
        /* Don't print until the index is zero */
        if (!iIndex)
        {
            /* Count the number of entries that need to be shown */
            if (iEntryCount)
            {
                /* Check the length of this entry */
                size_t stLength = strlen(pSearch->fatEntry.FileName);

                if (stLength > stMaxNameLength)
                {
                    stMaxNameLength = stLength;
                }

                /* Count the number of entries */
                iEntryCount--;
            }
            else
            {
                break;
            }
        }
        else
        {
            /* Count until desired position in the directory has been reached */
            iIndex--;
        }

        pSearch = pSearch->pNext;
    }

    if (iEntryCount)
    {
        /* Go through the file list */
        pSearch = pHtDir->pFileList;
        while (pSearch)
        {
            /* Don't print until the index is zero */
            if (!iIndex)
            {
                /* Count the number of entries that need to be shown */
                if (iEntryCount)
                {
                    /* Check the length of this entry */
                    size_t stLength = strlen(pSearch->fatEntry.FileName);
                    if (stLength > stMaxNameLength)
                    {
                        stMaxNameLength = stLength;
                    }

                    /* Count the number of entries */
                    iEntryCount--;
                }
                else
                {
                    break;
                }
            }
            else
            {
                /* Count until desired position in the directory has been
                 reached */
                iIndex--;
            }

            pSearch = pSearch->pNext;
        }
    }

    /* Return the next button status */
    return stMaxNameLength;
}
/*****************************************************************************
 End of function  bgGetMaxNameLength
 ******************************************************************************/

/*****************************************************************************
 Function Name: bdGeneratePage
 Description:   Function to generate the directory listing page
 Parameters:    IN  pHtDir - Pointer to html directory information
 Return value:  -1 on failure
 *****************************************************************************/
static int bdGeneratePage (PHTDIR pHtDir)
{
    pHtDir->pDrive = dskGetDrive(pHtDir->chDrive);
    if (pHtDir->pDrive)
    {
        char pszDirDate[32];
        size_t stMaxName;
        _Bool bfPrevious = false;
        _Bool bfNext = false;
        _Bool bfRoot = true;

        int iReady = R_FAT_DriveIsAvailable(pHtDir->pDrive);

        /* Print the header */
        wi_printf(pHtDir->pSess, gcpszHeader);

        /* Print the drive list */
        bgAttachedDrives(pHtDir);

        /* Print the current drive and folder */
        wi_printf(pHtDir->pSess, gcpszCurrentFolder, pHtDir->chDrive, pHtDir->pszDir);

        if (iReady)
        {
            /* Get the details of the directory itself */
            pHtDir->fatError = R_FAT_FindFirst(&gs_dir, &pHtDir->fatEntry, pHtDir->pszDir, "*");

            /* Make the data string for the directory */
            bdFormatDate(pszDirDate, &pHtDir->fatEntry);

            /* Build a list of the directory */
            bdBuildDirectoryList(pHtDir);
            /* Check to see if this is the root folder - in which case
             there is no need for the parent directory entry in the listing */
            if (khanCompare((const int8_t *) "/", (const int8_t *) pHtDir->pszDir, 1))
            {
                bfRoot = false;
                pHtDir->iEntryCount--;
                /* Check for the previous button */
                if (pHtDir->iIndex > (BD_MAX_FILE_ENTRIES - 1))
                {
                    bfPrevious = true;
                }
            }

            /* Adjust the index if there is a page change request */
            if (pHtDir->bfGetNext)
            {
                pHtDir->iIndex += pHtDir->iEntryCount;
            }

            if (pHtDir->bfGetPrev)
            {
                pHtDir->iIndex -= pHtDir->iEntryCount;
                /* Make sure it does not go -ve */
                pHtDir->iIndex = (pHtDir->iIndex < 0) ? 0 : pHtDir->iIndex;
            }

            /* Check for the previous button */
            if (pHtDir->iIndex >= pHtDir->iEntryCount)
            {
                bfPrevious = true;
            }

            /* Look at the length of the file names on the list */
            stMaxName = bgGetMaxNameLength(pHtDir, pHtDir->iIndex, pHtDir->iEntryCount);
            if (stMaxName < BD_MIN_FILE_NAME_PADDIND)
            {
                pHtDir->stFileNamePadding = BD_MIN_FILE_NAME_PADDIND;
            }
            else
            {
                pHtDir->stFileNamePadding = stMaxName;
            }
        }
        else
        {
            pHtDir->stFileNamePadding = BD_MIN_FILE_NAME_PADDIND;

            /* Set the FAT error so as not to print the directory listing */
            pHtDir->fatError = 1;
        }

        /* Print the directory listing table header */
        wi_printf(pHtDir->pSess, gcpszTableHeader, pHtDir->chDrive, pHtDir->pszEncodedPath, pHtDir->dirSort,
                pHtDir->iIndex, pHtDir->stFileNamePadding + 7, gpszPad, pHtDir->chDrive, pHtDir->pszEncodedPath,
                pHtDir->dirSort, pHtDir->iIndex, pHtDir->chDrive, pHtDir->pszEncodedPath, pHtDir->dirSort,
                pHtDir->iIndex);

        /* If we found the directory then format the entries */
        if (!pHtDir->fatError)
        {
            /* If this is not the root directory */
            if (!bfRoot)
            {
                /* Find the date for the parent directory */
                bdGetDirectoryDate(pHtDir, pszDirDate);

                /* Print the parent directory entry */
                bgParentDirectory(pHtDir, pszDirDate);
            }

            /* Cache the clients drive and path */
            bdCache(pHtDir->pSess, &pHtDir->chDrive, pHtDir->pszDir, &pHtDir->iIndex, &pHtDir->dirSort);
            /* Print the files and folders */
            bfNext = bgDirectory(pHtDir, pHtDir->iIndex, pHtDir->iEntryCount);
        }
        else
        {
            if (iReady)
            {
                wi_printf(pHtDir->pSess, "    No files found.\r\n");
            }
            else
            {
                wi_printf(pHtDir->pSess, "    The information about this drive is currently being gathered.\r\n");
            }
        }

        /* Format the onclick script for the previous button */
        bdDirectoryIndexAction(gpszTemp1, pHtDir->chDrive, pHtDir->pszEncodedPath, pHtDir->dirSort, pHtDir->iIndex,
                true, bfPrevious);

        /* Format the onclick script for the next button */
        bdDirectoryIndexAction(gpszTemp2, pHtDir->chDrive, pHtDir->pszEncodedPath, pHtDir->dirSort, pHtDir->iIndex,
                false, bfNext);

        /* Print the footer to the directory table */
        wi_printf(pHtDir->pSess, gcpszTableFooter, gpszTemp1, gpszTemp2);

        /* Free the directory listing */
        bdFreeList(&pHtDir->pDirList);

        /* Free the directory listing */
        bdFreeList(&pHtDir->pFileList);
        return 0;
    }

    return -1;
}
/*****************************************************************************
 End of function  bdGeneratePage
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
