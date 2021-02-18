#ifndef LOCATE_H
#define LOCATE_H

#include "sra_clicker.h"

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
    uint8_t b;      // blue v alue, "
    
    // functions
    bool (*refresh)(sra_locate_t *self);
    bool (*locate_color_rgb)(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);  // TODO respect valid_window, only take screenshot of valid_window
    void (*get_color_rgb_xy)(sra_locate_t *self, int x, int y);
    uint32_t (*get_color_xy)(sra_locate_t *self, int x, int y);
    bool (*save_bmp)(sra_locate_t *self, char *Filename);
    bool (*restrict_window)(sra_locate_t *self, char *Window);
    bool (*locate_color)(sra_locate_t *self, uint32_t Color);
    
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

void sra_locate_setup(sra_locate_t *self);
void sra_locate_free(sra_locate_t *self);

#endif
