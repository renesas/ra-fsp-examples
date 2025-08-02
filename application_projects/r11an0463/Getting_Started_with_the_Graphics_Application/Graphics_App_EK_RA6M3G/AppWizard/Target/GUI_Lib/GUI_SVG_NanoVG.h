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

** emWin V6.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_SVG_NanoVG.h
Purpose     : NanoVG interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SVG_NANOVG_H
#define GUI_SVG_NANOVG_H

#include "GUI.h"
#include "GUI_SVG_Global.h"

/*********************************************************************
*
*       OpenGL header files
*
**********************************************************************
*/
#ifndef   GUI_SVG_OPENGL2_HEADER
  #define GUI_SVG_OPENGL2_HEADER      <GL/gl.h>
#endif

#ifndef   GUI_SVG_OPENGL3_HEADER
  #define GUI_SVG_OPENGL3_HEADER      <GL/gl3.h>
#endif

#ifndef   GUI_SVG_OPENGLES2_HEADER
  #define GUI_SVG_OPENGLES2_HEADER    <GLES2/gl2.h>
#endif

#ifndef   GUI_SVG_OPENGLES3_HEADER
  #define GUI_SVG_OPENGLES3_HEADER    <GLES3/gl3.h>
#endif

/*********************************************************************
*
*       NanoVG header files.
*
**********************************************************************
*/
#ifndef   GUI_SVG_NANOVG_HEADER
  #define GUI_SVG_NANOVG_HEADER      <nanovg.h>
#endif

#ifndef   GUI_SVG_NANOVG_GL_HEADER
  #define GUI_SVG_NANOVG_GL_HEADER   <nanovg_gl.h>
#endif

/*********************************************************************
*
*       NanoVG rendering backend
*
**********************************************************************
*/
#define GUI_SVG_NANOVG_BACKEND_GL2     0
#define GUI_SVG_NANOVG_BACKEND_GL3     1
#define GUI_SVG_NANOVG_BACKEND_GLES2   2
#define GUI_SVG_NANOVG_BACKEND_GLES3   3
//
// Default: OpenGL ES 2.0
//
#ifndef   GUI_SVG_NANOVG_RENDER_BACKEND
  #define GUI_SVG_NANOVG_RENDER_BACKEND   GUI_SVG_NANOVG_BACKEND_GLES2
#endif
//
// Make compile time selection of rendering backend.
//
#if   (GUI_SVG_NANOVG_RENDER_BACKEND == GUI_SVG_NANOVG_BACKEND_GL2)
  #ifdef GUI_SVG_NANOVG_IMPLEMENTATION
    #define NANOVG_GL2_IMPLEMENTATION
  #endif
  #define GUI_SVG_NANOVG_RENDER_BACKEND_HEADER     GUI_SVG_OPENGL2_HEADER
  #define FUNC_nvgCreate                           nvgCreateGL2
  #define FUNC_nvgDelete                           nvgDeleteGL2
#elif (GUI_SVG_NANOVG_RENDER_BACKEND == GUI_SVG_NANOVG_BACKEND_GL3)
  #ifdef GUI_SVG_NANOVG_IMPLEMENTATION
    #define NANOVG_GL3_IMPLEMENTATION
  #endif
  #define GUI_SVG_NANOVG_RENDER_BACKEND_HEADER     GUI_SVG_OPENGL3_HEADER
  #define FUNC_nvgCreate                           nvgCreateGL3
  #define FUNC_nvgDelete                           nvgDeleteGL3
#elif (GUI_SVG_NANOVG_RENDER_BACKEND == GUI_SVG_NANOVG_BACKEND_GLES2)
  #ifdef GUI_SVG_NANOVG_IMPLEMENTATION
    #define NANOVG_GLES2_IMPLEMENTATION
  #endif
  #define GUI_SVG_NANOVG_RENDER_BACKEND_HEADER     GUI_SVG_OPENGLES2_HEADER
  #define FUNC_nvgCreate                           nvgCreateGLES2
  #define FUNC_nvgDelete                           nvgDeleteGLES2
#elif (GUI_SVG_NANOVG_RENDER_BACKEND == GUI_SVG_NANOVG_BACKEND_GLES3)
  #ifdef GUI_SVG_NANOVG_IMPLEMENTATION
    #define NANOVG_GLES3_IMPLEMENTATION
  #endif
  #define GUI_SVG_NANOVG_RENDER_BACKEND_HEADER     GUI_SVG_OPENGLES3_HEADER
  #define FUNC_nvgCreate                           nvgCreateGLES3
  #define FUNC_nvgDelete                           nvgDeleteGLES3
#endif
//
// Stringify defines for LoadAPI function.
//
#define STR_nvgCreate      SVG_MAKE_STR(FUNC_nvgCreate)
#define STR_nvgDelete      SVG_MAKE_STR(FUNC_nvgDelete)

