

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ÅeAS ISÅf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/



#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <crypto.h>


static void writeHex(FILE *fp, uint8_t *pBuffer, uint32_t length)
{
  for (uint32_t i = 0; i < length-1; i++)
  {
    if (0 == i % 20)
    {
      fprintf(fp, "\n");
    }
    fprintf(fp, "0x%02X, ", pBuffer[i]);
  }
  fprintf(fp, "0x%02X\n", pBuffer[length-1]);
}

int main(const int argc, const char *argv[])
{
  if (argc != 3)
  {
    printf("Usage : %s <length of buffer> <fiename for buffer> \n", argv[0]);
    return -1;
  }

  uint32_t bufLen = atoi(argv[1]);
  const char *filename = argv[2];

  uint8_t *pBuffer = (uint8_t*)malloc(bufLen);
  if (0 == pBuffer)
  {
    printf("malloc failed\n");
    return -1;
  }


  // populate buffer.
  for (uint32_t i = 0; i < bufLen; i++)
  {
    pBuffer[i] = rand() & 0xff;
  }

  // calc hash.
  Sha256 hash;
  cryptoCalculateSha256Start(SEGMENTED_HASH_WHOLE_UPDATE, pBuffer, bufLen);
  cryptoCalculateSha256MiddleEnd(SEGMENTED_HASH_WHOLE_UPDATE, NULL, 0, true, &hash);

  FILE *fp;
  if (0 != fopen_s(&fp, filename, "w"))
  {
    printf("failed to open %s for write\n", filename);
    return -1;
  }

  fprintf(fp, "#include \"stdint.h\"\n\n");
  fprintf(fp, "#include \"randData.h\"\n\n");

  fprintf(fp, "const uint32_t randDataLength = %d;\n\n", bufLen);

  fprintf(fp, "const uint8_t randDataHash[] =\n{");
  writeHex(fp, hash, sizeof(hash));
  fprintf(fp, "}; \n\n");

  fprintf(fp, "const uint8_t randData[] =\n{");
  writeHex(fp, pBuffer, bufLen);
  fprintf(fp, "}; \n\n");

  fclose(fp);

  return 0;
}

