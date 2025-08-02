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
File        : GUI_SVG_NemaVG.h
Purpose     : NemaVG interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SVG_NEMAVG_H
#define GUI_SVG_NEMAVG_H

#include "GUI.h"
#include "GUI_SVG_Global.h"

/*********************************************************************
*
*       NemaVG header files.
*
**********************************************************************
*/
#ifndef   GUI_SVG_NEMA_VG_HEADER
  #define GUI_SVG_NEMA_VG_HEADER           <nema_vg.h>
#endif

#ifndef   GUI_SVG_NEMA_VG_VERSION_HEADER
  #define GUI_SVG_NEMA_VG_VERSION_HEADER   <nema_vg_version.h>
#endif

#ifdef GUI_SVG_HAS_NEMAVG
  #include GUI_SVG_NEMA_VG_HEADER
  #include GUI_SVG_NEMA_VG_VERSION_HEADER
#endif

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       API version
*
**********************************************************************
*/
//
// API versions
//
#define NEMA_VG_VERSION_1_1_5   (0x010105)
#define NEMA_VG_VERSION_1_1_7   (0x010107)

#ifndef GUI_SVG_HAS_NEMAVG
  //
  // NemaVG API version
  //
  #define NEMA_VG_API_VERSION            NEMA_VG_VERSION_1_1_5    // Minimum v1.1.5 required
#endif

/*********************************************************************
*
*       Type definitions
*
**********************************************************************
*/
#ifndef GUI_SVG_HAS_NEMAVG
  //
  // Handle types
  //
  #define NEMA_VG_HANDLE         void *
  #define NEMA_VG_PATH_HANDLE    NEMA_VG_HANDLE
  #define NEMA_VG_PAINT_HANDLE   NEMA_VG_HANDLE
  #define NEMA_VG_GRAD_HANDLE    NEMA_VG_HANDLE
  //
  // General types
  //
  typedef uint32_t nema_tex_format_t;
  typedef float    nema_vg_float_t;
  typedef uint8_t  nema_tex_mode_t;
  //
  // Define private struct types.
  //
  DEFINE_TYPE_PRIVATE(nema_matrix3x3_t, sizeof(GUI_MATRIX));
  DEFINE_TYPE_PRIVATE(nema_cmdlist_t,   40);
  //
  // Define public struct types.
  //
  DEFINE_TYPE_PUBLIC(color_var_t,
    float r;
    float g;
    float b;
    float a;
  );
  DEFINE_TYPE_PUBLIC(nema_buffer_t,
    int       size;      
    int       fd;        
    void     *base_virt; 
    uintptr_t base_phys; 
  );
  DEFINE_TYPE_PUBLIC(nema_img_obj_t,
    nema_buffer_t bo;
    uint16_t      w;
    uint16_t      h;
    int           stride;
    uint32_t      color;
    uint8_t       format;
    uint8_t       sampling_mode;
  );
#endif

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
typedef int                  (GUI_SVG_NEMAVG_INIT_FUNC)               (void);
typedef void                 (GUI_SVG_NEMAVG_SETCLIP_FUNC)            (int32_t x, int32_t y, uint32_t w, uint32_t h);
typedef void                 (GUI_SVG_NEMAVG_BINDDSTTEX_FUNC)         (uintptr_t baseaddr_phys, uint32_t width, uint32_t height,
                                                                       nema_tex_format_t format, int32_t stride);
typedef nema_cmdlist_t       (GUI_SVG_NEMAVG_CLCREATESIZED_FUNC)      (int size_bytes);
typedef void                 (GUI_SVG_NEMAVG_CLBINDCIRCULAR_FUNC)     (nema_cmdlist_t *cl);
typedef void                 (GUI_SVG_NEMAVG_CLSUBMIT_FUNC)           (nema_cmdlist_t *cl);
typedef int                  (GUI_SVG_NEMAVG_CLWAIT_FUNC)             (nema_cmdlist_t *cl);
typedef void                 (GUI_SVG_NEMAVG_CLREWIND_FUNC)           (nema_cmdlist_t *cl);
typedef void                 (GUI_SVG_NEMAVG_CLDESTROY_FUNC)          (nema_cmdlist_t *cl);
typedef void                 (GUI_SVG_NEMAVG_VGINIT_FUNC)             (int width, int height);
typedef void                 (GUI_SVG_NEMAVG_VGDEINIT_FUNC)           (void);
typedef void                 (GUI_SVG_NEMAVG_HANDLELARGECOORDS_FUNC)  (uint8_t enable, uint8_t allow_internal_alloc);
typedef void                 (GUI_SVG_NEMAVG_SETBLEND_FUNC)           (uint32_t blend);
typedef void                 (GUI_SVG_NEMAVG_SETFILLRULE_FUNC)        (uint8_t fill_rule);
typedef void                 (GUI_SVG_NEMAVG_SETQUALITY_FUNC)         (uint8_t quality);
typedef uint32_t             (GUI_SVG_NEMAVG_DRAWPATH_FUNC)           (NEMA_VG_PATH_HANDLE path, NEMA_VG_PAINT_HANDLE paint);
typedef uint32_t             (GUI_SVG_NEMAVG_DRAWRECT_FUNC)           (float x, float y, float width, float height,
                                                                       nema_matrix3x3_t m, NEMA_VG_PAINT_HANDLE paint);