#ifdef GUI_SVG_HAS_NANOVG
  //
  // Configure NanoVG
  //
  #ifndef   NVG_MAX_STATES
    #define NVG_MAX_STATES  1    // To save memory, we don't save/restore states.
  #endif
  //
  // Now we can include NanoVG
  //
  #include GUI_SVG_NANOVG_RENDER_BACKEND_HEADER
  #include GUI_SVG_NANOVG_HEADER
  #include GUI_SVG_NANOVG_GL_HEADER
  //
  // Cleanup, prevent multiple definitions
  //
  #undef NANOVG_GL2_IMPLEMENTATION
  #undef NANOVG_GL3_IMPLEMENTATION
  #undef NANOVG_GLES2_IMPLEMENTATION
  #undef NANOVG_GLES3_IMPLEMENTATION
  #undef NANOVG_GL_IMPLEMENTATION
  #undef GUI_SVG_NANOVG_IMPLEMENTATION
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Type definitions: OpenGL
*
**********************************************************************
*/
#ifndef GUI_SVG_HAS_NANOVG
  #define GLint        int
  #define GLsizei      int
  #define GLenum       unsigned int
  #define GLvoid       void
  #define GLbitfield   unsigned int
#endif

/*********************************************************************
*
*       Type definitions: NanoVG
*
**********************************************************************
*/
#ifndef GUI_SVG_HAS_NANOVG
  DEFINE_TYPE_PUBLIC(NVGcolor,
    union {
      float rgba[4];
      struct {
        float r;
        float g;
        float b;
        float a;
      };
    };
  );
  DEFINE_TYPE_PUBLIC(NVGpaint,
    float xform[6];
    float extent[2];
    float radius;
    float feather;
    NVGcolor innerColor;
    NVGcolor outerColor;
    int image;
  );
  #define NVGcontext     void
  #define NVGcreateFlags int
  #define NVGimageFlags  int
  #define NVGlineCap     int
  #define NVGsolidity    int
#endif

/*********************************************************************
*
*       Function prototypes: OpenGL
*
**********************************************************************
*/
typedef void (GUI_SVG_GL_CLEAR_FUNC)     (GLbitfield mask);
typedef void (GUI_SVG_GL_READPIXELS_FUNC)(GLint x, GLint y, GLsizei width, GLsizei height,
                                          GLenum format, GLenum type, GLvoid * pixels);

/*********************************************************************
*
*       API struct for OpenGL
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_GL_API_STRUCT
*
*  Description
*    Maps the required OpenGL functions required for NanoVG.
*
*    For more details about these functions, please refer to the OpenGL
*    API documentation.
*/
typedef struct {
  GUI_SVG_GL_CLEAR_FUNC      * pfClear;       // Pointer to OpenGL function \c{glClear()}.
  GUI_SVG_GL_READPIXELS_FUNC * pfReadPixels;  // Pointer to OpenGL function \c{glReadPixels()}.
} GUI_SVG_GL_API_STRUCT;

