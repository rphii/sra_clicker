
#include "sra_clicker.h"

/*******************/
/*** DEFINITIONS ***/
/*******************/

#define _data ((sra_mouse_data_t*)(self->data))
#define countof(__array__) (sizeof(__array__)/sizeof(*__array__))

/************************************/
/*** PRIVATE VARIABLE DEFINITIONS ***/
/************************************/

typedef struct
{
    INPUT * input;
    int count;
}
sra_mouse_routine_t;

typedef struct
{
    struct
    {
        int Width;
        int Height;
    }
    Screen;
    
    sra_mouse_routine_t Routines[SRA_MOUSE_ROUTINE_COUNT];    // mouse Routines like click, move, press, ...
}
sra_mouse_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static sra_mouse_error_t sra_mouse_initialize(sra_mouse_t *self);
static sra_mouse_error_t sra_mouse_clickl(sra_mouse_t *self);
static sra_mouse_error_t sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y);
static sra_mouse_error_t sra_mouse_move(sra_mouse_t *self, int x, int y);
static sra_mouse_error_t sra_mouse_pressl(sra_mouse_t *self);
static sra_mouse_error_t sra_mouse_releasel(sra_mouse_t *self);
static sra_mouse_error_t sra_mouse_update_dimensions(sra_mouse_t *self);
static sra_mouse_error_t sra_mouse_move_xy(sra_mouse_t *self, int x, int y);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static sra_mouse_error_t sra_mouse_initialize_routine(sra_mouse_routine_t *routine, int count);

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/

sra_mouse_t sra_mouse_new(void)
{
    return (sra_mouse_t)
    {
        // assign the functions
        .initialize = sra_mouse_initialize,
        .clickl = sra_mouse_clickl,
        .clickl_xy = sra_mouse_clickl_xy,
        .move = sra_mouse_move,
        .pressl = sra_mouse_pressl,
        .releasel = sra_mouse_releasel,
        .update_dimensions = sra_mouse_update_dimensions,
        .move_xy = sra_mouse_move_xy,
    };
}
 
/*  func    sra_mouse_setup
 *  desc    set up a sra_mouse_t struct. has to be called ONCE in order for 
 *          anything to work properly on the passed mouse structure
 */
sra_mouse_error_t sra_mouse_setup(sra_mouse_t *self)
{
    // allocate data memory
    self->data = malloc(sizeof(sra_mouse_data_t));
    if(!self->data)
    {
        return SRA_MOUSE_ERR_MALLOC_DATA;
    }
    
    // configure the clickl inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_CLICKL]), 2);
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL].input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the clickl_xy inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY]), 3);
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 0x4000; // 0x4000 is MOUSEEVENTF_VIRTUALDESK
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the move inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_MOVE]), 1);
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE].input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    
    // configure the pressl inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_PRESSL]), 1);
    _data->Routines[SRA_MOUSE_ROUTINE_PRESSL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    // configure the releasel inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_RELEASEL]), 1);
    _data->Routines[SRA_MOUSE_ROUTINE_RELEASEL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the move_xy inside data
    sra_mouse_initialize_routine(&(_data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY]), 1);
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 0x4000; // 0x4000 is MOUSEEVENTF_VIRTUALDESK
    
    // finish initialization
    self->update_dimensions(self);
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    sra_mouse_free
 *  desc    frees memory of the passed mouse struct
 */
sra_mouse_error_t sra_mouse_free(sra_mouse_t *self)
{
    // error prevention
    if(!self) return SRA_MOUSE_ERR_MISSING_SELF;

    // check for Routines
    for(int i = 0; i < SRA_MOUSE_ROUTINE_COUNT; i++)
    {
        if(_data->Routines[i].input)
        {
            free(_data->Routines[i].input);
        }
    }
    // free data
    free(self->data);
    
    return SRA_MOUSE_ERR_NONE;
}

/************************************/
/*** PRIVATE FUNCTION DEFINITIONS ***/
/************************************/

/*  func    sra_mouse_initialize
 *  desc    reinitialize a mouse structure
 */
static sra_mouse_error_t sra_mouse_initialize(sra_mouse_t *self)
{
    if(!self) return SRA_MOUSE_ERR_MISSING_SELF;
    sra_mouse_free(self);
    return sra_mouse_setup(self);
}

/*  func    sra_mouse_clickl
 *  desc    performs a left mouse click at the current mouse position
 */
static sra_mouse_error_t sra_mouse_clickl(sra_mouse_t *self)
{
    // error prevention
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_CLICKL].count, _data->Routines[SRA_MOUSE_ROUTINE_CLICKL].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    sra_mouse_clickl_xy
 *  desc    performs a left mouse click at the given coordinates in
 *          pixels; 0,0 is top left; Width,Height is bottom right
 */
static sra_mouse_error_t sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y)
{
    // error prevention
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // make additional routine instructions
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input[0].mi.dx = (65535 * x) / _data->Screen.Width;    // TODO magic numbers
    _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input[0].mi.dy = (65535 * y) / _data->Screen.Height;   // TODO magic numbers
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].count, _data->Routines[SRA_MOUSE_ROUTINE_CLICKL_XY].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    sra_mouse_move
 *  desc    moves the mouse by the specifiec x and y in pixels. Positive values move 
 *          the mouse right/down and negative values left/up
 */