typedef NEMA_VG_PATH_HANDLE  (GUI_SVG_NEMAVG_PATHCREATE_FUNC)         (void);
typedef void                 (GUI_SVG_NEMAVG_PATHSETMATRIX_FUNC)      (NEMA_VG_PATH_HANDLE path, nema_matrix3x3_t m);
typedef void                 (GUI_SVG_NEMAVG_PATHSETSHAPE_FUNC)       (NEMA_VG_PATH_HANDLE path, const size_t seg_size,
                                                                       const uint8_t* seg, const size_t data_size, const nema_vg_float_t* data);
typedef void                 (GUI_SVG_NEMAVG_PATHDESTROY_FUNC)        (NEMA_VG_PATH_HANDLE path);
typedef NEMA_VG_GRAD_HANDLE  (GUI_SVG_NEMAVG_GRADCREATE_FUNC)         (void);
typedef void                 (GUI_SVG_NEMAVG_GRADDESTROY_FUNC)        (NEMA_VG_GRAD_HANDLE grad);
typedef void                 (GUI_SVG_NEMAVG_GRADSET_FUNC)            (NEMA_VG_GRAD_HANDLE grad, int stops_count, float *stops, color_var_t* colors);
typedef NEMA_VG_PAINT_HANDLE (GUI_SVG_NEMAVG_PAINTCREATE_FUNC)        (void);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETTYPE_FUNC)       (NEMA_VG_PAINT_HANDLE paint, uint8_t type);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETGRADLINEAR_FUNC) (NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE grad,
                                                                       float x0, float y0, float x1, float y1, nema_tex_mode_t sampling_mode);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETGRADRADIAL2_FUNC)(NEMA_VG_PAINT_HANDLE paint, NEMA_VG_GRAD_HANDLE grad, float x0, float y0,
                                                                       float rx, float ry, nema_tex_mode_t sampling_mode);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETOPACITY_FUNC)    (NEMA_VG_PAINT_HANDLE paint, float opacity);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETPAINTCOLOR_FUNC) (NEMA_VG_PAINT_HANDLE paint, uint32_t rgba);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETSTROKEWIDTH_FUNC)(NEMA_VG_PAINT_HANDLE paint, float stroke_width);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETTEX_FUNC)        (NEMA_VG_PAINT_HANDLE paint, nema_img_obj_t* tex);
typedef void                 (GUI_SVG_NEMAVG_PAINTSETTEXMATRIX_FUNC)  (NEMA_VG_PAINT_HANDLE paint, nema_matrix3x3_t m);
typedef void                 (GUI_SVG_NEMAVG_PAINTDESTROY_FUNC)       (NEMA_VG_PAINT_HANDLE paint);
typedef void                 (GUI_SVG_NEMAVG_STROKESETCAPSTYLE_FUNC)  (uint8_t start_cap_style, uint8_t end_cap_style);
typedef void                 (GUI_SVG_NEMAVG_STROKESETJOINSTYLE_FUNC) (uint8_t join_style);
typedef void                 (GUI_SVG_NEMAVG_STROKESETMITERLIMIT_FUNC)(float MiterLimit);

