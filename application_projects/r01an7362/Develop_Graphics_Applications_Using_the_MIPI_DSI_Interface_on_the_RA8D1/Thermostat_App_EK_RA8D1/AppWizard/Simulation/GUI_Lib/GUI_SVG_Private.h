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
File        : GUI_SVG_Private.h
Purpose     : Private header for SVG support
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_SVG_PRIVATE_H
#define GUI_SVG_PRIVATE_H

#include "GUI_Private.h"
#include "GUI_ARRAY.h"
//
// Include SVG drivers
//
#include "GUI_SVG_OpenVG.h"
#include "GUI_SVG_VGLite.h"
#include "GUI_SVG_NemaVG.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       XML parsing module (not published for now)
*
**********************************************************************
*/
typedef GUI_HMEM   GUI_XML_Handle;

#define MAKE_STRUCT(NAME)  typedef struct NAME NAME; \
                           struct NAME 

// Keep structure the same as XML_ATTRIBUTE (private)
MAKE_STRUCT(GUI_XML_ATTRIBUTE) {
  const GUI_XML_ATTRIBUTE * pNext;
  const char              * sAttrib;
  const char              * sValue;
};

typedef void GUI_XML_DOCUMENT_CALLBACK(GUI_XML_Handle hXML, void * pUser);
typedef void GUI_XML_ELEMENT_CALLBACK (GUI_XML_Handle hXML, void * pUser, const char * sElement, const GUI_XML_ATTRIBUTE * pFirstAttrib);
typedef void GUI_XML_STRING_CALLBACK  (GUI_XML_Handle hXML, void * pUser, const char * s);
typedef void GUI_XML_ERROR_CALLBACK   (GUI_XML_Handle hXML, void * pUser, const char * sError, U32 Offset);

typedef struct {
  GUI_XML_DOCUMENT_CALLBACK * cbDocumentOpen;   // Called when XML doc opened.
  GUI_XML_DOCUMENT_CALLBACK * cbDocumentClose;  // Called when XML doc closed.
  GUI_XML_ELEMENT_CALLBACK  * cbElementOpen;    // Called when XML element was opened, e.g. <elem>
  GUI_XML_STRING_CALLBACK   * cbElementClose;   // Called when XML element was closed, e.g. </elem>
  GUI_XML_STRING_CALLBACK   * cbCharacter;      // Called when characters within an element were read.
  GUI_XML_STRING_CALLBACK   * cbCharData;       // Called when CDATA was read.
  GUI_XML_ERROR_CALLBACK    * cbError;          // Called when an error occured due to a malformed XML doc.
} GUI_XML_CALLBACKS;

typedef enum {
  GUI_XML_WHITELIST_ELEMENT = 0,
  GUI_XML_WHITELIST_ATTRIBUTE,
} GUI_XML_WHITELIST;

GUI_XML_Handle GUI_XML_Create       (const void * pFile, U32 FileSize);
GUI_XML_Handle GUI_XML_CreateEx     (GUI_SVG_GET_DATA_FUNC * pfGetData, void * p);
U32            GUI_XML_GetOffset    (GUI_XML_Handle hXML);
void           GUI_XML_SetBufferSize(GUI_XML_Handle hXML, U32 NumBytes);
void           GUI_XML_SetCallbacks (GUI_XML_Handle hXML, const GUI_XML_CALLBACKS * pCallbacks);
void           GUI_XML_SetUserData  (GUI_XML_Handle hXML, void * p);
void           GUI_XML_SetWhitelist (GUI_XML_Handle hXML, GUI_XML_WHITELIST Id, const char ** paWhitelist, unsigned NumItems);
void           GUI_XML_Delete       (GUI_XML_Handle hXML);
int            GUI_XML_Parse        (GUI_XML_Handle hXML);

