#ifndef LOCATE_H
#define LOCATE_H

#include "main.h"

/************************/
/*** LOCATE STRUCTURE ***/
/************************/

typedef struct sra_locate_s
{
    // readability
    #define sra_locate_t struct sra_locate_s
    
    // variables
    void *data;     // private data
    uint8_t x;      // x position ("return" value of some function)
    uint8_t y;      // y position ("return" value of some function)
    
    // functions
    void (*refresh)(sra_locate_t *self);
    bool (*locate_color_rgb)(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);
    
    // undo readability
    #undef sra_locate_t
}
sra_locate_t;

/**********************************/
/*** PUBLIC FUNCTION PROTOTYPES ***/
/**********************************/

void sra_locate_setup(sra_locate_t *self);
void sra_locate_free(sra_locate_t *self);

#endif
