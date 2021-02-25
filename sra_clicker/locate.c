
#include "sra_clicker.h"

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
    
    // when going through the bitmap, this array tells us where the next pixel we
    // have to look at lays. (added when the ignore_rect functions got added)
    int *NextPX;
    
    // image stuff
    int Width;  // width, duh
    int Height; // height
    int OffsX;  // offset x on screen
    int OffsY;  // offset y on screen
    
    // bitmap info & file header
    BITMAPINFOHEADER BitmapInfoHeader;
    BITMAPFILEHEADER BitmapFileHeader;
    
    // restricted window; NULL = everything
    char *Restrict; 
}
sra_locate_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static bool sra_locate_refresh(sra_locate_t *self);
static bool sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);
static void sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y);
static uint32_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y);
static bool sra_locate_save_bmp(sra_locate_t *self, char *Filename);
static bool sra_locate_restrict_window(sra_locate_t *self, char *Window);
static bool sra_locate_locate_color(sra_locate_t *self, uint32_t Color);
static int sra_locate_add_ignore_rect(sra_locate_t *self, int left, int top, int right, int bottom);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static inline uint8_t PosB(sra_locate_t *self, int x, int y);
static inline uint8_t PosR(sra_locate_t *self, int x, int y);
static inline uint8_t PosG(sra_locate_t *self, int x, int y);
static inline uint8_t PosA(sra_locate_t *self, int x, int y);

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
    self->data = malloc(sizeof(sra_locate_data_t));
    if(!self->data) return; // error precaution TODO add logging
    
    // initialize publics with 0
    self->x = 0;
    self->y = 0;
    self->r = 0;
    self->g = 0;
    self->b = 0;
    
    // initialize data with 0 / whatever its supposed to be
    _data->Pixels = NULL;
    _data->Restrict = NULL;
    _data->Width = 0;
    _data->Height = 0;
    _data->OffsX = 0;
    _data->OffsY = 0;
    
    _data->BitmapInfoHeader.biSize = 0;
    _data->BitmapInfoHeader.biWidth = 0;
    _data->BitmapInfoHeader.biHeight = 0;
    _data->BitmapInfoHeader.biPlanes = 1;
    _data->BitmapInfoHeader.biBitCount = 0;
    _data->BitmapInfoHeader.biCompression = 0;
    _data->BitmapInfoHeader.biSizeImage = 0;
    _data->BitmapInfoHeader.biXPelsPerMeter = 0;
    _data->BitmapInfoHeader.biYPelsPerMeter = 0;
    _data->BitmapInfoHeader.biClrUsed = 0;
    _data->BitmapInfoHeader.biClrImportant = 0;
    
    _data->BitmapFileHeader.bfType = 0x4D42;        // TODO magic number; this equals BM
    _data->BitmapFileHeader.bfSize = 0;
    _data->BitmapFileHeader.bfReserved1 = 0;
    _data->BitmapFileHeader.bfReserved2 = 0;
    _data->BitmapFileHeader.bfOffBits = 40 + 14;    // TODO magic numbers
    
    // assign functions
    self->refresh = sra_locate_refresh;
    self->locate_color_rgb = sra_locate_locate_color_rgb;
    self->get_color_rgb_xy = sra_locate_get_color_rgb_xy;
    self->get_color_xy = sra_locate_get_color_xy;
    self->save_bmp = sra_locate_save_bmp;
    self->restrict_window = sra_locate_restrict_window;
    self->locate_color = sra_locate_locate_color;
    self->add_ignore_rect = sra_locate_add_ignore_rect;
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
        // is there restriction to be free'd?
        if(_data->Restrict)
        {
            free(_data->Restrict);
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
 *  return  false   failed to refresh
 *          true    successful refresh
 */
static bool sra_locate_refresh(sra_locate_t *self)
{
    if(!self) return false;   // error precaution
    // https://stackoverflow.com/questions/66251475/screenshot-of-specific-window-c-c#66251475
    // https://stackoverflow.com/questions/3291167/how-can-i-take-a-screenshot-in-a-windows-application
    // https://stackoverflow.com/questions/16112482/c-getting-rgb-from-hbitmap
    // https://stackoverflow.com/questions/43925820/screen-capture-of-specific-window-c
    
    HDC hScreen = NULL;
    
    if(_data->Restrict) // a window is specified?
    {
        // take screenshot of that window
        HWND hWnd = FindWindowA(NULL, _data->Restrict);
        if(!hWnd) return false;
        hScreen = GetWindowDC(hWnd);
        if(!hScreen) return false;
        RECT rWnd;
        if(!GetWindowRect(hWnd, &rWnd)) return false;
        _data->OffsX = rWnd.left;
        _data->OffsY = rWnd.top;
        
        // TODO consider removing these since they are stored in bitmap header
        _data->Width = rWnd.right - rWnd.left;
        _data->Height = rWnd.bottom - rWnd.top; 
        
        printf("%d x %d off %d/%d\n", _data->Width, _data->Height, _data->OffsX, _data->OffsY);
    }
    else
    {
        // take screenshot of desktop
        hScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
        
        _data->Width = GetDeviceCaps(hScreen, HORZRES); 
        _data->Height = GetDeviceCaps(hScreen, VERTRES);
    }
    
    HDC hdcMem = CreateCompatibleDC(hScreen);
    if(!hdcMem) return false;
    
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, _data->Width, _data->Height);
    if(!hBitmap) return false;
    
    HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
    if(!hOld) return false;
    
    BitBlt(hdcMem, 0, 0, _data->Width, _data->Height, hScreen, 0, 0, SRCCOPY);
    if(!SelectObject(hdcMem, hOld)) return false;
    
    _data->BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    _data->BitmapInfoHeader.biPlanes = 1;
    _data->BitmapInfoHeader.biBitCount = 32;
    _data->BitmapInfoHeader.biWidth = _data->Width;
    _data->BitmapInfoHeader.biHeight = -_data->Height;
    _data->BitmapInfoHeader.biCompression = BI_RGB;
    _data->BitmapInfoHeader.biSizeImage = 4 * _data->Width * _data->Height;//0;// 3 * _data->Width * _data->Height;
    
    if(_data->Pixels)
    {
        free(_data->Pixels);
    }
    _data->Pixels = malloc(sizeof *_data->Pixels * 4 * _data->Width * _data->Height);
    if(!_data->Pixels) return false;
    
    if(_data->NextPX)
    {
        free(_data->NextPX);
    }
    _data->NextPX = malloc(sizeof *_data->Pixels * _data->Width * _data->Height);
    if(!_data->NextPX) return false;
    
    if(!GetDIBits(hdcMem, hBitmap, 0, _data->Height, _data->Pixels, (BITMAPINFO*)&_data->BitmapInfoHeader, DIB_RGB_COLORS)) return false;
    
    if(_data->Restrict) // is a window specified?
    {
        ReleaseDC(FindWindowA(NULL, _data->Restrict), hScreen);
    }
    else
    {
        ReleaseDC(GetDesktopWindow(), hScreen);
    }
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    
    return true;
}