#define GUI_XML_BEGIN_ITERATE_ATTRIBS(ATTRIBUTE_PTR)   \
  {                                                    \
    const GUI_XML_ATTRIBUTE * pAttrib = (const GUI_XML_ATTRIBUTE *)ATTRIBUTE_PTR; \
    const char              * sValue;                  \
    if (pAttrib) {                                     \
      do {                                             \
        sValue = pAttrib->sValue;                      \
        GUI_USE_PARA(sValue);
#define GUI_XML_END_ITERATE_ATTRIBS()            \
        pAttrib = pAttrib->pNext;                \
      } while (pAttrib);                         \
    }                                            \
  }

/*********************************************************************
*
*       CSS parsing module
* 
*  CSS parsing is as primitive as it needs to be, not intended for
*  publishing.
*
**********************************************************************
*/
typedef enum {
  GUI_CSS_SELECTOR_INVALID, // any other selectors that aren't supported
  GUI_CSS_SELECTOR_ALL,     // * { ... }
  GUI_CSS_SELECTOR_ID,      // #someId { ... }
  GUI_CSS_SELECTOR_CLASS,   // .someClass { ... }
} GUI_CSS_SELECTOR_TYPE;
//
// Keep structure the same as (GUI_)XML_ATTRIBUTE
// The structs should all be convertible.
//
MAKE_STRUCT(GUI_CSS_ATTRIBUTE) {
  const GUI_CSS_ATTRIBUTE * pNext;
  char                    * sAttrib;
  char                    * sValue;
};

//
// Keep structure the same as CSS_SELECTOR
// The structs should all be convertible.
//
MAKE_STRUCT(GUI_CSS_SELECTOR) {
  /* const GUI_CSS_SELECTOR * pNextSelector; */
  GUI_CSS_SELECTOR_TYPE    Type;
  const char             * sTarget;
};

//
// One CSS rule, this means the declaraction block {}
// with the selector(s).
//
MAKE_STRUCT(GUI_CSS_RULE) {
  GUI_CSS_SELECTOR          Selector;
  const GUI_CSS_ATTRIBUTE * pFirstAttrib;
};

GUI_ARRAY GUI_CSS__Parse              (const char * sCSS);
void      GUI_CSS__Delete             (GUI_ARRAY haCss);
GUI_ARRAY GUI_CSS__GetMatchingRules   (GUI_ARRAY haCss, const char * sElement, const GUI_XML_ATTRIBUTE * pFirstAttrib);
void      GUI_CSS__ReadAttribValuePair(const char ** psCSS, char ** ppAttrib, char ** ppValue);

#define GUI_CSS_BEGIN_ITERATE_ATTRIBS   GUI_XML_BEGIN_ITERATE_ATTRIBS
#define GUI_CSS_END_ITERATE_ATTRIBS     GUI_XML_END_ITERATE_ATTRIBS

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Type aliases
//
#define F32  float
#define F64  double
//
// Physical display size.
//
#define LCD_XSIZE     ((LCD_GetSwapXY()) ? LCD_GetYSize() : LCD_GetXSize())
#define LCD_YSIZE     ((LCD_GetSwapXY()) ? LCD_GetXSize() : LCD_GetYSize())
#define LCD_XSIZE_F   ((F32)LCD_XSIZE)
#define LCD_YSIZE_F   ((F32)LCD_YSIZE)

#define SVG_LOCK(h)    if (h) {                \
                         {                     \
                           GUI_SVG_Obj * pObj; \
                           GUI_LOCK();         \
                           pObj = (GUI_SVG_Obj *)GUI_LOCK_H(h)

#define SVG_UNLOCK()       GUI_UNLOCK_H(pObj); \
                           GUI_UNLOCK();       \
                         }                     \
                       }
