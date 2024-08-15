/***********************************************************************************************************************
* Copyright (c) 2012 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/******************************************************************************
* File Name    : efsFile.c
* Version      : 1.00
* Description  : Functions to get the file data from an encapsulate file
*                system binary file created by the EmbedFS utility
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

#include <efsFile.h>
#include <string.h>
#include <trace.h>
#include <stdbool.h>
#include <stddef.h>

/*****************************************************************************
Function Macros
******************************************************************************/

/* Comment this line out to turn ON module trace in this file */
#undef _TRACE_ON_

#ifndef _TRACE_ON_
#undef TRACE
#define TRACE(x)
#endif

/*****************************************************************************
Public Functions
******************************************************************************/

/*****************************************************************************
Function Name: efsFindFile
Description:   Function to get a file from a binary file created by the 
               EmbedFS utility
Arguments:     IN  pvBin - Pointer to the encapsulated file system
               IN  pszFilePathAndName - Pointer to the file path and name
               OUT pEfsFile - Pointer to the encapsulated file information
Return value:  0 for success or error code
*****************************************************************************/
EFSERR efsFindFile(void *pvBin, int8_t *pszFilePathAndName, PEFS pEfsFile)
{
    if (pvBin)
    {
        PEFILE  pfsFile = (PEFILE)(((int8_t*)pvBin) + sizeof(VERSION));
#ifdef _DEBUG_
        PVERSION    pVer = pvBin;
        if ((size_t)pvBin & 0x03)
        {
            TRACE(("efsFindFile: **Error: EFS_BINARY_ALIGNMENT_ERROR\r\n"));
            return EFS_BINARY_ALIGNMENT_ERROR;
        }
        if (pVer->ulEndianTag != 0x87654321UL)
        {
            TRACE(("efsFindFile: **Error: EFS_BINARY_ENDIAN_ERROR\r\n"));
            return EFS_BINARY_ENDIAN_ERROR;
        }
#endif
        /* Check for the root directory entry */
        if (strcmp("\\", (const char *) &pfsFile->szName) == 0)
        {
            /* Search the encapsulated file system for the file */
            return efsSearch(pfsFile, pszFilePathAndName, pEfsFile);
        }
    }
    return EFS_BINARY_NOT_FOUND;
}
/*****************************************************************************
End of function  efsFindFile
******************************************************************************/

/*****************************************************************************
Function Name: efsSearch
Description:   Function to search the encapsulated file system for a file
Arguments:     IN  pfsFile - Pointer to the encapsuated file system
               IN  pszFind - Pointer to the file path and name
               OUT pEfsFile - Pointer to the encapsulated file 
Return value:  0 for success or error code
*****************************************************************************/
EFSERR efsSearch(PEFILE pfsFile, int8_t *pszFind, PEFS pEfsFile)
{
    int8_t  *pszPath;
    size_t  stPathLength;
    int8_t  *pszFile;
    /* Split the string into the name and the path */
    efsSplitFileNameAndPath(pszFind, &pszPath, &stPathLength, &pszFile);
    /* If the file is not in the root directory */
    if (!efsIsRoot(pszPath, stPathLength))
    {
        /* Find the directory */
        pfsFile = efsSearchForDir(pfsFile, pszPath, stPathLength);
        if (!pfsFile)
        {
            return EFS_DIRECTORY_NOT_FOUND;
        }
    }
    /* Set the output file path to the directory passed to the file
       search function */
    pEfsFile->pszFilePath = &pfsFile->szName;
    /* Find the file */
    return efsSearchForFile(pfsFile, pszFile, pEfsFile);
}  //
/*****************************************************************************
End of function  efsSearch
******************************************************************************/

/*****************************************************************************
Function Name: efsSplit
Description:   Function to split a file name and path into the name and path
Arguments:     IN  pszFilePathAndName - Pointer to the file name and path
               OUT ppsPath - Pointer to the path pointer
               OUT pstPathLength - The length of the path
               OUT ppszFile - Pointer to the file name pointer
Return value:  none
*****************************************************************************/
void efsSplitFileNameAndPath(int8_t  *pszFilePathAndName,
                             int8_t  **ppsPath,
                             size_t  *pstPathLength,
                             int8_t  **ppszFile)
{
    int8_t *pszSlash = NULL;
    int8_t *pScan = pszFilePathAndName;
    /* Scan the string and note the position of the slash */
    while (*pScan)
    {
        if ((*pScan == '\\')
        ||  (*pScan == '/'))
        {
            pszSlash = pScan;
        }
        pScan++;
    }
    /* The last slash separates the path from the file name */
    if (pszSlash)
    {
        /* The first character of the file name starts after the slash */
        *ppszFile = (pszSlash + 1);
        *pstPathLength = (size_t)(*ppszFile - pszFilePathAndName);
    }
    else
    {
        /* No file path, just a file name */
        *ppszFile = pszFilePathAndName;
        *pstPathLength = 0UL;
    }
    /* Set the path pointer NULL is expected if there is a file name only */
    *ppsPath = pszFilePathAndName;
}
/*****************************************************************************
End of function  efsSplit
******************************************************************************/

/*****************************************************************************
Function Name: efsIsRoot
Description:   Function to return true if the directory is the root
Arguments:     IN  pszPath - Pointer to the file path
               IN  stLength - The length of the file path
Return value:  true if this is the root
*****************************************************************************/
_Bool efsIsRoot(int8_t *pszPath, size_t stLength)
{
    /* If there is no path - assume root */
    if (!stLength)
    {
        return true;
    }
    /* The path must be only one character long */
    else if (stLength == 1UL)
    {
        if (*pszPath == '\\')
        {
            return true;
        }
        if (*pszPath == '/')
        {
            return true;
        }
    }
    return false;
}
/*****************************************************************************
End of function  efsIsRoot
******************************************************************************/