/*********************************************************************
*
*       Function prototypes: NanoVG
*
**********************************************************************
*/
typedef void         (GUI_SVG_NANOVG_BEGINFRAME_FUNC)     (NVGcontext * vg, float width, float height, float devicePixelRatio);
typedef void         (GUI_SVG_NANOVG_BEGINPATH_FUNC)      (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_BEZIERTO_FUNC)       (NVGcontext * vg, float c1x, float c1y, float c2x, float c2y, float x, float y);
typedef void         (GUI_SVG_NANOVG_CLOSEPATH_FUNC)      (NVGcontext * vg);
typedef NVGcontext * (GUI_SVG_NANOVG_CREATE_FUNC)         (int flags);
typedef int          (GUI_SVG_NANOVG_CREATEIMAGERGBA_FUNC)(NVGcontext * vg, int width, int height, int flags, const unsigned char* data);
typedef void         (GUI_SVG_NANOVG_DELETE_FUNC)         (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_DELETEIMAGE_FUNC)    (NVGcontext * vg, int image);
typedef void         (GUI_SVG_NANOVG_ENDFRAME_FUNC)       (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_FILL_FUNC)           (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_FILLCOLOR_FUNC)      (NVGcontext * vg, NVGcolor color);
typedef void         (GUI_SVG_NANOVG_FILLPAINT_FUNC)      (NVGcontext * vg, NVGpaint paint);
typedef void         (GUI_SVG_NANOVG_GLOBALALPHA_FUNC)    (NVGcontext * vg, float alpha);
typedef NVGpaint     (GUI_SVG_NANOVG_IMAGEPATTERN_FUNC)   (NVGcontext * vg, float ox, float oy, float ex, float ey, float angle, int image, float alpha);
typedef void         (GUI_SVG_NANOVG_LINECAP_FUNC)        (NVGcontext * vg, int cap);
typedef void         (GUI_SVG_NANOVG_LINEJOIN_FUNC)       (NVGcontext * vg, int join);
typedef void         (GUI_SVG_NANOVG_LINETO_FUNC)         (NVGcontext * vg, float x, float y);
typedef void         (GUI_SVG_NANOVG_MITERLIMIT_FUNC)     (NVGcontext * vg, float limit);
typedef void         (GUI_SVG_NANOVG_MOVETO_FUNC)         (NVGcontext * vg, float x, float y);
typedef void         (GUI_SVG_NANOVG_PATHWINDING_FUNC)    (NVGcontext * vg, int dir);
typedef void         (GUI_SVG_NANOVG_QUADTO_FUNC)         (NVGcontext * vg, float cx, float cy, float x, float y);
typedef void         (GUI_SVG_NANOVG_RECT_FUNC)           (NVGcontext * vg, float x, float y, float w, float h);
typedef void         (GUI_SVG_NANOVG_RESETSCISSOR_FUNC)   (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_RESETTRANSFORM_FUNC) (NVGcontext * vg);
typedef NVGcolor     (GUI_SVG_NANOVG_RGBA_FUNC)           (unsigned char r, unsigned char g, unsigned char b, unsigned char a);
typedef void         (GUI_SVG_NANOVG_SCISSOR_FUNC)        (NVGcontext * vg, float x, float y, float w, float h);
typedef void         (GUI_SVG_NANOVG_SHAPEANTIALIAS_FUNC) (NVGcontext * vg, int enabled);
typedef void         (GUI_SVG_NANOVG_STROKE_FUNC)         (NVGcontext * vg);
typedef void         (GUI_SVG_NANOVG_STROKECOLOR_FUNC)    (NVGcontext * vg, NVGcolor color);
typedef void         (GUI_SVG_NANOVG_STROKEWIDTH_FUNC)    (NVGcontext * vg, float size);
typedef void         (GUI_SVG_NANOVG_TRANSFORM_FUNC)      (NVGcontext * vg, float a, float b, float c, float d, float e, float f);