//
// Iterating over a GUI_ARRAY, in the fashion of C++ range-based for loops.
// The syntax is very similar:
//   for                    (Item  i : ItemArray) {...}
//   GUI_ARRAY_BEGIN_FOREACH(Item, i,  ItemArray)  ...
// 
// Notes: The first  if (pUnlock)... is done so that a 'continue' unlocks the last item.
//        The second if (pUnlock)... is done so that a 'break' unlocks the last item.
//
#define GUI_ARRAY_BEGIN_FOREACH_ABSTRACT(TYPE, VAR, ARR, LOOP)                          \
                               {                                                        \
                                 unsigned i, NumItems;                                  \
                                 TYPE * pUnlock = NULL;                                 \
                                 TYPE * VAR;                                            \
                                 if (ARR) {                                             \
                                   NumItems = GUI_ARRAY_GetNumItems(ARR);               \
                                   for LOOP {                                           \
                                     if (pUnlock) {                                     \
                                       GUI_UNLOCK_H(pUnlock);                           \
                                     }                                                  \
                                     pUnlock = VAR = (TYPE *)GUI_ARRAY_GetpItemLocked(ARR, i);
#define GUI_ARRAY_END_FOREACH()                                                         \
                                     GUI_UNLOCK_H(pUnlock);                             \
                                   }                                                    \
                                   if (pUnlock) {                                       \
                                     GUI_UNLOCK_H(pUnlock);                             \
                                   }                                                    \
                                 }                                                      \
                               }
//
// Allows iterating forward and backward
//
#define GUI_ARRAY_BEGIN_FOREACH(TYPE, VAR, ARR)          GUI_ARRAY_BEGIN_FOREACH_ABSTRACT(TYPE, VAR, ARR, (i = 0; i < NumItems; i++))
#define GUI_ARRAY_BEGIN_FOREACH_REVERSE(TYPE, VAR, ARR)  GUI_ARRAY_BEGIN_FOREACH_ABSTRACT(TYPE, VAR, ARR, (i = NumItems - 1; i >= 0; i--))

#define GUI_ARRAY_ADD_ITEM(ARR, VAR)   GUI_ARRAY_AddItem(ARR, &VAR, sizeof(VAR))
#define GUI_ARRAY_DELETE(ARR)          GUI_ARRAY_Delete(ARR); ARR = 0

#define INIT_OBJ(TYPE, PTR)         GUI_SVG__Init_##TYPE(PTR)
#define DECLARE_CONSTRUCTOR(TYPE)   void GUI_SVG__Init_##TYPE(TYPE * p)

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef   GUI_SVG_MAX_NUM_DASH_ITEMS
  #define GUI_SVG_MAX_NUM_DASH_ITEMS    10
#endif

/*********************************************************************
*
*       Enum types
*
**********************************************************************
*/
typedef enum {
  GUI_SVG_CAP_BUTT   = 0x1700,
  GUI_SVG_CAP_ROUND  = 0x1701,
  GUI_SVG_CAP_SQUARE = 0x1702,
} GUI_SVG_CAP_STYLE;

typedef enum {
  GUI_SVG_JOIN_MITER = 0x1800,
  GUI_SVG_JOIN_ROUND = 0x1801,
  GUI_SVG_JOIN_BEVEL = 0x1802,
} GUI_SVG_JOIN_STYLE;

typedef enum {
  GUI_SVG_STROKE_PATH = (1 << 0),
  GUI_SVG_FILL_PATH   = (1 << 1),
} GUI_SVG_PAINT_MODE;

typedef enum {
  GUI_SVG_CLOSE_PATH  = ( 0 << 1),
  GUI_SVG_MOVE_TO     = ( 1 << 1),
  GUI_SVG_LINE_TO     = ( 2 << 1),
  GUI_SVG_HLINE_TO    = ( 3 << 1),
  GUI_SVG_VLINE_TO    = ( 4 << 1),
  GUI_SVG_QUAD_TO     = ( 5 << 1),
  GUI_SVG_CUBIC_TO    = ( 6 << 1),
  GUI_SVG_SQUAD_TO    = ( 7 << 1),
  GUI_SVG_SCUBIC_TO   = ( 8 << 1),
  GUI_SVG_SCCWARC_TO  = ( 9 << 1),
  GUI_SVG_SCWARC_TO   = (10 << 1),
  GUI_SVG_LCCWARC_TO  = (11 << 1),
  GUI_SVG_LCWARC_TO   = (12 << 1),
} GUI_SVG_PATH_SEGMENT;

typedef enum {
  GUI_SVG_ABSOLUTE = 0,
  GUI_SVG_RELATIVE = 1,
} GUI_SVG_PATH_ABSREL;

