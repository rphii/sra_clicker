
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
    int OffsX;  // offset x on screen TODO do I even need those...?
    int OffsY;  // offset y on screen
    
    // bitmap info & file header
    BITMAPINFOHEADER BitmapInfoHeader;
    BITMAPFILEHEADER BitmapFileHeader;
    
    // window handle for restrict; NULL = desktop
    HWND Handle;
}
sra_locate_data_t;

/***********************************/
/*** PRIVATE FUNCTION PROTOTYPES ***/
/***********************************/

static sra_locate_error_t sra_locate_refresh(sra_locate_t *self);
static sra_locate_error_t sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b);
static sra_locate_error_t sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y);
static sra_locate_error_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y);
static sra_locate_error_t sra_locate_save_bmp(sra_locate_t *self, wchar_t *Filename);
static sra_locate_error_t sra_locate_restrict_window(sra_locate_t *self, wchar_t *Window);
static sra_locate_error_t sra_locate_locate_color(sra_locate_t *self, uint32_t Color);
static sra_locate_error_t sra_locate_add_ignore_rect(sra_locate_t *self, int left, int top, int right, int bottom);

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

/*  func    sra_locate_new
 *  desc    locate struct initializer
 */
sra_locate_t sra_locate_new(void)
{
    return (sra_locate_t)
    {
        .data = NULL,
        .x = 0,
        .y = 0,
        .a = 0,
        .r = 0,
        .g = 0,
        .b = 0,
        .argb = 0,
        // assign functions
        .refresh = sra_locate_refresh,
        .locate_color_rgb = sra_locate_locate_color_rgb,
        .get_color_rgb_xy = sra_locate_get_color_rgb_xy,
        .get_color_xy = sra_locate_get_color_xy,
        .save_bmp = sra_locate_save_bmp,
        .restrict_window = sra_locate_restrict_window,
        .locate_color = sra_locate_locate_color,
        //.add_ignore_rect = sra_locate_add_ignore_rect,
    };
}

/*  func    sra_locate_setup
 *  desc    set up locate
 */
