/*******************************************************************************
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
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * @headerfile     efsFile.h
 * @brief          Functions to get the file data from an encapsulate file
*                  system binary file created by the EmbedFS utility
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A1LU
 *****************************************************************************/
 /*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *****************************************************************************/
/******************************************************************************
  WARNING!  IN ACCORDANCE WITH THE USER LICENCE THIS CODE MUST NOT BE CONVEYED
  OR REDISTRIBUTED IN COMBINATION WITH ANY SOFTWARE LICENSED UNDER TERMS THE
  SAME AS OR SIMILAR TO THE GNU GENERAL PUBLIC LICENCE
******************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef EFSFILE_H_INCLUDED
#define EFSFILE_H_INCLUDED

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_WEBIF_API 
 * @defgroup R_SW_PKG_93_EFS_FILE Embedded File System
 * @brief Functions to get the file data from EmbedFS
 *
 * @anchor R_SW_PKG_93_EFS_FILE_SUMMARY
 * @par Summary
 *
 * Embedded File System Interaction functions, allow user to extrapolate
 * information from files whilst maintaining encapsulation.
 * 
 * @anchor R_SW_PKG_93_EFS_FILE_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA1LU Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include <stddef.h>

/*****************************************************************************
Enumerated Types
******************************************************************************/

typedef enum _EFSERR
{
    EFS_OK = 0,
    EFS_FILE_NOT_FOUND = -1,
    EFS_DIRECTORY_NOT_FOUND = -2,
    EFS_BINARY_NOT_FOUND = -3,
    EFS_BINARY_ENDIAN_ERROR = -4,
    EFS_BINARY_ALIGNMENT_ERROR = -5
} EFSERR;

/*****************************************************************************
Typedefs
******************************************************************************/

typedef struct _VERSION
{
    
    uint32_t   ulEndianTag;  /*!< The endian tag 0x87654321UL */
    
    uint32_t   ulVersion;    /*!< The version of the utility that generated the data */
} VERSION,
*PVERSION;

/* The structure of the embedded file header */
typedef struct _EFHDR
{
    
    uint32_t   ulNextOffset;      /*!< The offset from the start of this header to the next in bytes.
                                     If this value is zero then this header is the last on the list */
    
    uint32_t   ulDataOffset;      /*!< The offset from the start of this header to the start of the
                                     file data */
   
    uint32_t   ulDataLength;      /*!< The length of the file data - set to zero when it is a folder marker */
} EFHDR,
*PEFHDR;

/* The structure of the embedded file */
typedef struct _EFILE
{
    
    EFHDR   fileHeader;    /*!< The file header */
    
    int8_t  szName;        /*!< Immediately followed by the file name */

    /* The file data follows this */
} EFILE,
*PEFILE;

/* Define a data structure to hold the information about the file */
typedef struct _EFS
{
    
    const int8_t    *pszFileName;     /*!< Pointer to the file name as stored in the binary.
                                           NOTE: The case may not match the string passed to the 
                                                 efsFindFile function */

    const int8_t    *pszFilePath;     /*!< Pointer to the file path as stored in the binary.
                                           NOTE: The case and directory delimiters may not 
                                                 match the string passes to the 
                                                 efsFindFile function */
    
    const uint8_t   *pbyFileData;    /*!< Pointer to the file data, always 4 byte aligned */
    
    uint32_t        ulFileLength;    /*!< The length of the data */
    
    _Bool           bfDataAllocated; /*!< Flag to specify that the file data must be freed on close */
} EFS,
*PEFS;

/*****************************************************************************
Public Functions
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief         Function to get a file from a binary file created by the 
 *                EmbedFS utility
 *   
 * @param[in]     pvBin: Pointer to the encapsulated file system
 * @param[in]     pszFilePathAndName: Pointer to the file path and name
 * @param[out]    pEfsFile: Pointer to the encapsulated file information
 * 
 * @retval        0:  Success 
 * @retval        ER_CODE: error code
 */
extern  EFSERR efsFindFile(void     *pvBin,
                           int8_t   *pszFilePathAndName,
                           PEFS     pEfsFile);

/**
 * @brief         Function to search the encapsulated file system for a file
 *     
 * @param[in]     pfsFile:  Pointer to the encapsulated file system
 * @param[in]     pszFind:  Pointer to the file path and name
 * @param[out]               OUT pEfsFile - Pointer to the encapsulated file 
 * 
 * @retval        0:  Success 
 * @retval        ER_CODE: error code
 */
extern  EFSERR efsSearch(PEFILE pfsFile, int8_t *pszFind, PEFS pEfsFile);

/**
 * @brief         Function to split a file name and path into the name and path
 *     
 * @param[in]     pszFilePathAndName:   Pointer to the file name and path
 * @param[out]    ppsPath: Pointer to the path pointer
 * @param[out]    pstPathLength: The length of the path
 * @param[out]    ppszFile: Pointer to the file name pointer
 * 
 * @return        None. 
 */
extern  void efsSplitFileNameAndPath(int8_t  *pszFilePathAndName,
                                     int8_t  **ppsPath,
                                     size_t  *pstPathLength,
                                     int8_t  **ppszFile);

/**
 * @brief         Function to return true if the directory is the root
 *     
 * @param[in]     pszPath: Pointer to the file path
 * @param[in]     stLength: The length of the file path
 * 
 * @retval        TRUE: If this is the root
 */
extern  _Bool efsIsRoot(int8_t *pszPath, size_t stLength);

/**
 * @brief         Function to search the encapsulated file system for a directory
 *     
 * @param[in]     pfsFile: Pointer to the directory
 * @param[in]     pszPath: Pointer to the directory name
 * @param[in]     stPathLength: The length of the directory path
 * 
 * @retval        p_dir: Pointer to the directory 
 * @retval        NULL:  On error
 */
extern  PEFILE efsSearchForDir(PEFILE pfsFile,
                               int8_t *pszPath,
                               size_t stPathLength);

/**
 * @brief         Function to search a directory for the file name
 *    
 * @param[in]     pfsFile:  Pointer to the directory
 * @param[in]     pszFile:  Pointer to the file name
 * @param[out]    pEfsFile: Pointer to the file information
 * 
 * @retval        0: For Success
 * @retval        ER_CODE: Error Code
 */
extern  EFSERR efsSearchForFile(PEFILE pfsFile,
                                int8_t *pszFile,
                                PEFS   pEfsFile);

/**
 * @brief         Function to compare two strings case insensitive
 *   
 * @param[in]     pszS1: Pointer to the first string
 * @param[in]     pszS2: Pointer to the second string
 * 
 * @retval        0:  If the two strings are equivalent
 */
extern  int efsStricmp(const int8_t *pszS1, const int8_t *pszS2);

/**
 * @brief         Function to perform a slash and case insensitive string compare
 *     
 * @param[in]     pszS1: Pointer to the first string
 * @param[in]     pszS2: Pointer to the second string
 * @param[in]     stLengthS1: The length of S1
 * 
 * @retval        0: If the two strings are equivalent.
 */
extern  int efsKhanCompare(const int8_t *pszS1,
                           const int8_t *pszS2,
                           size_t        stLengthS1);

#ifdef __cplusplus
}
#endif

#endif /* EFSFILE_H_INCLUDED */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
End  Of File
******************************************************************************/
