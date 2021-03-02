#ifndef MOUSE_H
#define MOUSE_H

#include "sra_clicker.h"

/****************************/
/*** ENUMERATION TYPEDEFS ***/
/****************************/

typedef enum
{
    #define X(x) SRA_MOUSE_ERR_##x
    X(NONE),                    // no error occured; successful
    X(MISSING_SELF),            // self is missing
    X(MISSING_ROUTINE),         // routine is missing
    X(MALLOC_DATA),             // failed to malloc data
    X(MALLOC_ROUTINE),          // failed to malloc routine
    X(GETDESKTOPWINDOW),        // failed to GetDesktopWindow
    X(GETWINDOWRECT),           // failed to GetWindowRect
    #undef X
}
sra_mouse_error_t;

typedef enum
{
    #define X(x) SRA_MOUSE_ROUTINE_##x
    X(CLICKL),
    X(CLICKL_XY),
    X(MOVE),
    X(PRESSL),
    X(RELEASEL),
    X(MOVE_XY),
    // below one has to stay the last at all costs
    X(COUNT)
    #undef X
}
sra_mouse_routines_t;

/***********************/
/*** MOUSE STRUCTURE ***/
/***********************/

typedef struct sra_mouse_s
{
    // readability
    #define sra_mouse_t struct sra_mouse_s
    
    // variables
    void *data;     // private data
    
    // functions
    sra_mouse_error_t (*const initialize)(sra_mouse_t *self);
    sra_mouse_error_t (*const clickl)(sra_mouse_t *self);
    sra_mouse_error_t (*const clickl_xy)(sra_mouse_t *self, int x, int y);
    sra_mouse_error_t (*const move)(sra_mouse_t *self, int x, int y);
    sra_mouse_error_t (*const pressl)(sra_mouse_t *self);
    sra_mouse_error_t (*const releasel)(sra_mouse_t *self);
    sra_mouse_error_t (*const update_dimensions)(sra_mouse_t* self);
    sra_mouse_error_t (*const move_xy)(sra_mouse_t *self, int x, int y);
    
    // TODO add these
    //(*scroll)
    
    // undo readability
    #undef sra_mouse_t
}
sra_mouse_t;

/**********************************/
/*** PUBLIC FUNCTION PROTOTYPES ***/
/**********************************/

sra_mouse_t sra_mouse_new(void);
sra_mouse_error_t sra_mouse_setup(sra_mouse_t *self);
sra_mouse_error_t sra_mouse_free(sra_mouse_t *self);

#endif