static sra_mouse_error_t sra_mouse_move(sra_mouse_t *self, int x, int y)
{
    // error prevention
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // make additional routine instructions
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input[0].mi.dx = x;  // TODO magic numbers
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input[0].mi.dy = y;
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].count, _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    sra_mouse_pressl
 *  desc    press the left mouse button
 */
static sra_mouse_error_t sra_mouse_pressl(sra_mouse_t *self)
{
    // error prevention
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_PRESSL].count, _data->Routines[SRA_MOUSE_ROUTINE_PRESSL].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    sra_mouse_releasel
 *  desc    release the left mouse button
 */
static sra_mouse_error_t sra_mouse_releasel(sra_mouse_t *self)
{
    // error prevenetion
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_RELEASEL].count, _data->Routines[SRA_MOUSE_ROUTINE_RELEASEL].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}

/*  func    static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y);
 *  desc    move the mouse to the specified coordinates (pixels)
 *          0,0 is top left
 *          Width,Height is bottom right
 */
static sra_mouse_error_t sra_mouse_move_xy(sra_mouse_t *self, int x, int y)
{
    // error prevention
    if(!self->data) return SRA_MOUSE_ERR_MISSING_SELF;
    // make additional routine instructions
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input[0].mi.dx = (65535 * x) / _data->Screen.Width;    // TODO magic numbers
    _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input[0].mi.dy = (65535 * y) / _data->Screen.Height;   // TODO magic numbers
    // execute routine
    SendInput(_data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].count, _data->Routines[SRA_MOUSE_ROUTINE_MOVE_XY].input, sizeof(INPUT));
    
    return SRA_MOUSE_ERR_NONE;
}


static sra_mouse_error_t sra_mouse_update_dimensions(sra_mouse_t *self)
{
    if(!self) return SRA_MOUSE_ERR_MISSING_SELF;   // error precaution
    
    // https://stackoverflow.com/questions/8690619/how-to-get-Screen-resolution-in-c
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    if(!hDesktop) return SRA_MOUSE_ERR_GETDESKTOPWINDOW;
    // Get the size of Screen to the variable desktop
    if(!GetWindowRect(hDesktop, &desktop)) return SRA_MOUSE_ERR_GETWINDOWRECT;
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    _data->Screen.Width = desktop.right - desktop.left;
    _data->Screen.Height = desktop.bottom - desktop.top;
    
    return SRA_MOUSE_ERR_NONE;
}

/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

/*  func    sra_mouse_initialize_input_mouse
 *  desc    initialize an INPUT to be mouse with all other values initially set to 0
 */
static sra_mouse_error_t sra_mouse_initialize_routine(sra_mouse_routine_t *routine, int count)
{
    if(!routine) return SRA_MOUSE_ERR_MISSING_ROUTINE;    // error precaution
    
    routine->input = malloc(sizeof *routine->input * count);
    if(routine->input)
    {
        // successful allocation
        routine->count = count;
        
        // initialize rest with 0
        for(int i = 0; i < count; i++)
        {
            routine->input[i].type = INPUT_MOUSE;
            routine->input[i].mi.mouseData = 0;
            routine->input[i].mi.dwExtraInfo = 0;
            routine->input[i].mi.dwFlags = 0;
            routine->input[i].mi.dx = 0;
            routine->input[i].mi.dy = 0;
            routine->input[i].mi.mouseData = 0;
            routine->input[i].mi.time = 0;
        }
    }
    else
    {
        routine->count = 0;
        return SRA_MOUSE_ERR_MALLOC_ROUTINE;
    }
    
    return SRA_MOUSE_ERR_NONE;
}