typedef enum {
  GUI_SVG_CLOSE          = GUI_SVG_CLOSE_PATH,
  GUI_SVG_MOVE_TO_ABS    = GUI_SVG_MOVE_TO    | GUI_SVG_ABSOLUTE,
  GUI_SVG_MOVE_TO_REL    = GUI_SVG_MOVE_TO    | GUI_SVG_RELATIVE,
  GUI_SVG_LINE_TO_ABS    = GUI_SVG_LINE_TO    | GUI_SVG_ABSOLUTE,
  GUI_SVG_LINE_TO_REL    = GUI_SVG_LINE_TO    | GUI_SVG_RELATIVE,
  GUI_SVG_HLINE_TO_ABS   = GUI_SVG_HLINE_TO   | GUI_SVG_ABSOLUTE,
  GUI_SVG_HLINE_TO_REL   = GUI_SVG_HLINE_TO   | GUI_SVG_RELATIVE,
  GUI_SVG_VLINE_TO_ABS   = GUI_SVG_VLINE_TO   | GUI_SVG_ABSOLUTE,
  GUI_SVG_VLINE_TO_REL   = GUI_SVG_VLINE_TO   | GUI_SVG_RELATIVE,
  GUI_SVG_QUAD_TO_ABS    = GUI_SVG_QUAD_TO    | GUI_SVG_ABSOLUTE,
  GUI_SVG_QUAD_TO_REL    = GUI_SVG_QUAD_TO    | GUI_SVG_RELATIVE,
  GUI_SVG_CUBIC_TO_ABS   = GUI_SVG_CUBIC_TO   | GUI_SVG_ABSOLUTE,
  GUI_SVG_CUBIC_TO_REL   = GUI_SVG_CUBIC_TO   | GUI_SVG_RELATIVE,
  GUI_SVG_SQUAD_TO_ABS   = GUI_SVG_SQUAD_TO   | GUI_SVG_ABSOLUTE,
  GUI_SVG_SQUAD_TO_REL   = GUI_SVG_SQUAD_TO   | GUI_SVG_RELATIVE,
  GUI_SVG_SCUBIC_TO_ABS  = GUI_SVG_SCUBIC_TO  | GUI_SVG_ABSOLUTE,
  GUI_SVG_SCUBIC_TO_REL  = GUI_SVG_SCUBIC_TO  | GUI_SVG_RELATIVE,
  GUI_SVG_SCCWARC_TO_ABS = GUI_SVG_SCCWARC_TO | GUI_SVG_ABSOLUTE,
  GUI_SVG_SCCWARC_TO_REL = GUI_SVG_SCCWARC_TO | GUI_SVG_RELATIVE,
  GUI_SVG_SCWARC_TO_ABS  = GUI_SVG_SCWARC_TO  | GUI_SVG_ABSOLUTE,
  GUI_SVG_SCWARC_TO_REL  = GUI_SVG_SCWARC_TO  | GUI_SVG_RELATIVE,
  GUI_SVG_LCCWARC_TO_ABS = GUI_SVG_LCCWARC_TO | GUI_SVG_ABSOLUTE,
  GUI_SVG_LCCWARC_TO_REL = GUI_SVG_LCCWARC_TO | GUI_SVG_RELATIVE,
  GUI_SVG_LCWARC_TO_ABS  = GUI_SVG_LCWARC_TO  | GUI_SVG_ABSOLUTE,
  GUI_SVG_LCWARC_TO_REL  = GUI_SVG_LCWARC_TO  | GUI_SVG_RELATIVE,
} GUI_SVG_PATH_COMMAND;

typedef enum {
  GUI_SVG_FILL_EVEN_ODD = 0x1900,
  GUI_SVG_FILL_NON_ZERO = 0x1901,
} GUI_SVG_FILL_RULE;

typedef enum {
  GUI_SVG_GRADIENT_OBJ_BOUND_BOX,
  GUI_SVG_GRADIENT_USER_SPACE_ON_USE,
} GUI_SVG_GRADIENT_UNITS;