/*********************************************************************
*
*       API struct for NemaVG
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_NEMAVG_API_STRUCT
*
*  Description
*    Maps the required functions of the ThinkSilicon NemaVG API.
*
*    A structure of this type can be set with GUI_SVG_DRIVER_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the NemaVG code (meaning \c{GUI_SVG_HAS_NEMAVG} was not defined.
*
*    For more details about these functions, please refer to the NemaVG
*    API documentation.
*/
typedef struct {
  GUI_SVG_NEMAVG_INIT_FUNC                * pfInit;                    // Pointer to NemaVG function \c{nema_init()}
  GUI_SVG_NEMAVG_SETCLIP_FUNC             * pfSetClip;                 // Pointer to NemaVG function \c{nema_set_clip()}
  GUI_SVG_NEMAVG_BINDDSTTEX_FUNC          * pfBindDstTex;              // Pointer to NemaVG function \c{nema_bind_dst_tex()}
  GUI_SVG_NEMAVG_CLCREATESIZED_FUNC       * pfClCreateSized;           // Pointer to NemaVG function \c{nema_cl_create_sized()}
  GUI_SVG_NEMAVG_CLBINDCIRCULAR_FUNC      * pfClBindCircular;          // Pointer to NemaVG function \c{nema_cl_bind_circular()}
  GUI_SVG_NEMAVG_CLSUBMIT_FUNC            * pfClSubmit;                // Pointer to NemaVG function \c{nema_cl_submit()}
  GUI_SVG_NEMAVG_CLWAIT_FUNC              * pfClWait;                  // Pointer to NemaVG function \c{nema_cl_wait()}
  GUI_SVG_NEMAVG_CLREWIND_FUNC            * pfClRewind;                // Pointer to NemaVG function \c{nema_cl_rewind()}
  GUI_SVG_NEMAVG_CLDESTROY_FUNC           * pfClDestroy;               // Pointer to NemaVG function \c{nema_cl_destroy()}
  GUI_SVG_NEMAVG_VGINIT_FUNC              * pfVgInit;                  // Pointer to NemaVG function \c{nema_vg_init()}
  GUI_SVG_NEMAVG_VGDEINIT_FUNC            * pfVgDeinit;                // Pointer to NemaVG function \c{nema_vg_deinit()}
  GUI_SVG_NEMAVG_HANDLELARGECOORDS_FUNC   * pfVgHandleLargeCoords;     // Pointer to NemaVG function \c{nema_vg_handle_large_coords()}
  GUI_SVG_NEMAVG_SETBLEND_FUNC            * pfVgSetBlend;              // Pointer to NemaVG function \c{nema_vg_set_blend()}
  GUI_SVG_NEMAVG_SETFILLRULE_FUNC         * pfVgSetFillRule;           // Pointer to NemaVG function \c{nema_vg_set_fill_rule()}
  GUI_SVG_NEMAVG_SETQUALITY_FUNC          * pfVgSetQuality;            // Pointer to NemaVG function \c{nema_vg_set_quality()}
  GUI_SVG_NEMAVG_DRAWPATH_FUNC            * pfVgDrawPath;              // Pointer to NemaVG function \c{nema_vg_draw_path()}
  GUI_SVG_NEMAVG_DRAWRECT_FUNC            * pfVgDrawRect;              // Pointer to NemaVG function \c{nema_vg_draw_rect()}
  GUI_SVG_NEMAVG_PATHCREATE_FUNC          * pfVgPathCreate;            // Pointer to NemaVG function \c{nema_vg_path_create()}
  GUI_SVG_NEMAVG_PATHSETMATRIX_FUNC       * pfVgPathSetMatrix;         // Pointer to NemaVG function \c{nema_vg_path_set_matrix()}
  GUI_SVG_NEMAVG_PATHSETSHAPE_FUNC        * pfVgPathSetShape;          // Pointer to NemaVG function \c{nema_vg_path_set_shape()}
  GUI_SVG_NEMAVG_PATHDESTROY_FUNC         * pfVgPathDestroy;           // Pointer to NemaVG function \c{nema_vg_path_destroy()}
  GUI_SVG_NEMAVG_GRADCREATE_FUNC          * pfVgGradCreate;            // Pointer to NemaVG function \c{nema_vg_grad_create()}
  GUI_SVG_NEMAVG_GRADSET_FUNC             * pfVgGradSet;               // Pointer to NemaVG function \c{nema_vg_grad_set()}
  GUI_SVG_NEMAVG_GRADDESTROY_FUNC         * pfVgGradDestroy;           // Pointer to NemaVG function \c{nema_vg_grad_destroy()}
  GUI_SVG_NEMAVG_PAINTCREATE_FUNC         * pfVgPaintCreate;           // Pointer to NemaVG function \c{nema_vg_paint_create()}
  GUI_SVG_NEMAVG_PAINTSETTYPE_FUNC        * pfVgPaintSetType;          // Pointer to NemaVG function \c{nema_vg_paint_set_type()}
  GUI_SVG_NEMAVG_PAINTSETGRADLINEAR_FUNC  * pfVgPaintSetGradLinear;    // Pointer to NemaVG function \c{nema_vg_paint_set_grad_linear()}
  GUI_SVG_NEMAVG_PAINTSETGRADRADIAL2_FUNC * pfVgPaintSetGradRadial2;   // Pointer to NemaVG function \c{nema_vg_paint_set_grad_radial2()}
  GUI_SVG_NEMAVG_PAINTSETOPACITY_FUNC     * pfVgPaintSetOpacity;       // Pointer to NemaVG function \c{nema_vg_paint_set_opacity()}
  GUI_SVG_NEMAVG_PAINTSETPAINTCOLOR_FUNC  * pfVgPaintSetPaintColor;    // Pointer to NemaVG function \c{nema_vg_paint_set_paint_color()}
  GUI_SVG_NEMAVG_PAINTSETSTROKEWIDTH_FUNC * pfVgPaintSetStrokeWidth;   // Pointer to NemaVG function \c{nema_vg_paint_set_stroke_width()}
  GUI_SVG_NEMAVG_PAINTSETTEX_FUNC         * pfVgPaintSetTex;           // Pointer to NemaVG function \c{nema_vg_paint_set_tex()}
  GUI_SVG_NEMAVG_PAINTSETTEXMATRIX_FUNC   * pfVgPaintSetTexMatrix;     // Pointer to NemaVG function \c{nema_vg_paint_set_tex_matrix()}
  GUI_SVG_NEMAVG_PAINTDESTROY_FUNC        * pfVgPaintDestroy;          // Pointer to NemaVG function \c{nema_vg_paint_destroy()}
  GUI_SVG_NEMAVG_STROKESETCAPSTYLE_FUNC   * pfVgStrokeSetCapStyle;     // Pointer to NemaVG function \c{nema_vg_stroke_set_cap_style()}
  GUI_SVG_NEMAVG_STROKESETJOINSTYLE_FUNC  * pfVgStrokeSetJoinStyle;    // Pointer to NemaVG function \c{nema_vg_stroke_set_join_style()}
  GUI_SVG_NEMAVG_STROKESETMITERLIMIT_FUNC * pfVgStrokeSetMiterLimit;   // Pointer to NemaVG function \c{nema_vg_stroke_set_miter_limit()}
} GUI_SVG_NEMAVG_API_STRUCT;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// These functions are only available since v1.1.7, make sure they are
// not referenced in GUI_SVG_DECLARE_NEMAVG_API if unavailable.
//
#if (NEMA_VG_API_VERSION < NEMA_VG_VERSION_1_1_7)
  #define NEMA_VG_FUNCTIONS_1_1_7         NULL,    \
                                          NULL,    \
                                          NULL,
