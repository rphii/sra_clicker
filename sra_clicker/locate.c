
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
static void sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y);
static uint32_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y);

/**********************************/
/*** HELPER FUNCTION PROTOTYPES ***/
/**********************************/

static uint8_t PosB(sra_locate_t *self, int x, int y);
static uint8_t PosR(sra_locate_t *self, int x, int y);
static uint8_t PosG(sra_locate_t *self, int x, int y);
static uint8_t PosA(sra_locate_t *self, int x, int y);

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
    self->r = 0;
    self->g = 0;
    self->b = 0;
    
    // initialize data with 0
    _data->Pixels = NULL;
    _data->Width = 0;
    _data->Height = 0;
    
    // assign functions
    self->refresh = sra_locate_refresh;
    self->locate_color_rgb = sra_locate_locate_color_rgb;
    self->get_color_rgb_xy = sra_locate_get_color_rgb_xy;
    self->get_color_xy = sra_locate_get_color_xy;
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
    if(!self) return;   // error precaution
    // https://stackoverflow.com/questions/3291167/how-can-i-take-a-screenshot-in-a-windows-application
    // https://stackoverflow.com/questions/16112482/c-getting-rgb-from-hbitmap
    HDC hScreen = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    _data->Width = GetDeviceCaps(hScreen, HORZRES);
    _data->Height = GetDeviceCaps(hScreen, VERTRES);
    
    HDC hdcMem = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, _data->Width, _data->Height);
    HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, _data->Width, _data->Height, hScreen, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hOld);
    
    BITMAPINFOHEADER bmi = {0};
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = _data->Width;
    bmi.biHeight = -_data->Height;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 4 * _data->Width * _data->Height;//0;// 3 * _data->Width * _data->Height;
    
    if(_data->Pixels)
    {
        free(_data->Pixels);
    }
    _data->Pixels = (BYTE*)malloc(4 * _data->Width * _data->Height);
    //if(ScreenData)
    //    free(ScreenData);
    //ScreenData = (BYTE*)malloc(4 * _data->Width * _data->Height);
    
    GetDIBits(hdcMem, hBitmap, 0, _data->Height, _data->Pixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
    
    ReleaseDC(GetDesktopWindow(),hScreen);
    DeleteDC(hdcMem);
    DeleteObject(hBitmap);
    
    // TODO remove the code below; checkd to see wheter or not images are correct or not => they are
    // save the bitmap
    /*
    BITMAPFILEHEADER bf = {0};
    bf.bfType = 0x4D42;
    bf.bfOffBits = 40 + 14;
    bf.bfSize = 40 + 16 + bmi.biSizeImage;
    // output the file
    FILE * file = fopen("a.bmp", "wb");
    if(file == NULL) return;
    // file header
    fwrite(&bf.bfType, 2, 1, file);
    fwrite(&bf.bfSize, 4, 1, file);
    fwrite(&bf.bfReserved1, 2, 1, file);
    fwrite(&bf.bfReserved2, 2, 1, file);
    fwrite(&bf.bfOffBits, 4, 1, file);
    // info header
    fwrite(&bmi.biSize, 4, 1, file);
    fwrite(&bmi.biWidth, 4, 1, file);
    fwrite(&bmi.biHeight, 4, 1, file);
    fwrite(&bmi.biPlanes, 2, 1, file);
    fwrite(&bmi.biBitCount, 2, 1, file);
    fwrite(&bmi.biCompression, 4, 1, file);
    fwrite(&bmi.biSizeImage, 4, 1, file);
    fwrite(&bmi.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmi.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmi.biClrUsed, 4, 1, file);
    fwrite(&bmi.biClrImportant, 4, 1, file);
    // pixels
    fwrite(_data->Pixels, bmi.biSizeImage, 1, file);
    fclose(file);*/
}

/*  func    sra_locate_locate_color
 *  desc    locate color from within the image buffer. 
 *  return  true if color was found; coordinates stored in self->x and self->y
 *          false if color was not found
 */
static bool sra_locate_locate_color_rgb(sra_locate_t *self, uint8_t r, uint8_t g, uint8_t b)
{
    if(!self) return false; // error precaution
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

/*  func    sra_locate_get_color_rgb_xy
 *  desc    get color at x and y coordinate (pixels); normally run after refresh
 *  return  colors in self->r, self->g, self->b
 */
static void sra_locate_get_color_rgb_xy(sra_locate_t *self, int x, int y)
{
    if(!self) return;           // error precaution
    self->r = PosR(self, x, y);
    self->g = PosG(self, x, y);
    self->b = PosB(self, x, y);
    self->a = PosA(self, x, y);
}

/*  func    sra_locate_get_color_xy
 *  desc    return a 32 bit color from the coordinate x and y (pixels); normally run after refresh
 */
static uint32_t sra_locate_get_color_xy(sra_locate_t *self, int x, int y)
{
    if(!self) return 0; // error precaution
    return ((uint32_t*)_data->Pixels)[(y * _data->Width) + x];
}

/***********************************/
/*** HELPER FUNCTION DEFINITIONS ***/
/***********************************/

// TODO add descriptions and consider error precaution?
static uint8_t PosB(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 0];
}

static uint8_t PosG(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 1];
}

static uint8_t PosR(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 2];
}

static uint8_t PosA(sra_locate_t *self, int x, int y) 
{
    return ((uint8_t*)_data->Pixels)[4 * ((y * _data->Width) + x) + 3];
}