typedef enum {
  GUI_SVG_GRADIENT_PAD     = 0x1C00,
  GUI_SVG_GRADIENT_REPEAT  = 0x1C01,
  GUI_SVG_GRADIENT_REFLECT = 0x1C02,
} GUI_SVG_GRADIENT_SPREAD;

typedef enum {
  GUI_SVG_LINEAR_GRADIENT,
  GUI_SVG_RADIAL_GRADIENT,
} GUI_SVG_GRADIENT_TYPE;

typedef enum {
  GUI_SVG_GRADIENT_REF_STOP_ARRAY = (1 << 0), // If true than haStops array is referenced, not owned.
} GUI_SVG_GRADIENT_FLAGS;

typedef enum {
  GUI_SVG_PARSE_INIT,
  GUI_SVG_PARSE_DRAW,
} GUI_SVG_PARSING_STRATEGY;

/*********************************************************************
*
*       Structure types
*
**********************************************************************
*/
//
// Abstract buffer type
//
typedef struct {
  void                     * pData;
  int	                     Width, Height;
  int	                     Stride;
  const LCD_API_COLOR_CONV * pColorConv;
} GUI_SVG_RENDER_TARGET;

typedef struct {
  GUI_HMEM hId;
  U32      StopColor;
  float    StopOpacity;
  float    Offset;
} GUI_SVG_GRADIENT_STOP;

typedef struct {
  GUI_SVG_GRADIENT_TYPE   Type;
  GUI_HMEM                hLink;
  GUI_HMEM                hId;
  GUI_SVG_GRADIENT_UNITS  Unit;
  GUI_SVG_GRADIENT_SPREAD Spread;
  GUI_MATRIX              Mat;
  GUI_ARRAY               haStops; // Array of GUI_SVG_GRADIENT_STOP
  U8                      Flags;   // Combination of GUI_SVG_GRADIENT_FLAGS
} GUI_SVG_GRADIENT;

typedef struct {
  GUI_SVG_GRADIENT Base;
  float            x1, y1, x2, y2;
} GUI_SVG_GRADIENT_LINEAR;

typedef struct {
  GUI_SVG_GRADIENT Base;
  float cx, cy; // Center point
  float r;      // Radius
  float fx, fy; // Focal point
} GUI_SVG_GRADIENT_RADIAL;

typedef struct {
  GUI_COLOR          ColorFill;
  GUI_COLOR          ColorStroke;
  float              aDashArray[GUI_SVG_MAX_NUM_DASH_ITEMS];
  int                NumDash;
  float              StrokeWidth;
  float              MiterLimit;
  GUI_SVG_CAP_STYLE  LineCap;
  GUI_SVG_JOIN_STYLE LineJoin;
  GUI_SVG_FILL_RULE  FillRule;
  GUI_SVG_GRADIENT * pGradientFill;
  GUI_SVG_GRADIENT * pGradientStroke;
  float              FillOpacity;
  float              StrokeOpacity;
  float              Opacity;
  float              StrokeDashoffset;
} GUI_SVG_STYLE;
//
// Context for handling paths.
//
typedef struct {
  GUI_POINTF InitialPos;           // Initial position of a sub-path.
  GUI_POINTF PenPos;
  GUI_POINTF LastQuadEndPos;
  GUI_POINTF LastQuadControlPos;
  GUI_POINTF LastCubicEndPos;
  GUI_POINTF LastCubicControlPos;
  U32        Flags;                // Flags to determine if above coords are valid. If set it is invalid. LSB is first struct member, and so on...
} GUI_SVG_PATH_CONTEXT;

//
// Internal flags
//
#define GUI_SVG_FLAG_CACHEMODE    (1 << 0)    // Caching enabled.
#define GUI_SVG_FLAG_INVALID      (1 << 1)    // Cache invalid.

