/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.50 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCD_SIM.h
Purpose     : Declares LCD interface functions
----------------------------------------------------------------------
*/

#ifndef LCDSIM_H
#define LCDSIM_H

#include "LCD.h"

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************
*
*       Defines
*
*************************************************************
*/
#define LCDSIM_MAX_DISPLAYS GUI_NUM_LAYERS

#define GUI_TRANSMODE_PIXELALPHA 0
#define GUI_TRANSMODE_CHROMA     1
#define GUI_TRANSMODE_ZERO       2

#define KEYSTATE_DIRTY           0x10000

/************************************************************
*
*       LCDSIM_      General declarations
*
*************************************************************
*/
void  LCDSIM_PreInit(void);
char* LCDSIM_Init(void);
void  LCDSIM_Exit(void);
int   LCDSIM_GetMouseState(LCD_tMouseState *pState);
void  LCDSIM_SetMouseState(int x, int y, int KeyStat, int LayerIndex);
void  LCDSIM_SetKeyState(U8 Key, I8 Pressed, int LayerIndex);
void  LCDSIM_CheckMouseState(int LayerIndex);
void  LCDSIM_CheckKeyState(int LayerIndex);
int   LCDSIM_SaveSBMP  (const char * sFileName);
int   LCDSIM_SaveSBMPEx(const char * sFileName, int x0, int y0, int xSize, int ySize);
void  LCDSIM_SetRGBOrder(unsigned RGBOrder);

/************************************************************
*
*       LCDSIM_     Functions for each display
*
*************************************************************
*/
void   LCDSIM_FillRect(int x0, int y0, int x1, int y1, int Index, int LayerIndex);
int    LCDSIM_GetModifyCnt(int LayerIndex);
int    LCDSIM_GetModifyCntInfo(int LayerIndex);
int    LCDSIM_GetPixelColor(int x, int y, int LayerIndex);
int    LCDSIM_GetPixelIndex(int x, int y, int LayerIndex);
int    LCDSIM_GetXSizeEx(int LayerIndex);
int    LCDSIM_GetYSizeEx(int LayerIndex);
int    LCDSIM_GetXSizeMax(void);
int    LCDSIM_GetYSizeMax(void);
int    LCDSIM_GetVXSizeEx(int LayerIndex);
int    LCDSIM_GetVYSizeEx(int LayerIndex);
int    LCDSIM_Index2Color(int Index, int LayerIndex);
int    LCDSIM_RLUT_GetPixelIndex(int x, int y, int LayerIndex);
void   LCDSIM_RLUT_SetPixelIndex(int x, int y, int Index, int LayerIndex);
void   LCDSIM_SetLUTEntry(U8 Pos, LCD_COLOR color, int LayerIndex);
void   LCDSIM_SetPixelIndex(int x, int y, int Index, int LayerIndex);
void   LCDSIM_SetPixelColor(int x, int y, LCD_COLOR PixelColor, int LayerIndex);
void   LCDSIM_SetSubPixel(int x, int y, U8 Value, int LayerIndex);
void   LCDSIM_SetPixelPhys(int x, int y, int Index, int LayerIndex);
int    LCDSIM_GetPixelPhys(int xPhys, int yPhys, int LayerIndex);
void   LCDSIM_FillRectPhys(int x0Phys, int y0Phys, int x1Phys, int y1Phys, int Index, int LayerIndex);
void   LCDSIM_SetOrg(int x, int y, int LayerIndex);
void   LCDSIM_SetAlpha(int Alpha, int LayerIndex);
int    LCDSIM_GetAlpha(int LayerIndex);
void   LCDSIM_SetLayerPos(int xPos, int yPos, int LayerIndex);
void   LCDSIM_SetLayerVis(int OnOff, int LayerIndex);
void   LCDSIM_SetSize(int LayerIndex, int xSize, int ySize);
void   LCDSIM_SetTransMode(int LayerIndex, int TransMode);
void   LCDSIM_SetChroma(int LayerIndex, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);
void   LCDSIM_SetCompositeColor(U32 Color);
void   LCDSIM_SetCompositeSize(int xSize, int ySize);
void   LCDSIM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst);
void   LCDSIM_Invalidate(int LayerIndex);
void * LCDSIM_GetBufferPtr(int LayerIndex);
void   LCDSIM_SetBackBufferIndex(int LayerIndex, int BufferIndex);