/*  func    sra_locate_locate_color
 *  desc    locate color from within the image buffer. 
 *  return  true if color was found; coordinates stored in self->x and self->y
 *          false if color was not found
 */
static bool sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b)
{
    // error precaution
    if(!self) return false;             // is there a struct?
    if(!_data->Pixels) return false;    // is there an image?
    
    // locate pixel
    for(int y = 0; y < _data->Height; y++)
    {
        for(int x = 0; x < _data->Width; x++)
        {
            // check for color
            if(PosR(self, x, y) != r) continue;
            if(PosG(self, x, y) != g) continue;
            if(PosB(self, x, y) != b) continue;
            
            // color was found
            self->x = x + _data->OffsX;
            self->y = y + _data->OffsY;
            return true;
        }
    }
    return false;
}

/*  func    sra_locate_get_color_rgb_xy
 *  desc    get color at x and y coordinate (pixels); normally run after refresh
 *  return  colors in self->r, self->g, self->b
 */
static void sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y)
{
    // error precaution
    if(!self) return;               // is there a struct?
    if(!_data->Pixels) return;      // is there an image?
    // adjust x and y
    x -= _data->OffsX;
    y -= _data->OffsY;
    // get pixel values
    self->r = PosR(self, x, y);
    self->g = PosG(self, x, y);
    self->b = PosB(self, x, y);
}

/*  func    sra_locate_get_color_xy
 *  desc    return a 32 bit color from the coordinate x and y (pixels); normally run after refresh
 */