//
// Aliases for common matrices and parameters
//
#define MAT_SVG(pSVG)    (&pSVG->BaseData.Mat)  // Current result of all loaded matrices and affine transformations.
#define MAT_TR(pSVG)     (&pSVG->Transform.aMatrix[GUI_SVG_MATRIX_TRANSLATE])
#define MAT_SC(pSVG)     (&pSVG->Transform.aMatrix[GUI_SVG_MATRIX_SCALE])
#define MAT_RO(pSVG)     (&pSVG->Transform.aMatrix[GUI_SVG_MATRIX_ROTATE])
#define VEC_RO(pSVG)     (&pSVG->Transform.aMatrix[GUI_SVG_VECTOR_ROTATE])
#define MAT_CUST(pSVG)   ((GUI_MATRIX *)GUI_ALLOC_h2p(pSVG->Transform.hMatrix)) // Custom matrix set by user.

typedef enum {
  GUI_SVG_MATRIX_TRANSLATE,   // Translation set by GUI_SVG_Translate().
  GUI_SVG_MATRIX_SCALE,       // Scale set by GUI_SVG_Scale().
  GUI_SVG_MATRIX_ROTATE,      // Rotation set by GUI_SVG_Rotate().
  GUI_SVG_VECTOR_ROTATE,
  //
  GUI_SVG_MATRIX_COUNT,       // LEAVE UNCHANGED!
} GUI_SVG_MATRIX_ID;

typedef struct {
  GUI_MATRIX aMatrix[GUI_SVG_MATRIX_COUNT];
  GUI_HMEM   hMatrix;     // Custom transformation matrix supplied with GUI_SVG_Transform()
} GUI_SVG_TRANSFORM_CONTEXT;

typedef struct {
  GUI_SVG_GRADIENT * pCurrentGrad;
  U8                 AcceptCss; // Flag to accept the next CDATA read as CSS data.
} GUI_SVG_PARSER_CONTEXT;

//
// The data that is saved for every indendation level.
// For each indentation level, a new instance of below struct is added.
// If the level decreases the previous one is restored.
// 
// This behavior was previously achieved with recursion and stack memory, but the new parsing
// strategy requires us to do this with a list of heap allocated items,
// because there is no more recursion.
//
typedef struct {
  GUI_SVG_STYLE Style;
  GUI_MATRIX    Mat;
} GUI_SVG_INDENT_DATA;

//
// Any raster image within <image> tag
//
typedef struct {
  U32               Off;    // Offset in file, this serves as the identifier because an id attribute is not required.
  GUI_MEMDEV_Handle hImage; // Prerendered image data.
} GUI_SVG_IMAGE;

typedef struct {
  GUI_XML_Handle            hXML;        // SVG file loaded into XML module.
  GUI_SVG_INDENT_DATA       BaseData;    // Matrix and style for top most level <svg>.
  GUI_ARRAY                 haIndent;    // GUI_SVG_INDENT_DATA for each indent level. Index 0 is topmost draw tag. <svg> level not included here.
  GUI_SVG_INFO              Info;        // Public info struct, data is used internally by object, when user retrieves info this will be copied.
  GUI_ARRAY                 haCssRules;  // Array of GUI_CSS_RULEs
  GUI_ARRAY                 haGradients; // Array of GUI_SVG_GRADIENTs, optional.
  GUI_ARRAY                 haImages;    // Array of GUI_SVG_IMAGE, optional.
  GUI_SVG_TRANSFORM_CONTEXT Transform;   // Parameters used for transformations.
  GUI_SVG_PATH_CONTEXT      PathCtx;     // Path context, contains positions like pen position, required for normalizing paths.
  GUI_SVG_PARSER_CONTEXT    ParserCtx;   // Context for parser module.
  void                    * pContext;    // Draw context managed by SVG driver.
  GUI_MEMDEV_Handle         hMemDev;     // Memdev used for caching the SVG.
  GUI_COLOR                 BkColor;     // Background color used for filling internal memory device.
  U8                        Flags;       // See GUI_SVG_FLAG...
} GUI_SVG_Obj;

typedef PTR_ADDR GUI_SVG_PATH_Handle;

