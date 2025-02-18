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

** emWin V6.44 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_SVG_OpenVG.h
Purpose     : OpenVG interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SVG_OPENVG_H
#define GUI_SVG_OPENVG_H

#include "GUI.h"

/*********************************************************************
*
*       OpenVG and EGL header files.
*
**********************************************************************
*/
#ifndef   GUI_SVG_OPENVG_HEADER
  #define GUI_SVG_OPENVG_HEADER   <VG/openvg.h>
#endif

#ifndef   GUI_SVG_VGU_HEADER
  #define GUI_SVG_VGU_HEADER      <VG/vgu.h>
#endif

#ifndef   GUI_SVG_EGL_HEADER
  #define GUI_SVG_EGL_HEADER      <EGL/egl.h>
#endif

#ifdef GUI_SVG_HAS_OPENVG
  #include GUI_SVG_OPENVG_HEADER
  #include GUI_SVG_VGU_HEADER
#endif

#ifdef GUI_SVG_HAS_EGL
  #include GUI_SVG_EGL_HEADER
#endif

/*********************************************************************
*
*       Availability of ShivaVG as DLL or source.
*
**********************************************************************
*/
//
// Define may be overriden in GUIConf.h
// This allows for easier switching between OpenVG implementations.
//
#ifndef GUI_SVG_HAS_SHIVAVG
  #if defined(GUI_SVG_HAS_OPENVG) && defined(__SHIVA_VG__)
    #define GUI_SVG_HAS_SHIVAVG       1    // Can be ShivaVG source or DLL.
    #define GUI_SVG_HAS_SHIVAVG_DLL   0
  #else
    #if defined(_WINDOWS) && !defined(GUI_SVG_HAS_OPENVG)  // Simulation always uses ShivaVG DLL by default.
      #define GUI_SVG_HAS_SHIVAVG       1
      #define GUI_SVG_HAS_SHIVAVG_DLL   1
    #else
      #define GUI_SVG_HAS_SHIVAVG       0
      #define GUI_SVG_HAS_SHIVAVG_DLL   0
    #endif
  #endif
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Type definitions
*
**********************************************************************
*/
#ifndef GUI_SVG_HAS_OPENVG
  //
  // Basic types
  //
  typedef enum {
    VG_FALSE = 0,
    VG_TRUE  = 1
  } VGboolean;
  #define VGfloat      float
  #define VGbyte       I8
  #define VGubyte      U8
  #define VGshort      U16
  #define VGint        I32
  #define VGuint       unsigned
  #define VGbitfield   U32
  //
  // Handle types
  //
  #define VGHandle     VGuint
  #define VGPath       VGuint
  #define VGPaint      VGuint
  #define VGImage      VGuint
  //
  // Enum types
  //
  #define VGParamType               int
  #define VGPathDatatype            int
  #define VGImageFormat             int
  #define VGPaintType               int
  #define VGPathParamType           int
  #define VGPaintParamType          int
  #define VGBlendMode               int
  #define VGPathCapabilities        int
  #define VGRenderingQuality        int
  #define VGImageQuality            int
  #define VGMatrixMode              int
  #define VGPaintMode               int
  #define VGFillRule                int
  #define VGColorRampSpreadMode     int
  //
  // Enum constants
  //
  #define VG_MATRIX_MODE                   ((VGParamType)0x1100)
  #define VG_STROKE_LINE_WIDTH             ((VGParamType)0x1110)
  #define VG_STROKE_CAP_STYLE              ((VGParamType)0x1111)
  #define VG_STROKE_JOIN_STYLE             ((VGParamType)0x1112)
  #define VG_STROKE_MITER_LIMIT            ((VGParamType)0x1113)
  #define VG_STROKE_DASH_PATTERN           ((VGParamType)0x1114)
  #define VG_STROKE_DASH_PHASE             ((VGParamType)0x1115)
  #define VG_STROKE_DASH_PHASE_RESET       ((VGParamType)0x1116)
  #define VG_STROKE_PATH                   ((VGPaintMode)(1 << 0))
  #define VG_FILL_PATH                     ((VGPaintMode)(1 << 1))
  #define VG_PATH_SCALE                    ((VGPathParamType)0x1602)
  #define VG_PATH_BIAS                     ((VGPathParamType)0x1603)
  #define VG_PATH_FORMAT_STANDARD          0
  #define VG_PATH_DATATYPE_F               ((VGPathDatatype)3)
  #define VG_PATH_CAPABILITY_ALL           ((VGPathCapabilities)((1 << 12) - 1))
  #define VG_PAINT_TYPE                    ((VGPaintParamType)0x1A00)
  #define VG_PAINT_COLOR                   ((VGPaintParamType)0x1A01)
  #define VG_PAINT_COLOR_RAMP_SPREAD_MODE  ((VGPaintParamType)0x1A02)
  #define VG_PAINT_COLOR_RAMP_STOPS        ((VGPaintParamType)0x1A03)
  #define VG_PAINT_LINEAR_GRADIENT         ((VGPaintParamType)0x1A04)
  #define VG_PAINT_RADIAL_GRADIENT         ((VGPaintParamType)0x1A05)
  #define VG_PAINT_TYPE_COLOR              ((VGPaintType)0x1B00)
  #define VG_PAINT_TYPE_LINEAR_GRADIENT    ((VGPaintType)0x1B01)
  #define VG_PAINT_TYPE_RADIAL_GRADIENT    ((VGPaintType)0x1B02)
  #define VG_CLEAR_COLOR                   ((VGParamType)0x1121)
  #define VG_FILL_RULE                     ((VGParamType)0x1101)
  #define VG_RENDERING_QUALITY             ((VGParamType)0x1103)
  #define VG_BLEND_MODE                    ((VGParamType)0x1104)
  #define VG_SCISSOR_RECTS                 ((VGParamType)0x1106)
  #define VG_SCISSORING                    ((VGParamType)0x1131)
  #define VG_BLEND_SRC_OVER                ((VGBlendMode)0x2001)
  #define VG_RENDERING_QUALITY_FASTER      ((VGRenderingQuality)0x1201)
  #define VG_RENDERING_QUALITY_BETTER      ((VGRenderingQuality)0x1202)
  #define VG_IMAGE_QUALITY_BETTER          ((VGImageQuality)(1 << 2))
  #define VG_MATRIX_PATH_USER_TO_SURFACE   ((VGMatrixMode)0x1400)
  #define VG_MATRIX_IMAGE_USER_TO_SURFACE  ((VGMatrixMode)0x1401)
  #define VG_MATRIX_FILL_PAINT_TO_USER     ((VGMatrixMode)0x1402)
  #define VG_MATRIX_STROKE_PAINT_TO_USER   ((VGMatrixMode)0x1403)
  #define VG_NON_ZERO                      ((VGFillRule)0x1901)
  #define VG_BW_1                          ((VGImageFormat)12)
  #define VG_sBGR_565                      ((VGImageFormat)(3 | (1 << 7)))
  #define VG_sRGB_565                      ((VGImageFormat) 3)
  #define VG_sABGR_1555                    ((VGImageFormat)(4 | (1 << 6) | (1 << 7)))
  #define VG_sARGB_1555                    ((VGImageFormat)(4 | (1 << 6)))
  #define VG_sXBGR_8888                    ((VGImageFormat)(0 | (1 << 6) | (1 << 7)))
  #define VG_sABGR_8888                    ((VGImageFormat)(1 | (1 << 6) | (1 << 7)))
  #define VG_sRGBA_8888                    ((VGImageFormat) 1)
  #define VG_sXRGB_8888                    ((VGImageFormat)(0 | (1 << 6)))
  #define VG_sARGB_8888                    ((VGImageFormat)(1 | (1 << 6)))