/*****************************************************************************
Function Name: efsSearchForDir
Description:   Function to search the encapsulated file system for a directory
Arguments:     IN  pfsFile - Pointer to the directory
               IN  pszPath - Pointer to the directory name
               IN  stLength - The length of the directory path
Return value:  Pointer to the directory or NULL on error
*****************************************************************************/
PEFILE efsSearchForDir(PEFILE pfsFile,
                       int8_t *pszPath,
                       size_t stPathLength)
{
    /* Drop the first slash of the file path */
    if ((*pszPath == '\\')
    ||  (*pszPath == '/'))
    {
        pszPath++;
        stPathLength--;
    }
    /* The file name will be delimited from the path by a slash.
       We don't check the last slash */
    stPathLength--;
    /* Until the end of the list */
    while (pfsFile->fileHeader.ulNextOffset)
    {
        int8_t *pszName = &pfsFile->szName;
#ifdef _DEBUG_
        /* All directory entries have a zero data offset */
        if (pfsFile->fileHeader.ulDataOffset != 0)
        {
            TRACE(("efsSearchForDir: **Error in efs\r\n"));
            return NULL;
        }
#endif
        /* Drop the first slash of the name */
        if ((*pszName == '\\')
        ||  (*pszName == '/'))
        {
            pszName++;
        }
        /* Check for a matching path 
           - ignoring case and different slashes */
        if (!efsKhanCompare(pszPath, pszName, stPathLength))
        {
            return pfsFile;
        }
        /* Point at the next directory entry */
        pfsFile = (PEFILE)(((uint8_t*)pfsFile)
                + pfsFile->fileHeader.ulDataLength);
    }
    return NULL;
}
/*****************************************************************************
End of function  efsSearchForDir
******************************************************************************/

/*****************************************************************************
Function Name: efsSearchForFile
Description:   Function to search a directory for the file name
Arguments:     IN  pfsFile - Pointer to the directory
               IN  pszFile - Pointer to the file name
               OUT pEfsFile - Pointer to the file information
Return value:  0 for success or error code
*****************************************************************************/
EFSERR efsSearchForFile(PEFILE pfsFile,
                        int8_t *pszFile,
                        PEFS   pEfsFile)
{
    /* Check that the pointer passed is to a valid directory */
    if (pfsFile->fileHeader.ulDataOffset == 0)
    {
        /* Set the pointer to the end of the directory */
        PEFILE pDirEnd = (PEFILE)(((uint8_t*)pfsFile)
                       + pfsFile->fileHeader.ulDataLength);
        /* Point at the first file in the directory */
        pfsFile = (PEFILE)(((uint8_t*)pfsFile)
                + pfsFile->fileHeader.ulNextOffset);
        /* Search the entire directory */
        while (pfsFile < pDirEnd)
        {
            /* Check for a matching name */
            if (!efsStricmp(pszFile, &pfsFile->szName))
            {
                /* Set the file information */
                pEfsFile->pszFileName = &pfsFile->szName;
                pEfsFile->pbyFileData = (uint8_t *)(((uint8_t*)pfsFile)
                                      + pfsFile->fileHeader.ulDataOffset);
                pEfsFile->ulFileLength = pfsFile->fileHeader.ulDataLength;
                pEfsFile->bfDataAllocated = false;
                return EFS_OK;
            }
            /* Point at the next entry */
            pfsFile = (PEFILE)(((uint8_t*)pfsFile)
                    + pfsFile->fileHeader.ulNextOffset);
        }
    }
    return EFS_FILE_NOT_FOUND;
}
/*****************************************************************************
End of function  efsSearchForFile
******************************************************************************/

/*****************************************************************************
Function Name: efsStricmp
Description:   Function to compare two strings case insensitive
Arguments:     IN  pszS1 - Pointer to the first string
               IN  pszS2 - Pointer to the second string
Return value:  If the two strings are equivalent the return is zero.
*****************************************************************************/
int efsStricmp(const int8_t *pszS1, const int8_t *pszS2)
{
    while (((*pszS1) | 0x20) == ((*pszS2) | 0x20))
    {
        if (!*pszS1++)
        {
            return 0;
        }
        pszS2++;
    }
    return 1;
}
/*****************************************************************************
End of function  efsStricmp
******************************************************************************/

/*****************************************************************************
Function Name: efsKhanCompare
Description:   Function to perform a slash and case insensitive string compare
Arguments:     IN  pszS1 - Pointer to the first string
               IN  pszS2 - Pointer to the second string
               IN  stLengthS2 - The length of S1
Return value:  If the two strings are equivalent the return is zero.
*****************************************************************************/
int efsKhanCompare(const int8_t *pszS1,
                   const int8_t *pszS2,
                   size_t        stLengthS1)
{
    /* Until the end of either sting */
    while ((*pszS1) && (*pszS2))
    {
        /* Do a case insensitive compare */
        if (((*pszS1) | 0x20) != ((*pszS2) | 0x20))
        {
            if ((((*pszS1) == '\\') || ((*pszS1) == '/'))
            &&  (((*pszS2) == '\\') || ((*pszS2) == '/')))
            {
                /* Only different by the slash is OK */
            }
            else
            {
                break;
            }
        }
        /* Bump the pointers */
        pszS1++;
        pszS2++;
        stLengthS1--;
        /* Both strings must be the same length */
        if ((!*pszS2)
        &&  (!stLengthS1))
        {
            return 0;
        }
    }
    return 1;
}
/*****************************************************************************
End of function  efsKhanCompare
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
