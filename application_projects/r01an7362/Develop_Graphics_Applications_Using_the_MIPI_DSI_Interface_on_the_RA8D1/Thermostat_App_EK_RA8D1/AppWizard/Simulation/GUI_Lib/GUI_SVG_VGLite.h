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
File        : GUI_SVG_VGLite.h
Purpose     : VGLite interface for SVG
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SVG_VGLITE_H
#define GUI_SVG_VGLITE_H

#include "GUI_ConfDefaults.h"
#include "GUI_SVG_Global.h"

/*********************************************************************
*
*       VGLite header files.
*
**********************************************************************
*/

#ifndef   GUI_SVG_VGLITE_HEADER
  #define GUI_SVG_VGLITE_HEADER      <vg_lite.h>
#endif

#ifndef   GUI_SVG_VGLITE_OS_HEADER
  #define GUI_SVG_VGLITE_OS_HEADER   <vg_lite_os.h>
#endif

#ifdef GUI_SVG_HAS_VGLITE
  #include GUI_SVG_VGLITE_HEADER
  #include GUI_SVG_VGLITE_OS_HEADER
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
#ifndef GUI_SVG_HAS_VGLITE
  //
  // Define primitive types.
  //
  #define vg_lite_error_t                       int
  #define vg_lite_format_t                      int
  #define vg_lite_quality_t                     int
  #define vg_lite_float_t                       float
  #define vg_lite_color_t                       uint32_t
  #define vg_lite_draw_path_type_t              int
  #define vg_lite_buffer_layout_t               int
  #define vg_lite_buffer_transparency_mode_t    int
  #define vg_lite_buffer_format_t               int
  #define vg_lite_buffer_image_mode_t           int
  #define vg_lite_radial_gradient_spreadmode_t  int
  #define vg_lite_cap_style_t                   int
  #define vg_lite_join_style_t                  int
  #define vg_lite_quality_t                     int
  #define vg_lite_blend_t                       int
  #define vg_lite_fill_t                        int
  #define vg_lite_filter_t                      int
  #define vg_lite_feature_t                     int
  //
  // Define private structure types.
  //
  DEFINE_TYPE_PRIVATE(vg_lite_path_t,                   72);
  DEFINE_TYPE_PRIVATE(vg_lite_yuvinfo_t,                52);
  DEFINE_TYPE_PRIVATE(vg_lite_matrix_t,                 36);
  DEFINE_TYPE_PRIVATE(vg_lite_linear_gradient_ext_t, 10432);
  DEFINE_TYPE_PRIVATE(vg_lite_radial_gradient_t,     10440);
  //
  // Define structs with public members.
  //
  DEFINE_TYPE_PUBLIC(vg_lite_buffer_t,
    int32_t                            width;
    int32_t                            height;
    int32_t                            stride;
    vg_lite_buffer_layout_t            tiled;
    vg_lite_buffer_format_t            format;
    void                             * handle;
    void                             * memory;
    uint32_t                           address;
    vg_lite_yuvinfo_t                  yuv;
    vg_lite_buffer_image_mode_t        image_mode;
    vg_lite_buffer_transparency_mode_t transparency_mode;
  );
  DEFINE_TYPE_PUBLIC(vg_lite_color_ramp_t,
    vg_lite_float_t stop;
    vg_lite_float_t red;
    vg_lite_float_t green;
    vg_lite_float_t blue;
    vg_lite_float_t alpha;
  );
  DEFINE_TYPE_PUBLIC(vg_lite_linear_gradient_parameter_t,
    vg_lite_float_t X0;
    vg_lite_float_t Y0;
    vg_lite_float_t X1;
    vg_lite_float_t Y1;
  );
  DEFINE_TYPE_PUBLIC(vg_lite_radial_gradient_parameter_t,
    vg_lite_float_t cx;
    vg_lite_float_t cy;
    vg_lite_float_t r;
    vg_lite_float_t fx;
    vg_lite_float_t fy;
  );
#endif