/*********************************************************************
*
*       API struct for NanoVG
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_NANOVG_API_STRUCT
*
*  Description
*    Maps the required functions of the NanoVG API.
*
*    A structure of this type can be set with GUI_SVG_DRIVER_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the NanoVG code (meaning \c{GUI_SVG_HAS_NANOVG} was not defined.
*
*    For more details about these functions, please refer to the NanoVG
*    API documentation.
*/
typedef struct {
  GUI_SVG_GL_API_STRUCT                 GL;                  // Functions for OpenGL rendering backend.
  GUI_SVG_NANOVG_BEGINFRAME_FUNC      * pfBeginFrame;        // Pointer to NanoVG function \c{(nvgBeginFrame)}. 
  GUI_SVG_NANOVG_BEGINPATH_FUNC       * pfBeginPath;         // Pointer to NanoVG function \c{(nvgBeginPath)}. 
  GUI_SVG_NANOVG_BEZIERTO_FUNC        * pfBezierTo;          // Pointer to NanoVG function \c{(nvgBezierTo)}. 
  GUI_SVG_NANOVG_CLOSEPATH_FUNC       * pfClosePath;         // Pointer to NanoVG function \c{(nvgClosePath)}. 
  GUI_SVG_NANOVG_CREATE_FUNC          * pfCreate;            // Pointer to NanoVG function \c{(nvgCreate)}. 
  GUI_SVG_NANOVG_CREATEIMAGERGBA_FUNC * pfCreateImageRGBA;   // Pointer to NanoVG function \c{(nvgCreateImageRGB)}. 
  GUI_SVG_NANOVG_DELETE_FUNC          * pfDelete;            // Pointer to NanoVG function \c{(nvgDelete)}. 
  GUI_SVG_NANOVG_DELETEIMAGE_FUNC     * pfDeleteImage;       // Pointer to NanoVG function \c{(nvgDeleteImage)}. 
  GUI_SVG_NANOVG_ENDFRAME_FUNC        * pfEndFrame;          // Pointer to NanoVG function \c{(nvgEndFrame)}. 
  GUI_SVG_NANOVG_FILL_FUNC            * pfFill;              // Pointer to NanoVG function \c{(nvgFill)}. 
  GUI_SVG_NANOVG_FILLCOLOR_FUNC       * pfFillColor;         // Pointer to NanoVG function \c{(nvgFillColor)}. 
  GUI_SVG_NANOVG_FILLPAINT_FUNC       * pfFillPaint;         // Pointer to NanoVG function \c{(nvgFillPaint)}. 
  GUI_SVG_NANOVG_GLOBALALPHA_FUNC     * pfGlobalAlpha;       // Pointer to NanoVG function \c{(nvgGlobalAlpha)}. 
  GUI_SVG_NANOVG_IMAGEPATTERN_FUNC    * pfImagePattern;      // Pointer to NanoVG function \c{(nvgImagePattern)}. 
  GUI_SVG_NANOVG_LINECAP_FUNC         * pfLineCap;           // Pointer to NanoVG function \c{(nvgLineCap)}. 
  GUI_SVG_NANOVG_LINEJOIN_FUNC        * pfLineJoin;          // Pointer to NanoVG function \c{(nvgLineJoin)}. 
  GUI_SVG_NANOVG_LINETO_FUNC          * pfLineTo;            // Pointer to NanoVG function \c{(nvgLineTo)}. 
  GUI_SVG_NANOVG_MITERLIMIT_FUNC      * pfMiterLimit;        // Pointer to NanoVG function \c{(nvgMiterLimit)}. 
  GUI_SVG_NANOVG_MOVETO_FUNC          * pfMoveTo;            // Pointer to NanoVG function \c{(nvgMoveTo)}. 
  GUI_SVG_NANOVG_PATHWINDING_FUNC     * pfPathWinding;       // Pointer to NanoVG function \c{(nvgPathWinding)}. 
  GUI_SVG_NANOVG_QUADTO_FUNC          * pfQuadTo;            // Pointer to NanoVG function \c{(nvgQuadTo)}. 
  GUI_SVG_NANOVG_RECT_FUNC            * pfRect;              // Pointer to NanoVG function \c{(nvgRect)}. 
  GUI_SVG_NANOVG_RESETSCISSOR_FUNC    * pfResetScissor;      // Pointer to NanoVG function \c{(nvgResetScissor)}. 
  GUI_SVG_NANOVG_RESETTRANSFORM_FUNC  * pfResetTransform;    // Pointer to NanoVG function \c{(nvgResetTransform)}. 
  GUI_SVG_NANOVG_RGBA_FUNC            * pfRGBA;              // Pointer to NanoVG function \c{(nvgRGBA)}. 
  GUI_SVG_NANOVG_SCISSOR_FUNC         * pfScissor;           // Pointer to NanoVG function \c{(nvgScissor)}. 
  GUI_SVG_NANOVG_SHAPEANTIALIAS_FUNC  * pfShapeAntiAlias;    // Pointer to NanoVG function \c{(nvgShapeAntiAlias)}. 
  GUI_SVG_NANOVG_STROKE_FUNC          * pfStroke;            // Pointer to NanoVG function \c{(nvgStroke)}. 
  GUI_SVG_NANOVG_STROKECOLOR_FUNC     * pfStrokeColor;       // Pointer to NanoVG function \c{(nvgStrokeColor)}. 
  GUI_SVG_NANOVG_STROKEWIDTH_FUNC     * pfStrokeWidth;       // Pointer to NanoVG function \c{(nvgStrokeWidth)}. 
  GUI_SVG_NANOVG_TRANSFORM_FUNC       * pfTransform;         // Pointer to NanoVG function \c{(nvgTransform)}.
} GUI_SVG_NANOVG_API_STRUCT;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_NANOVG_DECLARE_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_NANOVG_API_STRUCT
*    filled with the correct function pointers.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*/
#define GUI_SVG_NANOVG_DECLARE_API(VAR_NAME)                 \
  static const GUI_SVG_NANOVG_API_STRUCT VAR_NAME = {        \
    {                                                        \
      glClear,                                               \
      glReadPixels,                                          \
    },                                                       \
    nvgBeginFrame,                                           \
    nvgBeginPath,                                            \
    nvgBezierTo,                                             \
    nvgClosePath,                                            \
    nvgCreate,                                               \
    nvgCreateImageRGBA,                                      \
    nvgDelete,                                               \
    nvgDeleteImage,                                          \
    nvgEndFrame,                                             \
    nvgFill,                                                 \
    nvgFillColor,                                            \
    nvgFillPaint,                                            \
    nvgGlobalAlpha,                                          \
    nvgImagePattern,                                         \
    nvgLineCap,                                              \
    nvgLineJoin,                                             \
    nvgLineTo,                                               \
    nvgMiterLimit,                                           \
    nvgMoveTo,                                               \
    nvgPathWinding,                                          \
    nvgQuadTo,                                               \
    nvgRect,                                                 \
    nvgResetScissor,                                         \
    nvgResetTransform,                                       \
    nvgRGBA,                                                 \
    nvgScissor,                                              \
    nvgShapeAntiAlias,                                       \
    nvgStroke,                                               \
    nvgStrokeColor,                                          \
    nvgStrokeWidth,                                          \
    nvgTransform,                                            \
  }

#if defined(__cplusplus)
}
#endif

#endif /* GUI_SVG_NANOVG_H */

/*************************** End of file ****************************/