#endif

#ifndef GUI_SVG_HAS_EGL
  //
  // EGL constants
  //
  #define EGL_DONT_CARE               ((EGLint)-1)
  #define EGL_ALPHA_SIZE              0x3021
  #define EGL_BLUE_SIZE               0x3022
  #define EGL_GREEN_SIZE              0x3023
  #define EGL_RED_SIZE                0x3024
  #define EGL_SAMPLES                 0x3031
  #define EGL_SURFACE_TYPE            0x3033
  #define EGL_NONE                    0x3038
  #define EGL_LUMINANCE_SIZE          0x303D
  #define EGL_WINDOW_BIT              0x0004
  #define EGL_DEFAULT_DISPLAY         ((void *)0)
  #define EGL_SUCCESS                 0x3000
  #define EGL_OPENVG_API              0x30A1
  #define EGL_RENDERABLE_TYPE         0x3040
  #define EGL_OPENVG_BIT              0x0002
  #define EGL_PIXMAP_BIT              0x0002
  //
  // Generic types
  //
  #define NativeWindowType            void*
  //
  // EGL structs
  //
  typedef struct {
    int	   format;
    int	   width;
    int	   height;
    int	   stride;
    void * data;
  } NativePixmap;
  typedef NativePixmap*	NativePixmapType;
  //
  // EGL types
  //
  #define EGLDisplay           void*
  #define EGLSurface           void*
  #define EGLContext           void*
  #define EGLConfig            void*
  #define EGLNativeDisplayType void*
  #define EGLBoolean           unsigned int
  #define EGLint               int
  #define EGLenum              unsigned int
  #define EGLNativePixmapType  NativePixmapType