/*********************************************************************
*
*       Function prototypes
*
**********************************************************************
*/
typedef vg_lite_error_t    (GUI_SVG_VGLITE_BLIT_FUNC)            (vg_lite_buffer_t * target, vg_lite_buffer_t * source,
                                                                  vg_lite_matrix_t * matrix, vg_lite_blend_t blend,
                                                                  vg_lite_color_t color, vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_DRAWPATH_FUNC)        (vg_lite_buffer_t *target, vg_lite_path_t *path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t *matrix,
                                                                  vg_lite_blend_t blend, vg_lite_color_t color);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_INITPATH_FUNC)        (vg_lite_path_t *path, vg_lite_format_t data_format,
                                                                  vg_lite_quality_t quality, uint32_t path_length,
                                                                  void *path_data, vg_lite_float_t min_x,
                                                                  vg_lite_float_t min_y, vg_lite_float_t max_x,
                                                                  vg_lite_float_t max_y);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_CLEARPATH_FUNC)       (vg_lite_path_t *path);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_SETPATHDRAWTYPE_FUNC) (vg_lite_path_t *path, vg_lite_draw_path_type_t path_type);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_SETSTROKE_FUNC)       (vg_lite_path_t *path, vg_lite_cap_style_t stroke_cap_style,
                                                                  vg_lite_join_style_t stroke_join_style,
                                                                  vg_lite_float_t stroke_line_width,
                                                                  vg_lite_float_t stroke_miter_limit,
                                                                  vg_lite_float_t *stroke_dash_pattern,
                                                                  uint32_t stroke_dash_pattern_count,
                                                                  vg_lite_float_t stroke_dash_phase,
                                                                  vg_lite_color_t stroke_color);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_UPDATESTROKE_FUNC)    (vg_lite_path_t *path);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_SETLINGRAD_FUNC)      (vg_lite_linear_gradient_ext_t *grad, uint32_t count,
                                                                  vg_lite_color_ramp_t *vg_color_ramp,
                                                                  vg_lite_linear_gradient_parameter_t linear_gradient,
                                                                  vg_lite_radial_gradient_spreadmode_t spread_mode,
                                                                  uint8_t color_ramp_premultiplied);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_UPDATELINGRAD_FUNC)   (vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_matrix_t * (GUI_SVG_VGLITE_GETLINGRADMATRIX_FUNC)(vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_DRAWLINGRAD_FUNC)     (vg_lite_buffer_t * target, vg_lite_path_t * path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t * path_matrix,
                                                                  vg_lite_linear_gradient_ext_t *grad,
                                                                  vg_lite_color_t paint_color, vg_lite_blend_t blend,
                                                                  vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_CLEARLINGRAD_FUNC)    (vg_lite_linear_gradient_ext_t *grad);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_SETRADGRAD_FUNC)      (vg_lite_radial_gradient_t *grad, uint32_t count,
                                                                  vg_lite_color_ramp_t *vgColorRamp,
                                                                  vg_lite_radial_gradient_parameter_t radialGradient,
                                                                  vg_lite_radial_gradient_spreadmode_t spreadMode,
                                                                  uint8_t colorRampPremultiplied);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_UPDATERADGRAD_FUNC)   (vg_lite_radial_gradient_t *grad);
typedef vg_lite_matrix_t * (GUI_SVG_VGLITE_GETRADGRADMATRIX_FUNC)(vg_lite_radial_gradient_t *grad);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_DRAWRADGRAD_FUNC)     (vg_lite_buffer_t * target, vg_lite_path_t * path,
                                                                  vg_lite_fill_t fill_rule, vg_lite_matrix_t * path_matrix,
                                                                  vg_lite_radial_gradient_t *grad, vg_lite_color_t paint_color,
                                                                  vg_lite_blend_t blend, vg_lite_filter_t filter);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_CLEARRADGRAD_FUNC)    (vg_lite_radial_gradient_t *grad);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_ENABLESCISSOR_FUNC)   (void);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_DISABLESCISSOR_FUNC)  (void);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_SETSCISSOR_FUNC)      (int32_t x, int32_t y, int32_t width, int32_t height);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_FLUSH_FUNC)           (void);
typedef vg_lite_error_t    (GUI_SVG_VGLITE_FINISH_FUNC)          (void);
typedef uint32_t           (GUI_SVG_VGLITE_QUERY_FEATURE_FUNC)   (vg_lite_feature_t feature);