/********************************************************************
*
*       Publics for LCD
*
*********************************************************************
*/
void         SIM_GUI_SetCompositeSize(int xSize, int ySize);
void         SIM_GUI_SetCompositeColor(U32 Color);
U32          SIM_GUI_GetCompositeColor(void);
void         SIM_GUI_SetLCDPos(int xPos, int yPos);
int          SIM_GUI_SaveBMP(const char * sFileName);
int          SIM_GUI_SaveBMPEx(const char * sFileName, int x0, int y0, int xSize, int ySize);
int          SIM_GUI_SaveCompositeBMP(const char * sFileName);
int          SIM_GUI_SetTransColor(int Color);
int          SIM_GUI_SetLCDColorBlack (unsigned int Index, int Color);
int          SIM_GUI_SetLCDColorWhite (unsigned int Index, int Color);
void         SIM_GUI_SetMag(int MagX, int MagY);
int          SIM_GUI_GetMagX(void);
int          SIM_GUI_GetMagY(void);
int          SIM_GUI_GetForwardRButton(void);
void         SIM_GUI_SetForwardRButton(int OnOff);
void         SIM_GUI_SetTransMode(int LayerIndex, int TransMode);
void         SIM_GUI_SetChroma(int LayerIndex, unsigned long ChromaMin, unsigned long ChromaMax);
void         SIM_GUI_TakeScreenshot(void);
void         SIM_GUI_UseCustomBitmaps(void);
void         SIM_GUI_SetAccellerator(int Accellerator);
void         SIM_GUI_SetMainScreenOffset(int x, int y);
void         SIM_GUI_SetCompositeTouch(int LayerIndex);
int          SIM_GUI_GetCompositeTouch(void);
const char * SIM_GUI_SetCmdLine(char * pCmdLine);
void         SIM_GUI_SetAppName(const char * pAppName);
void         SIM_GUI_SetStartTime(int TimeStart);
void         SIM_GUI_EnableModifierKey(int OnOff);
int          SIM_GUI_IsModifierKeyEnabled(void);

/********************************************************************
*
*       Routine(s) in user application
*
*********************************************************************
*/
void SIM_X_Config(void);   /* Allow init before application starts ... Use it to set LCD offset etc */

/********************************************************************
*
*       Publics used by GUI_X module
*
*********************************************************************
*/
void SIM_GUI_Delay (int ms);
void SIM_GUI_ExecIdle(void);
int  SIM_GUI_GetTime(void);
int  SIM_GUI_GetKey(void);
int  SIM_GUI_WaitKey(void);
void SIM_GUI_StoreKey(int);

/********************************************************************
*
*       Publics for logging, warning, errorout
*
*********************************************************************
*/
void SIM_GUI_Log(const char *s);
void SIM_GUI_Log1(const char *s, int p0);
void SIM_GUI_Log2(const char *s, int p0, int p1);
void SIM_GUI_Log3(const char *s, int p0, int p1, int p2);
void SIM_GUI_Log4(const char *s, int p0, int p1, int p2,int p3);
void SIM_GUI_Warn(const char *s);
void SIM_GUI_Warn1(const char *s, int p0);
void SIM_GUI_Warn2(const char *s, int p0, int p1);
void SIM_GUI_Warn3(const char *s, int p0, int p1, int p2);
void SIM_GUI_Warn4(const char *s, int p0, int p1, int p2, int p3);
void SIM_GUI_ErrorOut(const char *s);
void SIM_GUI_ErrorOut1(const char *s, int p0);
void SIM_GUI_ErrorOut2(const char *s, int p0, int p1);
void SIM_GUI_ErrorOut3(const char *s, int p0, int p1, int p2);
void SIM_GUI_ErrorOut4(const char *s, int p0, int p1, int p2, int p3);
void SIM_GUI_EnableMessageBoxOnError(int Status);

/********************************************************************
*
*       Commandline support
*
*********************************************************************
*/
const char *SIM_GUI_GetCmdLine(void);

/********************************************************************
*
*       Multitasking support
*
*********************************************************************
*/
void SIM_GUI_CreateTask(char * pName, void * pFunc);
void SIM_GUI_Start(void);
unsigned long SIM_GUI_GetTaskID(void);
void SIM_GUI_Lock(void);
void SIM_GUI_Unlock(void);
void SIM_GUI_InitOS(void);



/********************************************************************
*
*              Publics for Hardkey simulation
*
*********************************************************************
*/
typedef void SIM_HARDKEY_CB(int KeyIndex, int State);

