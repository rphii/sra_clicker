
#include "main.h"

/***************/
/*** DEFINES ***/
/***************/

#define _data ((sra_locate_data_t*)self->data)

/************************************/
/*** PRIVATE VARIABLE DEFINITIONS ***/
/************************************/

typedef struct
{
    // pixel data of the image
    BYTE *Pixels;
    
    // width and height of the image
    int Width;
    int Height;
}
sra_locate_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static void sra_locate_refresh(sra_locate_t *self);
static bool sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static inline uint8_t PosB(sra_locate_t *self, uint8_t x, uint8_t y);
static inline uint8_t PosR(sra_locate_t *self, uint8_t x, uint8_t y);
static inline uint8_t PosG(sra_locate_t *self, uint8_t x, uint8_t y);

/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/

/*  func    sra_locate_setup
 *  desc    set up locate
 */
void sra_locate_setup(sra_locate_t *self)
{
    if(!self) return;   // error precaution
    
    // allocate data memory
    self->data = (void*)malloc(sizeof(sra_locate_data_t*));
    if(!self->data) return; // error precaution
    
    // initialize publics with 0
    self->x = 0;
    self->y = 0;
    
    // initialize data with 0
    _data->Pixels = NULL;
    _data->Width = 0;
    _data->Height = 0;
    
    // assign functions
    self->refresh = sra_locate_refresh;
    self->locate_color_rgb = sra_locate_locate_color_rgb;
}

/*  func    sra_locate_free
 *  desc    frees the memory of a locate
 */
void sra_locate_free(sra_locate_t *self)
{
    if(!self) return;   // error precaution
    
    // was there data allocated in the first place?
    if(self->data)
    {
        // is there a bitmap to be free'd?
        if(_data->Pixels)
        {
            free(_data->Pixels);
        }
        
        // free data
        free(self->data);
    }
}

/************************************/
/*** PRIVATE FUNCTION DEFINITIONS ***/
/************************************/

/*  func    sra_locate_refresh
 *  desc    refresh image buffer to locate in
 */
static void sra_locate_refresh(sra_locate_t *self)
{
    // https://stackoverflow.com/questions/3291167/how-can-i-take-a-screenshot-in-a-windows-application
    // https://stackoverflow.com/questions/16112482/c-getting-rgb-from-hbitmap
    
    // get the device context of the screen
    HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
    // and a device context to put it in
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    
    _data->Width = GetDeviceCaps(hScreenDC, HORZRES);
    _data->Height = GetDeviceCaps(hScreenDC, VERTRES);
    
    // maybe worth checking these are positive values
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, _data->Width, _data->Height);
    
    HGDIOBJ hOld = SelectObject(hMemoryDC, hBitmap);
    BitBlt(hMemoryDC, 0, 0, _data->Width, _data->Height, hScreenDC, 0, 0, SRCCOPY);
    SelectObject(hMemoryDC, hOld);
    
    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = _data->Width;
    bmi.biHeight = _data->Height;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;
    
    // free byte data
    if(_data->Pixels)
    {
        free(_data->Pixels);
    }
    
    // allocate memory
    _data->Pixels = (BYTE*)malloc(4 * _data->Width * _data->Height);
    
    // retrieves the bits of the specified compatible bitmap
    GetDIBits(hMemoryDC, hBitmap, 0, _data->Width, _data->Pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    
    // clean up
    ReleaseDC(GetDesktopWindow(),hScreenDC);
    DeleteDC(hMemoryDC);
    DeleteObject(hBitmap);
}

/*  func    sra_locate_locate_color
 *  desc    locate color from within the image buffer. 
 *  return  true if color was found; coordinates stored in self->x and self->y
 *          false if color was not found
 */
static bool sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b)
{
    for(int y = 0; y < _data->Height; y++)
    {
        for(int x = 0; x < _data->Width; x++)
        {
            // check for color
            if(PosR(self, x, y) != r) continue;
            if(PosG(self, x, y) != g) continue;
            if(PosB(self, x, y) != b) continue;
            
            // color was found
            self->x = x;
            self->y = y;
            return true;
        }
    }
    return false;
}

/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

static inline uint8_t PosB(sra_locate_t *self, uint8_t x, uint8_t y) 
{
    return _data->Pixels[4 * (( y * _data->Width ) + x) + 0];
}

static inline uint8_t PosG(sra_locate_t *self, uint8_t x, uint8_t y) 
{
    return _data->Pixels[4 * (( y * _data->Width ) + x) + 1];
}

static inline uint8_t PosR(sra_locate_t *self, uint8_t x, uint8_t y) 
{
    return _data->Pixels[4 * (( y * _data->Width ) + x) + 2];
}