sra_locate_error_t sra_locate_setup(sra_locate_t *self)
{
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;       // error precaution
    
    // allocate data memory
    self->data = malloc(sizeof(sra_locate_data_t));
    if(!self->data) return SRA_LOCATE_ERR_MALLOC_DATA;  // error precaution
    
    // initialize publics with 0
    self->x = 0;
    self->y = 0;
    self->r = 0;
    self->g = 0;
    self->b = 0;
    self->argb = 0;
    
    // initialize data with 0 / whatever its supposed to be
    _data->Pixels = NULL;
    _data->Handle = 0;
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
    
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_free
 *  desc    frees the memory of a locate
 */
sra_locate_error_t sra_locate_free(sra_locate_t *self)
{
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;   // error precaution
    
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
    
    return SRA_LOCATE_ERR_NONE;
}

/************************************/
/*** PRIVATE FUNCTION DEFINITIONS ***/
/************************************/

/*  func    sra_locate_refresh
 *  desc    refresh image buffer to locate in
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_refresh(sra_locate_t *self)
{
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;   // error precaution
    // https://stackoverflow.com/questions/66251475/screenshot-of-specific-window-c-c#66251475
    // https://stackoverflow.com/questions/3291167/how-can-i-take-a-screenshot-in-a-windows-application
    // https://stackoverflow.com/questions/16112482/c-getting-rgb-from-hbitmap
    // https://stackoverflow.com/questions/43925820/screen-capture-of-specific-window-c
    
    HDC hScreen = NULL;
    
    if(_data->Handle) // a window is specified?
    {
        // take screenshot of that window
        hScreen = GetWindowDC(_data->Handle);
        if(!hScreen) return SRA_LOCATE_ERR_GETWINDOWDC;
    }
    else
    {
        // take screenshot of desktop
        hScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
        if(!hScreen) return SRA_LOCATE_ERR_CREATEDC;
        
        _data->Width = GetDeviceCaps(hScreen, HORZRES); 
        _data->Height = GetDeviceCaps(hScreen, VERTRES);
    }
    
    HDC hdcMem = CreateCompatibleDC(hScreen);
    if(!hdcMem) return SRA_LOCATE_ERR_CREATECOMPATIBLEDC;
    
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, _data->Width, _data->Height);
    if(!hBitmap) return SRA_LOCATE_ERR_CREATECOMPATIBLEBITMAP;
    
    HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
    if(!hOld) return SRA_LOCATE_ERR_SELECTOBJECT;
    
    BitBlt(hdcMem, 0, 0, _data->Width, _data->Height, hScreen, 0, 0, SRCCOPY);
    if(!SelectObject(hdcMem, hOld)) return SRA_LOCATE_ERR_BITBLT;
    
    _data->BitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    _data->BitmapInfoHeader.biPlanes = 1;
    _data->BitmapInfoHeader.biBitCount = 32;
    _data->BitmapInfoHeader.biWidth = _data->Width;
    _data->BitmapInfoHeader.biHeight = -_data->Height;
    _data->BitmapInfoHeader.biCompression = BI_RGB;
    _data->BitmapInfoHeader.biSizeImage = 4 * _data->Width * _data->Height;//0;// 3 * _data->Width * _data->Height;
    
    if(!GetDIBits(hdcMem, hBitmap, 0, _data->Height, _data->Pixels, (BITMAPINFO*)&_data->BitmapInfoHeader, DIB_RGB_COLORS))
    {
        return SRA_LOCATE_ERR_GETDIBITS;
    }
    
    if(_data->Handle) // is a window specified?
    {
        ReleaseDC(_data->Handle, hScreen);
    }
    else
    {
        ReleaseDC(GetDesktopWindow(), hScreen);
    }
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_locate_color
 *  desc    locate color from within the image buffer. 
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b)
{
    // error precaution
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;   // is there a struct?
    if(!_data->Pixels) return SRA_LOCATE_ERR_MISSING_PIXELS;    // is there an image?
    
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
            return SRA_LOCATE_ERR_NONE;
        }
    }
    return SRA_LOCATE_ERR_EOFUNCTION;
}

/*  func    sra_locate_get_color_rgb_xy
 *  desc    get color at x and y coordinate (pixels); normally run after refresh
 *  return  error code (0 = success)
 *          colors in self->r, self->g, self->b
 */
static sra_locate_error_t sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y)
{
    // error precaution
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;   // is there a struct?
    if(!_data->Pixels) return SRA_LOCATE_ERR_MISSING_PIXELS; // is there an image?
    // adjust x and y
    x -= _data->OffsX;
    y -= _data->OffsY;
    // get pixel values
    self->r = PosR(self, x, y);
    self->g = PosG(self, x, y);
    self->b = PosB(self, x, y);
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_get_color_xy
 *  desc    return a 32 bit color from the coordinate x and y (pixels); normally run after refresh
 */
static sra_locate_error_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y)
{
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF; // error precaution
    // adjust x and y
    x -= _data->OffsX;
    y -= _data->OffsY;
    self->argb = ((uint32_t*)_data->Pixels)[(y * _data->Width) + x];
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_save_bmp
 *  desc    saves the pixels from refresh into a file
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_save_bmp(sra_locate_t *self, wchar_t *Filename)
{
    // error precaution
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;               // is there a struct?
    if(!_data->Pixels) return SRA_LOCATE_ERR_MISSING_PIXELS;    // is there image data?
    // save the bitmap
    // output the file
    FILE *file = _wfopen(Filename, L"wb");
    if(!file) return SRA_LOCATE_ERR_OPEN_FILE;
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
    
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_restrict_window
 *  param   Window  a string or NULL. NULL clears whatever restrict was set to
 *  desc    restricts locate operations on the specified window.
 *          unsupported:
 *              Calculator
 *              ...
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_restrict_window(sra_locate_t *self, wchar_t *Window)
{
    // error precaution
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;
    // set the restrict
    
    if(Window)  // a window is specified?
    {
        // find window
        //_data->Handle
        _data->Handle = FindWindow(NULL, Window);
        if(!_data->Handle) return SRA_LOCATE_ERR_FINDWINDOW;
        
        // get dimensions
        RECT rWnd;
        if(!GetWindowRect(_data->Handle, &rWnd)) 
        {
            _data->Handle = 0;  // rest handle
            return SRA_LOCATE_ERR_GETWINDOWRECT;
        }
        _data->OffsX = rWnd.left;
        _data->OffsY = rWnd.top;
        
        // TODO consider removing these since they are stored in bitmap header
        _data->Width = rWnd.right - rWnd.left;
        _data->Height = rWnd.bottom - rWnd.top; 
        
//        printf("%d x %d off %d/%d\n", _data->Width, _data->Height, _data->OffsX, _data->OffsY);
        
        // allocate memory for pixels
        if(_data->Pixels)
        {
            free(_data->Pixels);
        }
        _data->Pixels = malloc(sizeof *_data->Pixels * 4 * _data->Width * _data->Height);
        if(!_data->Pixels)
        {
            _data->Handle = 0;  // reset handle
            return SRA_LOCATE_ERR_MALLOC_PIXELS;
        }
        
        // allocate memory for next pixels
        if(_data->NextPX)
        {
            free(_data->NextPX);
        }
        _data->NextPX = malloc(sizeof *_data->Pixels * _data->Width * _data->Height);
        if(!_data->NextPX)
        {
            _data->Handle = 0;  // reset handle
            return SRA_LOCATE_ERR_MALLOC_NEXTPX;
        }
    }
    else    // target whole desktop
    {
        _data->Handle = 0;
        _data->OffsX = 0;
        _data->OffsY = 0;
    }
    return SRA_LOCATE_ERR_NONE;
}

/*  func    sra_locate_locate_color
 *  desc    locate a color off of a 32 bit color
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_locate_color(sra_locate_t *self, uint32_t Color)
{
    // error precaution
    if(!self) return SRA_LOCATE_ERR_MISSING_SELF;   // is there a struct?
    if(!_data->Pixels) return SRA_LOCATE_ERR_MISSING_PIXELS;    // is there an image?
    
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
                return SRA_LOCATE_ERR_NONE;
            }
        }
    }
    
    return SRA_LOCATE_ERR_EOFUNCTION;
}

/*  func    sra_locate_ignore_window
 *  desc    add a rectangle/region where some functions (locate) should not search.
 *          left,top,right,bottoom in pixels, relative to the window. (0,0 is top left,
 *          width/height is bottom right)
 *  return  error code (0 = success)
 */
static sra_locate_error_t sra_locate_add_ignore_rect(sra_locate_t *self, int left, int top, int right, int bottom)
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