int              SIM_HARDKEY_GetNum(void);
int              SIM_HARDKEY_GetState(unsigned int i);
SIM_HARDKEY_CB*  SIM_HARDKEY_SetCallback(unsigned int KeyIndex, SIM_HARDKEY_CB* pfCallback);
int              SIM_HARDKEY_SetMode (unsigned int KeyIndex, int Mode);
int              SIM_HARDKEY_SetState(unsigned int KeyIndex, int State);

/********************************************************************
*
*              Publics for LCD
*
*********************************************************************
*/
#define SIM_SetLCDPos(x, y)                 SIM_GUI_SetLCDPos(x, y)
#define SIM_SetTransColor(Color)            SIM_GUI_SetTransColor(Color)
#define SIM_SetLCDColorBlack(Index, Color)  SIM_GUI_SetLCDColorBlack(Index, Color)
#define SIM_SetLCDColorWhite(Index, Color)  SIM_GUI_SetLCDColorWhite(Index, Color)
#define SIM_SetMag(MagX, MagY)              SIM_GUI_SetMag(MagX, MagY)
#define SIM_GetMagX()                       SIM_GUI_GetMagX()
#define SIM_GetMagY()                       SIM_GUI_GetMagY()

/********************************************************************
*
*              Publics used by GUI_X module
*
*********************************************************************
*/
#define SIM_Delay(ms)     SIM_GUI_Delay(ms)
#define SIM_ExecIdle()    SIM_GUI_ExecIdle()
#define SIM_GetTime()     SIM_GUI_GetTime()
#define SIM_GetKey()      SIM_GUI_GetKey()
#define SIM_WaitKey()     SIM_GUI_WaitKey()
#define SIM_StoreKey(Key) SIM_GUI_StoreKey(Key)

/********************************************************************
*
*              Publics for logging, warning, errorout
*
*********************************************************************
*/
#define SIM_Log(s)                          SIM_GUI_Log(s)
#define SIM_Log1(s, p0)                     SIM_GUI_Log1(s, p0)
#define SIM_Log2(s, p0, p1)                 SIM_GUI_Log2(s, p0, p1)
#define SIM_Log3(s, p0, p1, p2)             SIM_GUI_Log3(s, p0, p1, p2)
#define SIM_Log4(s, p0, p1, p2,p3)          SIM_GUI_Log4(s, p0, p1, p2,p3)
#define SIM_Warn(s)                         SIM_GUI_Warn(s)
#define SIM_Warn1(s, p0)                    SIM_GUI_Warn1(s, p0)
#define SIM_Warn2(s, p0, p1)                SIM_GUI_Warn2(s, p0, p1)
#define SIM_Warn3(s, p0, p1, p2)            SIM_GUI_Warn3(s, p0, p1, p2)
#define SIM_Warn4(s, p0, p1, p2, p3)        SIM_GUI_Warn4(s, p0, p1, p2, p3)
#define SIM_ErrorOut(s)                     SIM_GUI_ErrorOut(s)
#define SIM_ErrorOut1(s, p0)                SIM_GUI_ErrorOut1(s, p0)
#define SIM_ErrorOut2(s, p0, p1)            SIM_GUI_ErrorOut2(s, p0, p1)
#define SIM_ErrorOut3(s, p0, p1, p2)        SIM_GUI_ErrorOut3(s, p0, p1, p2)
#define SIM_ErrorOut4(s, p0, p1, p2, p3)    SIM_GUI_ErrorOut4(s, p0, p1, p2, p3)
#define SIM_EnableMessageBoxOnError(Status) SIM_GUI_EnableMessageBoxOnError(Status)

/********************************************************************
*
*              Commandline support
*
*********************************************************************
*/
#define SIM_GetCmdLine() SIM_GUI_GetCmdLine()

/********************************************************************
*
*              Multitasking support
*
*********************************************************************
*/
#define SIM_CreateTask(pName, pFunc) SIM_GUI_CreateTask(pName, pFunc)
#define SIM_Start()                  SIM_GUI_Start()
#define SIM_GetTaskID()              SIM_GUI_GetTaskID()
#define SIM_Lock()                   SIM_GUI_Lock()
#define SIM_Unlock()                 SIM_GUI_Unlock()
#define SIM_InitOS()                 SIM_GUI_InitOS()




#if defined(__cplusplus)
}
#endif

#endif /* LCD_H */




