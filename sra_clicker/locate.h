#ifndef LOCATE_H
#define LOCATE_H

#include "sra_clicker.h"

/**************************/
/*** DEFINES - SETTINGS ***/
/**************************/

/******************/
/*** ERROR ENUM ***/
/******************/

typedef enum
{
    #define X(x) SRA_LOCATE_ERR_##x
    X(NONE),                    // function successfully executed
    X(MISSING_SELF),            // the self (locate) struct was missing
    X(MISSING_PIXELS),          // the pixel data in self is missing (malloc failed)
    X(MISSING_HANDLE),          // there is no handle
    X(EOFUNCTION),              // end of function reached; not necessairily an error
    X(OPEN_FILE),               // failed to open a file
    X(MALLOC_DATA),             // failed to malloc data
    X(MALLOC_PIXELS),           // failed to malloc pixels
    X(MALLOC_NEXTPX),           // failed to malloc nextpx
    X(GETWINDOWDC),             // failed to GetWindowDC
    X(CREATEDC),                // failed to CreateDC
    X(CREATECOMPATIBLEDC),      // failed to CreateCompatibleDC
    X(CREATECOMPATIBLEBITMAP),  // failed to CreateCompatibleBitmap
    X(SELECTOBJECT),            // failed to SelectObject
    X(BITBLT),                  // failed to BitBlt
    X(GETDIBITS),               // failed to GetDIBits
    X(FINDWINDOW),              // failed to FindWindow
    X(GETWINDOWRECT),           // failed to GetWindowRect
    #undef X
}
sra_locate_error_t;

/************************/
/*** LOCATE STRUCTURE ***/
/************************/

typedef struct sra_locate_s
{
    // readability
    #define sra_locate_t struct sra_locate_s
    
    // variables
    void *data;     // private data
    int x;          // x position ("return" value of some function)
    int y;          // y position,  "
    uint8_t a;      // alpha value, "
    uint8_t r;      // red value,   "
    uint8_t g;      // green value, "
    uint8_t b;      // blue value,  "
    uint32_t argb;  // ARGB value,  "
    
    // functions
    sra_locate_error_t (*const refresh)(sra_locate_t *self);
    sra_locate_error_t (*const locate_color_rgb)(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);  // TODO respect valid_window, only take screenshot of valid_window
    sra_locate_error_t (*const get_color_rgb_xy)(sra_locate_t *self, int x, int y);
    sra_locate_error_t (*const get_color_xy)(sra_locate_t *self, int x, int y);
    sra_locate_error_t (*const save_bmp)(sra_locate_t *self, wchar_t *Filename);
    sra_locate_error_t (*const restrict_window)(sra_locate_t *self, wchar_t *Window);
    sra_locate_error_t (*const locate_color)(sra_locate_t *self, uint32_t Color);
    sra_locate_error_t (*const add_ignore_rect)(sra_locate_t *self, int left, int top, int right, int bottom);
    
    //void (*restrict_window)(sra_locate_t *self, char *Window);
    //void (*focus_window)(sra_locate_t *self, char *Window);
    
    // TODO add these
    //(*set_valid_window, self, force to bring it to the foreground)
    
    // undo readability
    #undef sra_locate_t
}
sra_locate_t;

/**********************************/
/*** PUBLIC FUNCTION PROTOTYPES ***/
/**********************************/

sra_locate_t sra_locate_new(void);
sra_locate_error_t sra_locate_setup(sra_locate_t *self);
sra_locate_error_t sra_locate_free(sra_locate_t *self);

#endif
