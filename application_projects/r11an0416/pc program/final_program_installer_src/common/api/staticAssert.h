/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#if !defined STATIC_ASSERT_H
#define STATIC_ASSERT_H

#define STATIC_ASSERT(aSSERT)  STATIC_ASSERT_BODY(aSSERT, __LINE__, __FILE__)
#define STATIC_ASSERT_BODY(aSSERT, line, file)  typedef char assert_failed_##file##line[(aSSERT)?1:-1];

#endif