/*********************************************************************
*
*       API struct for VGLite
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_VGLITE_API_STRUCT
*
*  Description
*    Maps the required functions of the Vivante VGLite API.
*
*    A structure of this type can be set with GUI_SVG_DRIVER_BindAPI()
*    when a precompiled emWin library is used, that was compiled without
*    the VGLite code (meaning \c{GUI_SVG_HAS_VGLITE} was not defined.
*
*    For more details about what each function does, please refer to the VGLite
*    API documentation.
*/
typedef struct {
  GUI_SVG_VGLITE_BLIT_FUNC              * pfBlit;             // Pointer to VGLite function \c{vg_lite_blit()}
  GUI_SVG_VGLITE_DRAWPATH_FUNC          * pfDrawPath;         // Pointer to VGLite function \c{vg_lite_draw()}
  GUI_SVG_VGLITE_INITPATH_FUNC          * pfInitPath;         // Pointer to VGLite function \c{vg_lite_init_path()}
  GUI_SVG_VGLITE_INITPATH_FUNC          * pfInitArcPath;      // Pointer to VGLite function \c{vg_lite_init_arc_path()}
  GUI_SVG_VGLITE_CLEARPATH_FUNC         * pfClearPath;        // Pointer to VGLite function \c{vg_lite_clear_path()}
  GUI_SVG_VGLITE_SETPATHDRAWTYPE_FUNC   * pfSetDrawPathType;  // Pointer to VGLite function \c{vg_lite_set_draw_path_type()}
  GUI_SVG_VGLITE_SETSTROKE_FUNC         * pfSetStroke;        // Pointer to VGLite function \c{vg_lite_set_stroke()}
  GUI_SVG_VGLITE_UPDATESTROKE_FUNC      * pfUpdateStroke;     // Pointer to VGLite function \c{vg_lite_update_stroke()}
  GUI_SVG_VGLITE_SETLINGRAD_FUNC        * pfSetLinGrad;       // Pointer to VGLite function \c{vg_lite_set_linear_grad()}
  GUI_SVG_VGLITE_UPDATELINGRAD_FUNC     * pfUpdateLinGrad;    // Pointer to VGLite function \c{vg_lite_update_linear_grad()}
  GUI_SVG_VGLITE_GETLINGRADMATRIX_FUNC  * pfGetLinGradMatrix; // Pointer to VGLite function \c{vg_lite_get_linear_grad_matrix()}
  GUI_SVG_VGLITE_DRAWLINGRAD_FUNC       * pfDrawLinGrad;      // Pointer to VGLite function \c{vg_lite_draw_linear_gradient()}
  GUI_SVG_VGLITE_CLEARLINGRAD_FUNC      * pfClearLinGrad;     // Pointer to VGLite function \c{vg_lite_clear_linear_grad()}
  GUI_SVG_VGLITE_SETRADGRAD_FUNC        * pfSetRadGrad;       // Pointer to VGLite function \c{vg_lite_set_rad_grad()}
  GUI_SVG_VGLITE_UPDATERADGRAD_FUNC     * pfUpdateRadGrad;    // Pointer to VGLite function \c{vg_lite_update_rad_grad()}
  GUI_SVG_VGLITE_GETRADGRADMATRIX_FUNC  * pfGetRadGradMatrix; // Pointer to VGLite function \c{vg_lite_get_rad_grad_matrix()}
  GUI_SVG_VGLITE_DRAWRADGRAD_FUNC       * pfDrawRadGrad;      // Pointer to VGLite function \c{vg_lite_draw_radial_gradient()}
  GUI_SVG_VGLITE_CLEARRADGRAD_FUNC      * pfClearRadGrad;     // Pointer to VGLite function \c{vg_lite_clear_rad_grad()}
  GUI_SVG_VGLITE_ENABLESCISSOR_FUNC     * pfEnableScissor;    // Pointer to VGLite function \c{vg_lite_enable_scissor()}
  GUI_SVG_VGLITE_DISABLESCISSOR_FUNC    * pfDisableScissor;   // Pointer to VGLite function \c{vg_lite_disable_scissor()}
  GUI_SVG_VGLITE_SETSCISSOR_FUNC        * pfSetScissor;       // Pointer to VGLite function \c{vg_lite_set_scissor()}
  GUI_SVG_VGLITE_FLUSH_FUNC             * pfFlush;            // Pointer to VGLite function \c{vg_lite_flush()}
  GUI_SVG_VGLITE_FINISH_FUNC            * pfFinish;           // Pointer to VGLite function \c{vg_lite_finish()}
  GUI_SVG_VGLITE_QUERY_FEATURE_FUNC     * pfQueryFeature;     // Pointer to VGLite function \c{vg_lite_query_feature()}
} GUI_SVG_VGLITE_API_STRUCT;

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SVG_DECLARE_VGLITE_API
*
*  Description
*    Macro to define a structure of the type GUI_SVG_VGLITE_API_STRUCT
*    filled with the correct function pointers.
* 
*  Parameters
*    VAR_NAME: Identifier name to be used for the structure variable.
*/
#define GUI_SVG_DECLARE_VGLITE_API(VAR_NAME)          \
  static const GUI_SVG_VGLITE_API_STRUCT VAR_NAME = { \
    vg_lite_blit,                                     \
    vg_lite_draw,                                     \
    vg_lite_init_path,                                \
    vg_lite_init_arc_path,                            \
    vg_lite_clear_path,                               \
    vg_lite_set_draw_path_type,                       \
    vg_lite_set_stroke,                               \
    vg_lite_update_stroke,                            \
    vg_lite_set_linear_grad,                          \
    vg_lite_update_linear_grad,                       \
    vg_lite_get_linear_grad_matrix,                   \
    vg_lite_draw_linear_gradient,                     \
    vg_lite_clear_linear_grad,                        \
    vg_lite_set_rad_grad,                             \
    vg_lite_update_rad_grad,                          \
    vg_lite_get_rad_grad_matrix,                      \
    vg_lite_draw_radial_gradient,                     \
    vg_lite_clear_rad_grad,                           \
    vg_lite_enable_scissor,                           \
    vg_lite_disable_scissor,                          \
    vg_lite_set_scissor,                              \
    vg_lite_flush,                                    \
    vg_lite_finish,                                   \
    vg_lite_query_feature,                            \
  }

#if defined(__cplusplus)
}
#endif

#endif /* GUI_SVG_VGLITE_H */

/*************************** End of file ****************************/