#else
  #define NEMA_VG_FUNCTIONS_1_1_7         nema_vg_stroke_set_cap_style,    \
                                          nema_vg_stroke_set_join_style,   \
                                          nema_vg_stroke_set_miter_limit,
#endif

/*********************************************************************
*
*       GUI_SVG_DECLARE_NEMAVG_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_NEMAVG_API_STRUCT
*    filled with the correct function pointers.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*/
#define GUI_SVG_DECLARE_NEMAVG_API(VAR_NAME)                 \
  static const GUI_SVG_NEMAVG_API_STRUCT VAR_NAME = {        \
    nema_init,                                               \
    nema_set_clip,                                           \
    nema_bind_dst_tex,                                       \
    nema_cl_create_sized,                                    \
    nema_cl_bind_circular,                                   \
    nema_cl_submit,                                          \
    nema_cl_wait,                                            \
    nema_cl_rewind,                                          \
    nema_cl_destroy,                                         \
    nema_vg_init,                                            \
    (GUI_SVG_NEMAVG_VGDEINIT_FUNC *)nema_vg_deinit,          \
    nema_vg_handle_large_coords,                             \
    nema_vg_set_blend,                                       \
    nema_vg_set_fill_rule,                                   \
    nema_vg_set_quality,                                     \
    nema_vg_draw_path,                                       \
    nema_vg_draw_rect,                                       \
    (GUI_SVG_NEMAVG_PATHCREATE_FUNC *)nema_vg_path_create,   \
    nema_vg_path_set_matrix,                                 \
    nema_vg_path_set_shape,                                  \
    nema_vg_path_destroy,                                    \
    nema_vg_grad_create,                                     \
    nema_vg_grad_set,                                        \
    nema_vg_grad_destroy,                                    \
    (GUI_SVG_NEMAVG_PAINTCREATE_FUNC *)nema_vg_paint_create, \
    nema_vg_paint_set_type,                                  \
    nema_vg_paint_set_grad_linear,                           \
    nema_vg_paint_set_grad_radial2,                          \
    nema_vg_paint_set_opacity,                               \
    nema_vg_paint_set_paint_color,                           \
    nema_vg_paint_set_stroke_width,                          \
    nema_vg_paint_set_tex,                                   \
    nema_vg_paint_set_tex_matrix,                            \
    nema_vg_paint_destroy,                                   \
    NEMA_VG_FUNCTIONS_1_1_7                                  \
  }

#if defined(__cplusplus)
}
#endif

#endif /* GUI_SVG_NEMAVG_H */

/*************************** End of file ****************************/