static uint32_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y)
{
    if(!self) return 0; // error precaution
    // adjust x and y
    x -= _data->OffsX;
    y -= _data->OffsY;
    return ((uint32_t*)_data->Pixels)[(y * _data->Width) + x];
}

/*  func    sra_locate_save_bmp
 *  desc    saves the pixels from refresh into a file
 *  return  false   save failed
 *          true    save successful
 */
static bool sra_locate_save_bmp(sra_locate_t *self, char *Filename)
{
    // error precaution
    if(!self) return false;             // is there a struct?
    if(!_data->Pixels) return false;    // is there an image?
    // save the bitmap
    // output the file
    FILE * file = fopen(Filename, "wb");
    if(!file) return false;
    // file header
    _data->BitmapFileHeader.bfSize = 40 + 14 + _data->BitmapInfoHeader.biSizeImage;
    fwrite(&_data->BitmapFileHeader.bfType, 2, 1, file);
    fwrite(&_data->BitmapFileHeader.bfSize, 4, 1, file);
    fwrite(&_data->BitmapFileHeader.bfReserved1, 2, 1, file);
    fwrite(&_data->BitmapFileHeader.bfReserved2, 2, 1, file);
    fwrite(&_data->BitmapFileHeader.bfOffBits, 4, 1, file);
    // info header
    fwrite(&_data->BitmapInfoHeader.biSize, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biWidth, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biHeight, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biPlanes, 2, 1, file);
    fwrite(&_data->BitmapInfoHeader.biBitCount, 2, 1, file);
    fwrite(&_data->BitmapInfoHeader.biCompression, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biSizeImage, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biXPelsPerMeter, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biYPelsPerMeter, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biClrUsed, 4, 1, file);
    fwrite(&_data->BitmapInfoHeader.biClrImportant, 4, 1, file);
    // pixels
    fwrite(_data->Pixels, _data->BitmapInfoHeader.biSizeImage, 1, file);
    fclose(file);
    
    return true;
}

/*  func    sra_locate_restrict_window
 *  param   Window  a string or NULL. NULL clears whatever restrict was set to
 *  desc    restricts locate operations on the specified window.
 *          unsupported:
 *              Calculator
 *              ...
 *  return  true    successfully set it (any unsupported ones will also return true)
 *          false   failed to set it
 */
static bool sra_locate_restrict_window(sra_locate_t *self, char *Window)
{
    // error precaution
    if(!self) return false;
    // set the restrict
    if(_data->Restrict)
    {
        free(_data->Restrict);
    }
    if(Window)
    {
        _data->Restrict = malloc(sizeof *_data->Restrict * (strlen(Window)) + 1);
        if(!_data->Restrict) return false;
        strcpy(_data->Restrict, Window);
    }
    else
    {
        _data->Restrict = 0;
        _data->OffsX = 0;
        _data->OffsY = 0;
    }
    return true;
}

/*  func    sra_locate_locate_color
 *  desc    locate a color off of a 32 bit color
 */
static bool sra_locate_locate_color(sra_locate_t *self, uint32_t Color)
{
    // error precaution
    if(!self) return false;             // is there a struct?
    if(!_data->Pixels) return false;    // is there an image?
    
    // locate pixel
    for(int y = 0; y < _data->Height; y++)
    {
        for(int x = 0; x < _data->Width; x++)
        {
            if(((uint32_t*)_data->Pixels)[(y * _data->Width) + x] == Color)
            {
                // color was found
                self->x = x + _data->OffsX;
                self->y = y + _data->OffsY;
                return true;
            }
        }
    }
    
    return false;
}

/*  func    sra_locate_ignore_window
 *  desc    add a rectangle/region where some functions (locate) should not search.
 *          left,top,right,bottoom in pixels, relative to the window. (0,0 is top left,
 *          width/height is bottom right)
 *          
 *  return  returns the "id" of the added rectangle
 */
static int sra_locate_add_ignore_rect(sra_locate_t *self, int left, int top, int right, int bottom)
{
    return 0;
}

/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

// TODO add descriptions and consider error precaution?
static inline uint8_t PosB(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 0];
}

static inline uint8_t PosG(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 1];
}

static inline uint8_t PosR(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 2];
}

static inline uint8_t PosA(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 3];
}