#endif

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
//
// EGL API
//
typedef EGLDisplay (GUI_SVG_EGL_GETDISPLAY_FUNC)         (EGLNativeDisplayType display_id);
typedef EGLSurface (GUI_SVG_EGL_CREATEPIXMAPSURFACE_FUNC)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
typedef EGLBoolean (GUI_SVG_EGL_DESTROYSURFACE_FUNC)     (EGLDisplay dpy, EGLSurface surface);
typedef EGLContext (GUI_SVG_EGL_CREATECONTEXT_FUNC)      (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
typedef EGLBoolean (GUI_SVG_EGL_INITIALIZE_FUNC)         (EGLDisplay dpy, EGLint *major, EGLint *minor);
typedef EGLBoolean (GUI_SVG_EGL_SWAPBUFFERS_FUNC)        (EGLDisplay dpy, EGLSurface surface);
typedef EGLBoolean (GUI_SVG_EGL_BINDAPI_FUNC)            (EGLenum api);
typedef EGLBoolean (GUI_SVG_EGL_CHOOSECONFIG_FUNC)       (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
typedef EGLBoolean (GUI_SVG_EGL_MAKECURRENT_FUNC)        (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
typedef EGLBoolean (GUI_SVG_EGL_COPYBUFFERS_FUNC)        (EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
typedef EGLint     (GUI_SVG_EGL_GETERROR_FUNC)           (void);
//
// OpenVG API
//
typedef void    (GUI_SVG_OPENVG_LOADMATRIX_FUNC)    (const VGfloat * pMatrix);
typedef void    (GUI_SVG_OPENVG_GETMATRIX_FUNC)     (VGfloat * pMatrix);
typedef VGPath  (GUI_SVG_OPENVG_CREATEPATH_FUNC)    (VGint PathFormat, VGPathDatatype PathType, VGfloat Scale, VGfloat Bias, VGint SegmentCapacity, VGint CoordCapacityHint, VGbitfield PathCapabilities);
typedef void    (GUI_SVG_OPENVG_DESTROYPATH_FUNC)   (VGPath hPath);
typedef VGImage (GUI_SVG_OPENVG_CREATEIMAGE_FUNC)   (VGImageFormat format, VGint width, VGint height, VGbitfield allowedQuality);
typedef void    (GUI_SVG_OPENVG_DESTROYIMAGE_FUNC)  (VGImage hImg);
typedef void    (GUI_SVG_OPENVG_DRAWIMAGE_FUNC)     (VGImage hImg);
typedef void    (GUI_SVG_OPENVG_IMAGESUBDATA_FUNC)  (VGImage image, const void * data, VGint dataStride, VGImageFormat dataFormat, VGint x, VGint y, VGint width, VGint height);
typedef void    (GUI_SVG_OPENVG_APPENDPATHDATA_FUNC)(VGPath hPath, VGint NumSegments, const VGubyte * pSegments, const void * pData);
typedef void    (GUI_SVG_OPENVG_DRAWPATH_FUNC)      (VGPath hPath, VGbitfield PaintModes);
typedef VGPaint (GUI_SVG_OPENVG_CREATEPAINT_FUNC)   (void);
typedef void    (GUI_SVG_OPENVG_SETPAINT_FUNC)      (VGPaint hPaint, VGbitfield PaintModes);
typedef void    (GUI_SVG_OPENVG_DESTROYPAINT_FUNC)  (VGPaint hPaint);
typedef void    (GUI_SVG_OPENVG_FLUSH_FUNC)         (void);
typedef void    (GUI_SVG_OPENVG_FINISH_FUNC)        (void);
typedef void    (GUI_SVG_OPENVG_WRITEPIXELS_FUNC)   (const void * data, VGint dataStride, VGImageFormat dataFormat, VGint dx, VGint dy, VGint width, VGint height);
typedef void    (GUI_SVG_OPENVG_READPIXELS_FUNC)    (void * data, VGint dataStride, VGImageFormat dataFormat, VGint sx, VGint sy, VGint width, VGint height);
typedef void    (GUI_SVG_OPENVG_SETF_FUNC)          (VGParamType ParamType, VGfloat Val);
typedef void    (GUI_SVG_OPENVG_SETI_FUNC)          (VGParamType ParamType, VGint Val);
typedef void    (GUI_SVG_OPENVG_SETFV_FUNC)         (VGParamType ParamType, VGint Count, const VGfloat * pData);
typedef void    (GUI_SVG_OPENVG_SETIV_FUNC)         (VGParamType ParamType, VGint Count, const VGint   * pData);
typedef VGfloat (GUI_SVG_OPENVG_GETF_FUNC)          (VGParamType ParamType);
typedef VGint   (GUI_SVG_OPENVG_GETI_FUNC)          (VGParamType ParamType);
typedef void    (GUI_SVG_OPENVG_GETFV_FUNC)         (VGParamType ParamType, VGint Count, VGfloat * pData);
typedef void    (GUI_SVG_OPENVG_GETIV_FUNC)         (VGParamType ParamType, VGint Count, VGint   * pData);
typedef void    (GUI_SVG_OPENVG_SETPARAMETERF_FUNC) (VGHandle hObj, VGint ParamType, VGfloat Val);
typedef void    (GUI_SVG_OPENVG_SETPARAMETERI_FUNC) (VGHandle hObj, VGint ParamType, VGint Val);
typedef void    (GUI_SVG_OPENVG_SETPARAMETERFV_FUNC)(VGHandle hObj, VGint ParamType, VGint Count, const VGfloat * pData);
typedef void    (GUI_SVG_OPENVG_SETPARAMETERIV_FUNC)(VGHandle hObj, VGint ParamType, VGint Count, const VGint   * pData);
typedef VGfloat (GUI_SVG_OPENVG_GETPARAMETERF_FUNC) (VGHandle hObj, VGint ParamType);
typedef VGint   (GUI_SVG_OPENVG_GETPARAMETERI_FUNC) (VGHandle hObj, VGint ParamType);
typedef void    (GUI_SVG_OPENVG_GETPARAMETERFV_FUNC)(VGHandle hObj, VGint ParamType, VGint Count, VGfloat * pData);
typedef void    (GUI_SVG_OPENVG_GETPARAMETERIV_FUNC)(VGHandle hObj, VGint ParamType, VGint Count, VGint   * pData);

/*********************************************************************
*
*       API struct for OpenVG
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_EGL_API_STRUCT
*
*  Description
*    Maps the required EGL functions of the Khronos OpenVG API.
*
*    Using EGL is optional since some OpenVG implementations do not implement
*    the EGL API.
*/
typedef struct {
  GUI_SVG_EGL_GETDISPLAY_FUNC          * pfGetDisplay;          // Pointer to EGL function \c{eglGetDisplay()}
  GUI_SVG_EGL_CREATEPIXMAPSURFACE_FUNC * pfCreatePixmapSurface; // Pointer to EGL function \c{eglCreatePixmapSurface()}
  GUI_SVG_EGL_DESTROYSURFACE_FUNC      * pfDestroySurface;      // Pointer to EGL function \c{eglDestroySurface()}
  GUI_SVG_EGL_CREATECONTEXT_FUNC       * pfCreateContext;       // Pointer to EGL function \c{eglCreateContext()}
  GUI_SVG_EGL_INITIALIZE_FUNC          * pfInitialize;          // Pointer to EGL function \c{eglInitialize()}
  GUI_SVG_EGL_SWAPBUFFERS_FUNC         * pfSwapBuffers;         // Pointer to EGL function \c{eglSwapBuffers()}
  GUI_SVG_EGL_BINDAPI_FUNC             * pfBindAPI;             // Pointer to EGL function \c{eglBindAPI()}
  GUI_SVG_EGL_CHOOSECONFIG_FUNC        * pfChooseConfig;        // Pointer to EGL function \c{eglChooseConfig()}
  GUI_SVG_EGL_MAKECURRENT_FUNC         * pfMakeCurrent;         // Pointer to EGL function \c{eglMakeCurrent()}
  GUI_SVG_EGL_COPYBUFFERS_FUNC         * pfCopyBuffers;         // Pointer to EGL function \c{eglCopyBuffers()}
  GUI_SVG_EGL_GETERROR_FUNC            * pfGetError;            // Pointer to EGL function \c{eglGetError()}
} GUI_SVG_EGL_API_STRUCT;

/*********************************************************************
*
*       GUI_SVG_OPENVG_API_STRUCT
*
*  Description
*    Maps the required functions of the Khronos OpenVG API.
*
*    A structure of this type can be set with GUI_SVG_DRIVER_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the OpenVG code (meaning \c{GUI_SVG_HAS_OPENVG} and \c{GUI_SVG_HAS_EGL}
*    were not defined.
*
*    For more details about these functions, please refer to the OpenVG
*    API documentation.
*/
typedef struct {
  GUI_SVG_OPENVG_LOADMATRIX_FUNC     * pfLoadMatrix;     // Pointer to OpenVG function \c{vgLoadMatrix()}
  GUI_SVG_OPENVG_GETMATRIX_FUNC      * pfGetMatrix;      // Pointer to OpenVG function \c{vgGetMatrix()}
  GUI_SVG_OPENVG_CREATEPATH_FUNC     * pfCreatePath;     // Pointer to OpenVG function \c{vgCreatePath()}
  GUI_SVG_OPENVG_DESTROYPATH_FUNC    * pfDestroyPath;    // Pointer to OpenVG function \c{vgDestroyPath()}
  GUI_SVG_OPENVG_CREATEIMAGE_FUNC    * pfCreateImage;    // Pointer to OpenVG function \c{vgCreateImage()}
  GUI_SVG_OPENVG_DESTROYIMAGE_FUNC   * pfDestroyImage;   // Pointer to OpenVG function \c{vgDestroyImage()}
  GUI_SVG_OPENVG_DRAWIMAGE_FUNC      * pfDrawImage;      // Pointer to OpenVG function \c{vgDrawImage()}
  GUI_SVG_OPENVG_IMAGESUBDATA_FUNC   * pfImageSubData;   // Pointer to OpenVG function \c{vgImageSubData()}
  GUI_SVG_OPENVG_APPENDPATHDATA_FUNC * pfAppendPathData; // Pointer to OpenVG function \c{vgAppendPathData()}
  GUI_SVG_OPENVG_DRAWPATH_FUNC       * pfDrawPath;       // Pointer to OpenVG function \c{vgDrawPath()}
  GUI_SVG_OPENVG_CREATEPAINT_FUNC    * pfCreatePaint;    // Pointer to OpenVG function \c{vgCreatePaint()}
  GUI_SVG_OPENVG_SETPAINT_FUNC       * pfSetPaint;       // Pointer to OpenVG function \c{vgSetPaint()}
  GUI_SVG_OPENVG_DESTROYPAINT_FUNC   * pfDestroyPaint;   // Pointer to OpenVG function \c{vgDestroyPaint()}
  GUI_SVG_OPENVG_FLUSH_FUNC          * pfFlush;          // Pointer to OpenVG function \c{vgFlush()}
  GUI_SVG_OPENVG_FINISH_FUNC         * pfFinish;         // Pointer to OpenVG function \c{vgFinish()}
  GUI_SVG_OPENVG_WRITEPIXELS_FUNC    * pfWritePixels;    // Pointer to OpenVG function \c{vgWritePixels()}
  GUI_SVG_OPENVG_READPIXELS_FUNC     * pfReadPixels;     // Pointer to OpenVG function \c{vgReadPixels()}
  GUI_SVG_OPENVG_SETF_FUNC           * pfSetF;           // Pointer to OpenVG function \c{vgSetf()}
  GUI_SVG_OPENVG_SETI_FUNC           * pfSetI;           // Pointer to OpenVG function \c{vgSeti()}
  GUI_SVG_OPENVG_SETFV_FUNC          * pfSetFV;          // Pointer to OpenVG function \c{vgSetfv()}
  GUI_SVG_OPENVG_SETIV_FUNC          * pfSetIV;          // Pointer to OpenVG function \c{vgSetiv()}
  GUI_SVG_OPENVG_GETF_FUNC           * pfGetF;           // Pointer to OpenVG function \c{vgGetf()}
  GUI_SVG_OPENVG_GETI_FUNC           * pfGetI;           // Pointer to OpenVG function \c{vgGeti()}
  GUI_SVG_OPENVG_GETFV_FUNC          * pfGetFV;          // Pointer to OpenVG function \c{vgGetfv()}
  GUI_SVG_OPENVG_GETIV_FUNC          * pfGetIV;          // Pointer to OpenVG function \c{vgGetiv()}
  GUI_SVG_OPENVG_SETPARAMETERF_FUNC  * pfSetParameterF;  // Pointer to OpenVG function \c{vgSetParameterf()}
  GUI_SVG_OPENVG_SETPARAMETERI_FUNC  * pfSetParameterI;  // Pointer to OpenVG function \c{vgSetParameteri()}
  GUI_SVG_OPENVG_SETPARAMETERFV_FUNC * pfSetParameterFV; // Pointer to OpenVG function \c{vgSetParameterfv()}
  GUI_SVG_OPENVG_SETPARAMETERIV_FUNC * pfSetParameterIV; // Pointer to OpenVG function \c{vgSetParameteriv()}
  GUI_SVG_OPENVG_GETPARAMETERF_FUNC  * pfGetParameterF;  // Pointer to OpenVG function \c{vgGetParameterf()}
  GUI_SVG_OPENVG_GETPARAMETERI_FUNC  * pfGetParameterI;  // Pointer to OpenVG function \c{vgGetParameteri()}
  GUI_SVG_OPENVG_GETPARAMETERFV_FUNC * pfGetParameterFV; // Pointer to OpenVG function \c{vgGetParameterfv()}
  GUI_SVG_OPENVG_GETPARAMETERIV_FUNC * pfGetParameterIV; // Pointer to OpenVG function \c{vgGetParameteriv()}
  const GUI_SVG_EGL_API_STRUCT       * pEGL;             // Pointer to a GUI_SVG_EGL_API_STRUCT structure.
                                                         // May be NULL if EGL is not to be used.
} GUI_SVG_OPENVG_API_STRUCT;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_DECLARE_EGL_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_EGL_API_STRUCT
*    filled with the correct function pointers.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*/
#define GUI_SVG_DECLARE_EGL_API(VAR_NAME)             \
  static const GUI_SVG_EGL_API_STRUCT VAR_NAME = {    \
    eglGetDisplay,                                    \
    eglCreatePixmapSurface,                           \
    eglDestroySurface,                                \
    eglCreateContext,                                 \
    eglInitialize,                                    \
    eglSwapBuffers,                                   \
    eglBindAPI,                                       \
    eglChooseConfig,                                  \
    eglMakeCurrent,                                   \
    eglCopyBuffers,                                   \
    eglGetError,                                      \
  }

/*********************************************************************
*
*       GUI_SVG_DECLARE_OPENVG_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_OPENVG_API_STRUCT
*    filled with the correct function pointers.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*    EGL_PTR:  Optional pointer to GUI_SVG_EGL_API_STRUCT if EGL API should
*              be used. If not, it must be left as NULL.
*/
#define GUI_SVG_DECLARE_OPENVG_API(VAR_NAME, EGL_PTR) \
  static const GUI_SVG_OPENVG_API_STRUCT VAR_NAME = { \
    vgLoadMatrix,                                     \
    vgGetMatrix,                                      \
    vgCreatePath,                                     \
    vgDestroyPath,                                    \
    vgCreateImage,                                    \
    vgDestroyImage,                                   \
    vgDrawImage,                                      \
    vgImageSubData,                                   \
    vgAppendPathData,                                 \
    vgDrawPath,                                       \
    vgCreatePaint,                                    \
    vgSetPaint,                                       \
    vgDestroyPaint,                                   \
    vgFlush,                                          \
    vgFinish,                                         \
    vgWritePixels,                                    \
    vgReadPixels,                                     \
    vgSetf,                                           \
    vgSeti,                                           \
    vgSetfv,                                          \
    vgSetiv,                                          \
    vgGetf,                                           \
    vgGeti,                                           \
    vgGetfv,                                          \
    vgGetiv,                                          \
    vgSetParameterf,                                  \
    vgSetParameteri,                                  \
    vgSetParameterfv,                                 \
    vgSetParameteriv,                                 \
    vgGetParameterf,                                  \
    vgGetParameteri,                                  \
    vgGetParameterfv,                                 \
    vgGetParameteriv,                                 \
    EGL_PTR,                                          \
  }

/*********************************************************************
*
*       GUI_SVG_DECLARE_OPENVG_AND_EGL_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_OPENVG_API_STRUCT
*    filled with the correct function pointers as well as a pointer
*    to a GUI_SVG_EGL_API_STRUCT structure.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*/
#define GUI_SVG_DECLARE_OPENVG_AND_EGL_API(VAR_NAME)   \
  GUI_SVG_DECLARE_EGL_API(_EGL);                       \
  GUI_SVG_DECLARE_OPENVG_API(VAR_NAME, &_EGL)

#if defined(__cplusplus)
}
#endif

#endif /* GUI_SVG_OPENVG_TYPES_H */

/*************************** End of file ****************************/
