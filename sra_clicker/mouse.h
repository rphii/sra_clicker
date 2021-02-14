#ifndef MOUSE_H
#define MOUSE_H

#include "main.h"

/***********************/
/*** MOUSE STRUCTURE ***/
/***********************/

typedef struct sra_mouse_s
{
    // readability
    #define sra_mouse_t struct sra_mouse_s
    
    // variables
    void *data;
    
    // functions
    void (*initialize)(sra_mouse_t *self);
    void (*clickl)(sra_mouse_t *self);
    void (*clickl_xy)(sra_mouse_t *self, int x, int y);
    void (*move_xy)(sra_mouse_t *self, int x, int y);
    void (*pressl)(sra_mouse_t *self);
    void (*releasel)(sra_mouse_t *self);
    
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