//
// Static context for each SVG driver
//
typedef struct {
  GUI_SVG_HOOKS Hooks;        // Additional optional driver/draw init hooks.
  const void  * pAPI;         // User supplied pointer to 2D vector API.
  GUI_HMEM      hContext;     // Additional data req. by the driver, allocated on demand.
  U8            DriverInited; // Flag set when driver has been inited.
} GUI_SVG_DRIVER_CONTEXT;

typedef struct {
  GUI_SVG_PATH_Handle      (* pfCreatePath)     (void);
  void                     (* pfAppendPath)     (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath, int NumSegments, const U8 * pSegments, int NumData, const float * pCoordinates);
  void                     (* pfFinalizePath)   (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath);
  void                     (* pfDestroyPath)    (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath);
  void                     (* pfDrawPath)       (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath, GUI_SVG_STYLE * pStyle);
  void                     (* pfDrawImage)      (GUI_SVG_Obj * pObj, const GUI_SVG_RENDER_TARGET * pRT);
  void                     (* pfEnableClipping) (int x, int y, int w, int h);
  void                     (* pfDisableClipping)(void);
  U8                       (* pfInitDriver)     (void);
  void                     (* pfDeinitDriver)   (void);
  U8                       (* pfBeginDraw)      (GUI_SVG_Obj * pObj);
  void                     (* pfEndDraw)        (GUI_SVG_Obj * pObj);
  GUI_SVG_DRIVER            * pfGetDriver;
  GUI_SVG_DRIVER_CONTEXT * (* pfGetContext)     (void);
  void                     (* pfLoadAPI)        (void * pAPI, GUI_SVG_LOAD_API_CALLBACK * cbLoadFunction);
  int                      (* pfHasAPI)         (void);
  U8                       (* pfSetHooks)       (void);
} GUI_SVG_DRIVER_API;

typedef int (DRAW_IMAGE_FILE_FUNC)(const void * pFileData, int FileSize, int x0, int y0);
typedef int (DRAW_GIF_FILE_FUNC)  (const void * pFileData, U32 FileSize, int x0, int y0);
typedef int (DRAW_BMP_FILE_FUNC)  (const void * pFileData,               int x0, int y0);

//
// Global context
//
typedef struct {
  const GUI_SVG_DRIVER_API * pDriver;            // Currently selected driver.
  DRAW_IMAGE_FILE_FUNC     * pfDrawPNG;          // Optional pointer to GUI_PNG_Draw()
  DRAW_IMAGE_FILE_FUNC     * pfDrawJPEG;         // Optional pointer to GUI_JPEG_Draw()
  DRAW_GIF_FILE_FUNC       * pfDrawGIF;          // Optional pointer to GUI_GIF_Draw()
  DRAW_BMP_FILE_FUNC       * pfDrawBMP;          // Optional pointer to GUI_BMP_Draw()
  unsigned                   NumBytesBuffer;     // Size of the buffer that will be used for buffering externally read files.
  GUI_HMEM                   hBase64DecodeTable; // Decoding table for Base64.
  int                        NumDotsPerInch;     // Configured DPI for display, used for real life units like cm.
} GUI_SVG_CONTEXT;

//
// Shorter aliases
//
#define SVG_CONTEXT  (&GUI_SVG__Context)
#define SVG_DRIVER   (SVG_CONTEXT->pDriver)
#define SVG_USER_API (_Context.pAPI)         // In static context

//
// Parameters used by draw functions
//
typedef struct {
  float x, y, w, h, rx, ry;
} GUI_SVG_RECT_PARAMETERS;

typedef struct {
  float x1, y1, x2, y2;
} GUI_SVG_LINE_PARAMETERS;

typedef struct {
  float cx, cy, r;
} GUI_SVG_CIRCLE_PARAMETERS;

typedef struct {
  float cx, cy, rx, ry;
} GUI_SVG_ELLIPSE_PARAMETERS;

