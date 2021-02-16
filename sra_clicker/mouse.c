
#include "main.h"

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
    sra_mouse_routine_t routines[ROUTINE_COUNT];    // mouse routines like click, move, press, ...
    
    struct
    {
        int width;
        int height;
    }
    screen;
    
}
sra_mouse_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static void sra_mouse_initialize(sra_mouse_t *self);
static void sra_mouse_clickl(sra_mouse_t *self);
static void sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y);
static void sra_mouse_move(sra_mouse_t *self, int x, int y);
static void sra_mouse_pressl(sra_mouse_t *self);
static void sra_mouse_releasel(sra_mouse_t *self);
static void sra_mouse_update_dimensions(sra_mouse_t *self);
static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static void sra_mouse_initialize_routine(sra_mouse_routine_t *routine, int count);

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/
 
/*  func    sra_mouse_setup
 *  desc    set up a sra_mouse_t struct. has to be called ONCE in order for 
 *          anything to work properly on the passed mouse structure
 */
void sra_mouse_setup(sra_mouse_t *self)
{
    // allocate data memory
    self->data = (void*)malloc(sizeof(sra_mouse_data_t));
    if(!self->data)
    {
        return;
    }
    
    // configure the clickl inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_CLICKL]), 2);
    _data->routines[ROUTINE_CLICKL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->routines[ROUTINE_CLICKL].input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the clickl_xy inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_CLICKL_XY]), 3);
    _data->routines[ROUTINE_CLICKL_XY].input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 0x4000; // 0x4000 is MOUSEEVENTF_VIRTUALDESK
    _data->routines[ROUTINE_CLICKL_XY].input[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->routines[ROUTINE_CLICKL_XY].input[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the move_xy inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_MOVE]), 1);
    _data->routines[ROUTINE_MOVE].input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    
    // configure the pressl inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_PRESSL]), 1);
    _data->routines[ROUTINE_PRESSL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    // configure the releasel inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_RELEASEL]), 1);
    _data->routines[ROUTINE_RELEASEL].input[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the move_xy inside data
    sra_mouse_initialize_routine(&(_data->routines[ROUTINE_MOVE_XY]), 1);
    _data->routines[ROUTINE_MOVE_XY].input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 0x4000; // 0x4000 is MOUSEEVENTF_VIRTUALDESK
    
    // assign functions
    self->initialize = sra_mouse_initialize;
    self->clickl = sra_mouse_clickl;
    self->clickl_xy = sra_mouse_clickl_xy;
    self->move = sra_mouse_move;
    self->pressl = sra_mouse_pressl;
    self->releasel = sra_mouse_releasel;
    self->update_dimensions = sra_mouse_update_dimensions;
    self->move_xy = sra_mouse_move_xy;
    
    // finish initialization
    self->update_dimensions(self);
}

/*  func    sra_mouse_free
 *  desc    frees memory of the passed mouse struct
 */
void sra_mouse_free(sra_mouse_t *self)
{
    // check if there was data allocated in the first place
    if(self->data)
    {
        // check for routines
        for(int i = 0; i < ROUTINE_COUNT; i++)
        {
            if(_data->routines[i].input)
            {
                free(_data->routines[i].input);
            }
        }
        // free data
        free(self->data);
    }
}

/************************************/
/*** PRIVATE FUNCTION DEFINITIONS ***/
/************************************/

/*  func    sra_mouse_initialize
 *  desc    reinitialize a mouse structure
 */
static void sra_mouse_initialize(sra_mouse_t *self)
{
    sra_mouse_free(self);
    sra_mouse_setup(self);
}

/*  func    sra_mouse_clickl
 *  desc    performs a left mouse click at the current mouse position
 */
static void sra_mouse_clickl(sra_mouse_t *self)
{
    if(self->data)
    {
        SendInput(_data->routines[ROUTINE_CLICKL].count, _data->routines[ROUTINE_CLICKL].input, sizeof(INPUT));
    }
}

/*  func    sra_mouse_clickl_xy
 *  desc    performs a left mouse click at the given coordinates in
 *          pixels; 0,0 is top left; width,height is bottom right
 */
static void sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y)
{
    if(self->data)
    {
        _data->routines[ROUTINE_CLICKL_XY].input[0].mi.dx = (65535 * x) / _data->screen.width;    // TODO magic numbers
        _data->routines[ROUTINE_CLICKL_XY].input[0].mi.dy = (65535 * y) / _data->screen.height;   // TODO magic numbers
        
        //_data->routines[ROUTINE_CLICKL_XY].input[0].mi.dx = x;  // TODO magic numbers
        //_data->routines[ROUTINE_CLICKL_XY].input[0].mi.dy = y;
        SendInput(_data->routines[ROUTINE_CLICKL_XY].count, _data->routines[ROUTINE_CLICKL_XY].input, sizeof(INPUT));
    }
}

/*  func    sra_mouse_move
 *  desc    moves the mouse by the specifiec x and y in pixels. Positive values move 
 *          the mouse right/down and negative values left/up
 */
static void sra_mouse_move(sra_mouse_t *self, int x, int y)
{
    if(self->data)
    {
        _data->routines[ROUTINE_MOVE_XY].input[0].mi.dx = x;  // TODO magic numbers
        _data->routines[ROUTINE_MOVE_XY].input[0].mi.dy = y;
        SendInput(_data->routines[ROUTINE_MOVE_XY].count, _data->routines[ROUTINE_MOVE_XY].input, sizeof(INPUT));
    }
}

/*  func    sra_mouse_pressl
 *  desc    press the left mouse button
 */
static void sra_mouse_pressl(sra_mouse_t *self)
{
    if(self->data)
    {
        SendInput(_data->routines[ROUTINE_PRESSL].count, _data->routines[ROUTINE_PRESSL].input, sizeof(INPUT));
    }
}

/*  func    sra_mouse_releasel
 *  desc    release the left mouse button
 */
static void sra_mouse_releasel(sra_mouse_t *self)
{
    if(self->data)
    {
        SendInput(_data->routines[ROUTINE_RELEASEL].count, _data->routines[ROUTINE_RELEASEL].input, sizeof(INPUT));
    }
}

/*  func    static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y);
 *  desc    move the mouse to the specified coordinates (pixels)
 *          0,0 is top left
 *          width,height is bottom right
 */
static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y)
{
    if(self->data)
    {
        _data->routines[ROUTINE_MOVE_XY].input[0].mi.dx = (65535 * x) / _data->screen.width;    // TODO magic numbers
        _data->routines[ROUTINE_MOVE_XY].input[0].mi.dy = (65535 * y) / _data->screen.height;   // TODO magic numbers
        
        SendInput(_data->routines[ROUTINE_MOVE_XY].count, _data->routines[ROUTINE_MOVE_XY].input, sizeof(INPUT));
    }
}


/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

/*  func    sra_mouse_initialize_input_mouse
 *  desc    initialize an INPUT to be mouse with all other values initially set to 0
 */
static void sra_mouse_initialize_routine(sra_mouse_routine_t *routine, int count)
{
    if(!routine) return;    // error precaution
    
    routine->input = (INPUT*)malloc(sizeof(INPUT) * count);
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
    }
}

static void sra_mouse_update_dimensions(sra_mouse_t *self)
{
    if(!self) return;   // error precaution
    
    // https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    _data->screen.width = desktop.right - desktop.left;
    _data->screen.height = desktop.bottom - desktop.top;
}