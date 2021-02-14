
#include "main.h"
#include "mouse.h"

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
    // TODO I don't like this structure, nest it further?
    INPUT clickl[2];
    INPUT clickl_xy[3];
    INPUT move_xy[1];
    INPUT pressl[1];
    INPUT releasel[1];
}
sra_mouse_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static void sra_mouse_initialize(sra_mouse_t *self);
static void sra_mouse_clickl(sra_mouse_t *self);
static void sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y);
static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y);
static void sra_mouse_pressl(sra_mouse_t *self);
static void sra_mouse_releasel(sra_mouse_t *self);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static void sra_mouse_initialize_input_mouse(INPUT *input);

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
    
    // configure the clickl inside data
    sra_mouse_initialize_input_mouse(&_data->clickl[0]);
    sra_mouse_initialize_input_mouse(&_data->clickl[1]);
    _data->clickl[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->clickl[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the clickl_xy inside data
    sra_mouse_initialize_input_mouse(&_data->clickl_xy[0]);
    sra_mouse_initialize_input_mouse(&_data->clickl_xy[1]);
    sra_mouse_initialize_input_mouse(&_data->clickl_xy[2]);
    _data->clickl_xy[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | 0x4000; // 0x4000 is MOUSEEVENTF_VIRTUALDESK
    _data->clickl_xy[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    _data->clickl_xy[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // configure the move inside data
    sra_mouse_initialize_input_mouse(&_data->move_xy[0]);
    _data->move_xy[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    
    // configure the pressl inside data
    sra_mouse_initialize_input_mouse(&_data->pressl[0]);
    _data->pressl[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    // configure the releasel inside data
    sra_mouse_initialize_input_mouse(&_data->releasel[0]);
    _data->releasel[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    
    // assign functions
    self->initialize = sra_mouse_initialize;
    self->clickl = sra_mouse_clickl;
    self->clickl_xy = sra_mouse_clickl_xy;
    self->move_xy = sra_mouse_move_xy;
    self->pressl = sra_mouse_pressl;
    self->releasel = sra_mouse_releasel;
}

/*  func    sra_mouse_free
 *  desc    frees memory of the passed mouse struct
 */
void sra_mouse_free(sra_mouse_t *self)
{
    if(self->data)
    {
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
        SendInput(2, _data->clickl, sizeof(INPUT));
    }
}

/*  func    sra_mouse_clickl_xy
 *  desc    performs a left mouse click at the given coordinates.
 *          0,0 is top left
 *          65535,65535 is bottom right
 */
static void sra_mouse_clickl_xy(sra_mouse_t *self, int x, int y)
{
    if(self->data)
    {
        _data->clickl_xy[0].mi.dx = x;
        _data->clickl_xy[0].mi.dy = y;
        SendInput(3, _data->clickl_xy, sizeof(INPUT));  // TODO get rid of magic numbers
    }
}

/*  func    sra_mouse_move_xy
 *  desc    moves the mouse by the specifiec x and y in pixels. Positive values move 
 *          the mouse right/down and negative values left/up
 */
static void sra_mouse_move_xy(sra_mouse_t *self, int x, int y)
{
    if(self->data)
    {
        _data->move_xy[0].mi.dx = x;
        _data->move_xy[0].mi.dy = y;
        SendInput(1, _data->move_xy, sizeof(INPUT));
    }
}

/*  func    sra_mouse_pressl
 *  desc    press the left mouse button
 */
static void sra_mouse_pressl(sra_mouse_t *self)
{
    if(self->data)
    {
        SendInput(1, _data->pressl, sizeof(INPUT));
    }
}

/*  func    sra_mouse_releasel
 *  desc    release the left mouse button
 */
static void sra_mouse_releasel(sra_mouse_t *self)
{
    if(self->data)
    {
        SendInput(1, _data->releasel, sizeof(INPUT));
    }
}

/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

/*  func    sra_mouse_initialize_input_mouse
 *  desc    initialize an INPUT to be mouse with all other values initially set to 0
 */
static void sra_mouse_initialize_input_mouse(INPUT *input)
{
    input->type = INPUT_MOUSE;
    input->mi.mouseData = 0;
    input->mi.dwExtraInfo = 0;
    input->mi.dwFlags = 0;
    input->mi.dx = 0;
    input->mi.dy = 0;
    input->mi.mouseData = 0;
    input->mi.time = 0;
}