/*********************************************************************
*
*       Private code
*
**********************************************************************
*/
GUI_SVG_GRADIENT * GUI_SVG__GetGradientLocked(GUI_SVG_Obj * pObj, const char * sId);
void               GUI_SVG__GetRenderTarget  (GUI_SVG_RENDER_TARGET * pRT);
int                GUI_SVG__GetNumCoords     (GUI_SVG_PATH_COMMAND Segment);
void               GUI_SVG__Parse            (GUI_SVG_Obj * pObj, GUI_SVG_PARSING_STRATEGY Strategy);
//
// Basic draw routines.
//
GUI_SVG_PATH_Handle   GUI_SVG__CreatePath       (void);
void                  GUI_SVG__AppendPath       (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath, U8 Segment, float * pData);
void                  GUI_SVG__FinalizePath     (GUI_SVG_Obj * pObj, GUI_SVG_PATH_Handle hPath);
void                  GUI_SVG__DrawPath         (GUI_SVG_Obj * pObj, GUI_SVG_STYLE * pStyle, const GUI_SVG_PATH_Handle        * phPath);
void                  GUI_SVG__DrawRect         (GUI_SVG_Obj * pObj, GUI_SVG_STYLE * pStyle, const GUI_SVG_RECT_PARAMETERS    * p);
void                  GUI_SVG__DrawCircle       (GUI_SVG_Obj * pObj, GUI_SVG_STYLE * pStyle, const GUI_SVG_CIRCLE_PARAMETERS  * p);
void                  GUI_SVG__DrawEllipse      (GUI_SVG_Obj * pObj, GUI_SVG_STYLE * pStyle, const GUI_SVG_ELLIPSE_PARAMETERS * p);
void                  GUI_SVG__DrawLine         (GUI_SVG_Obj * pObj, GUI_SVG_STYLE * pStyle, const GUI_SVG_LINE_PARAMETERS    * p);
void                  GUI_SVG__DrawImage        (GUI_SVG_Obj * pObj, U32 TagOffset, float x, float y, float w, float h);
//
// Management of indentation level data.
//
GUI_SVG_INDENT_DATA * GUI_SVG__IncIndent       (GUI_SVG_Obj * pObj);
void                  GUI_SVG__DecIndent       (GUI_SVG_Obj * pObj);
GUI_SVG_INDENT_DATA * GUI_SVG__GetCurrentIndent(GUI_SVG_Obj * pObj);
void                  GUI_SVG__UnlockIndent    (GUI_SVG_Obj * pObj, GUI_SVG_INDENT_DATA ** ppData);
//
// Constructors for objects
//
DECLARE_CONSTRUCTOR(GUI_SVG_STYLE);
DECLARE_CONSTRUCTOR(GUI_SVG_GRADIENT);
DECLARE_CONSTRUCTOR(GUI_SVG_GRADIENT_STOP);
DECLARE_CONSTRUCTOR(GUI_SVG_Obj);
//
// Private string functions
//
char * GUI__ReadStringUntil (const char * sDelim, const char ** pp);
int    GUI__EqualsIgnoreCase(const char * s0, const char * s1);
int    GUI__BeginsWith      (const char * s0, const char * s1);
//
// Aliases for string functions
//
#define STR_EQUALS(s0, s1)        GUI__EqualsIgnoreCase(s0, s1)
#define STR_BEGINS_WITH(s0, s1)   GUI__BeginsWith(s0, s1)

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
//
// Global SVG context
//
extern GUI_SVG_CONTEXT GUI_SVG__Context;
//
// SVG drivers for each 2D vector API
//
extern const GUI_SVG_DRIVER_API GUI_SVG__OpenVG_API;
extern const GUI_SVG_DRIVER_API GUI_SVG__VGLite_API;
extern const GUI_SVG_DRIVER_API GUI_SVG__NemaVG_API;
//
#define GUI_SVG_DRIVER_API_OPENVG  &GUI_SVG__OpenVG_API
#define GUI_SVG_DRIVER_API_VGLITE  &GUI_SVG__VGLite_API
#define GUI_SVG_DRIVER_API_NEMAVG  &GUI_SVG__NemaVG_API

#if defined(__cplusplus)
}
#endif

#endif /* GUI_SVG_PRIVATE_H */

/*************************** End of file ****************************/
