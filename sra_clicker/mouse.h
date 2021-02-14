#ifndef MOUSE_H
#define MOUSE_H

#include "main.h"

/****************************/
/*** ENUMERATION TYPEDEFS ***/
/****************************/

typedef enum
{
    ROUTINE_CLICKL,
    ROUTINE_CLICKL_XY,
    ROUTINE_MOVE_XY,
    ROUTINE_PRESSL,
    ROUTINE_RELEASEL,
    // below one has to stay the last at all costs
    ROUTINE_COUNT
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
    void *data;         // private data
    
    // functions
    void (*initialize)(sra_mouse_t *self);
    void (*clickl)(sra_mouse_t *self);
    void (*clickl_xy)(sra_mouse_t *self, int x, int y);
    void (*move_xy)(sra_mouse_t *self, int x, int y);
    void (*pressl)(sra_mouse_t *self);
    void (*releasel)(sra_mouse_t *self);
    void (*refresh_wh)(sra_mouse_t *self);
    void (*update_dimensions)(sra_mouse_t* self);
    
    // undo readability
    #undef sra_mouse_t
}
sra_mouse_t;

/**********************************/
/*** PUBLIC FUNCTION PROTOTYPES ***/
/**********************************/

void sra_mouse_setup(sra_mouse_t *self);
void sra_mouse_free(sra_mouse_t *self);

#endif
