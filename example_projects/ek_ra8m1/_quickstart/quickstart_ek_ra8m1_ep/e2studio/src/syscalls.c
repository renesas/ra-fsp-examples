/***********************************************************************************************************************
 * File Name    : syscalls.c
 * Description  : Override for missing system functions required for GCC.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include <errno.h>
#include <sys/stat.h>

int _open(const char *path, int file);
int _close(int file);
int _getpid(void);
int _kill(int pid, int sig);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);
int _fstat(int file, struct stat *pstat);
int _isatty(int file);

int _open(const char *path, int file) {
    FSP_PARAMETER_NOT_USED(path);
    FSP_PARAMETER_NOT_USED(file);
  return -1;
}

int _close(int file) {
    FSP_PARAMETER_NOT_USED(file);
  return -1;
}

int _getpid(void) {
  return 1;
}


#undef errno
extern int errno;
int _kill(int pid, int sig) {
    FSP_PARAMETER_NOT_USED(pid);
    FSP_PARAMETER_NOT_USED(sig);
  errno = EINVAL;
  return -1;
}

int _lseek(int file, int ptr, int dir) {
    FSP_PARAMETER_NOT_USED(file);
    FSP_PARAMETER_NOT_USED(ptr);
    FSP_PARAMETER_NOT_USED(dir);
  return 0;
}

int _read(int file, char *ptr, int len) {
    FSP_PARAMETER_NOT_USED(file);
    FSP_PARAMETER_NOT_USED(ptr);
    FSP_PARAMETER_NOT_USED(len);
  return 0;
}

int _write(int file, char *ptr, int len) {
  int todo;

  FSP_PARAMETER_NOT_USED(file);
  FSP_PARAMETER_NOT_USED(ptr);

  for (todo = 0; todo < len; todo++) {
    //outbyte (*ptr++);
  }
  return len;
}

int _fstat (int file, struct stat *pstat)
{
    FSP_PARAMETER_NOT_USED(file);
    FSP_PARAMETER_NOT_USED(pstat);
    return 0;
}


int _isatty(int file)
{
    FSP_PARAMETER_NOT_USED(file);
    return 0;
}
