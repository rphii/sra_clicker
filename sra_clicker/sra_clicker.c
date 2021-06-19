
#include "sra_clicker.h"

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // thanks to
    //https://stackoverflow.com/questions/22975916/global-keyboard-hook-with-wh-keyboard-ll-and-keybd-event-windows
    
    BOOL fEatKeystroke = FALSE;
    PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
    
    switch (wParam)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            break;
        default:
            break;
        }
    }
    
    return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

#define BUFFERSIZE 4096

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int main(void)
{
    // install hook
    //HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    MSG msg;
    
    int delay = 0;
    int vertical = false;
    int horizontal = false;
    
    
    wchar_t window[BUFFERSIZE];
    
    printf(">> Only works on Windows and not in every application.\n");
    printf(">> This program only moves the mouse around.\n");
    printf(">> The program only moves the mouse when the entered window was found,\n");
    printf("   is active and the mouse won't leave the window.\n");
    printf(">> If, for whatever reason, something does go horribly wrong (which I would be surprised)\n");
    printf("   press the key combination \"ctrl+shift+esc\" to open the task manager,\n");
    printf("   where the program will, no matter what, lose control over the mouse.\n");
    printf(">> exact name of targeted window (maybe check task-manager to figure that out): ");
    while( fgetws(window, BUFFERSIZE, stdin) ) /* break with ^D or ^Z */
    {
//      if(wcslen(buffer) && buffer[wcslen(buffer) - 1] == '\n') buffer[wcslen(buffer) - 1] = 0;
        //window = realloc( window, sizeof(wchar_t*) * wcslen(window) + 1 + wcslen(buffer) );
        //window[wcslen(window) + wcslen(buffer)] = 0;
        //if( !text ) ... /* error handling */
        //wcscat( window, buffer ); /* note a '\n' is appended here everytime */
        //wprintf(L"%s\n", buffer);
        break;
    }
    if(wcslen(window) && window[wcslen(window) - 1] == '\n') window[wcslen(window) - 1] = 0;
    printf(">> delay in seconds: ");
    scanf("%d", &delay);
    printf(">> random vertical movement (reply with 1 if yes, 0=no): ");
    scanf("%d", &vertical);
    printf(">> radom horizontal movement (reply with 1 if yes, 0=no): ");
    scanf("%d", &horizontal);
    
    //ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);         // hide console
    
    // Install the low-level keyboard & mouse hooks
    //HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    
    srand(time(0));
    
    // create mouse and initialize it
    sra_mouse_t mouse = sra_mouse_new();
    sra_mouse_setup(&mouse);
    
    // create locate and initialize it
    sra_locate_t locate = sra_locate_new();
    sra_locate_setup(&locate);
    
    // create ai
//    sra_nn_t ai = {0};
//    int ai_size = sra_nn_init(0, 0, 4, (uint32_t[]){160 * 90, 230, 230, 7}); // WASDE VX[mouse x, mouse y, scroll amount]
//    //printf("ai: %d bytes\n", ai_size);
//    if(!sra_nn_init(&ai, 0, 4, (uint32_t[]){160 * 90, 230, 230, 7})) // WASDE VX[mouse x, mouse y, scroll amount]
//    {
//        return 0;
//    }
//    printf("ok0\n");
    
    //if(locate.restrict_window(&locate, "Notepad++ : a free (GNU) source code editor (32 Bit)"))
//    if(!locate.restrict_window(&locate, window))
//    {
//        printf("ok1\n");
//        if(!locate.refresh(&locate))
//        {
//            printf("ok2\n");
//            if(!locate.save_bmp(&locate, L"image.bmp"))
//            {
//                printf("ok3\n");
//            }
//            //printf(".");
//        }
//    }
    
//    for(;;)
//    {
//        while (!GetMessage(&msg, NULL, NULL, NULL))     //this while loop keeps the hook
//        {//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
    
    mouse.restrict_window(&mouse, window);
    for(;;)
    {
        // 90° right,down = 805x
        // 90° left,up = -805x
        //mouse.move(&mouse, 0, 805);
        int x = 0;
        if(horizontal) x = rand() % (2 * 3217) - 3216;
        int y = 0;
        if(vertical) y = rand() % (2 * 3217) - 3216;
        mouse.move(&mouse, x, y);
        //usleep(100000);
        sleep(delay);
    }
    
    // Keep this app running until we're told to stop
    /*MSG Message;
    for(;;)
    {
        BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
        if(MessageResult > 0)
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        else
        {
            break;
        }
    }*/
    
    /*locate.restrict_window(&locate, "Bluestacks");
    locate.refresh(&locate);
    locate.save_bmp(&locate, "image.bmp");
    for(;;)
    {
    }*/
    
    /*locate.refresh(&locate);
    // TODO add this thing to locate.c/h, but add it good
    for(;;)
    {
        HWND handle = FindWindow(NULL, L"Rechner");
        //HWND handle = FindWindowA(NULL, "Rechner");
        RECT rect;
        if(GetWindowRect(handle, &rect))
        {
            if(handle == GetForegroundWindow())
            {
                printf("%4d/%4d;%4d/%4d\n", rect.left, rect.top, rect.right, rect.bottom);
            }
            else
            {
                printf("window not active\n");
            }
        }
        else
        {
            printf("window not found\n");
        }
    }*/
    
    
    //for(int i = 0; i < 999999999; i++) {};
    //mouse.clickl_xy(&mouse, 1000, 1000);
    
    
    /*for(;;)
    {
        
        for(int i = 0; i < 999999999; i++) {};
        mouse.pressl(&mouse);
        for(int i = 0; i < 999999999; i++) {};
        mouse.releasel(&mouse);
    }*/
    
    /* CI below
    bool found = false;
    locate.restrict_window(&locate, L"BlueStacks");
    locate.refresh(&locate);
    for(int i = 0; i < 1000; i++)
    //for(;;)
    {
        
        //for(int i = 0; i < 999999; i++) {};
        
        locate.refresh(&locate);
        
        //found = locate.locate_color_rgb(&locate, 191, 226, 255);    // bright blue
        //found = locate.locate_color_rgb(&locate, 108, 131, 224);    // desaturated blue
        found = locate.locate_color(&locate, 0xff6c83e0);    // desaturated blue
        if(!found)
        {
            //printf("1");
            //mouse.move_xy(&mouse, locate.x, locate.y);
            //mouse.pressl(&mouse);
            mouse.clickl_xy(&mouse, locate.x, locate.y);
        }
        locate.refresh(&locate);
        //found = locate.locate_color_rgb(&locate, 130, 169, 252);    // darker blue
        found = locate.locate_color(&locate, 0xff82a9fc);    // darker blue
        if(!found)
        {
            //printf("2");
            //mouse.move_xy(&mouse, locate.x, locate.y);
            //mouse.pressl(&mouse);
            mouse.clickl_xy(&mouse, locate.x, locate.y);
        }
        
        locate.refresh(&locate);
        found = locate.locate_color(&locate, 0xffb8dbff);    // bright blue
        if(!found)
        {
            //printf("3");
            mouse.clickl_xy(&mouse, locate.x, locate.y);
        }
        
        locate.refresh(&locate);
        found = locate.locate_color(&locate, 0xff758ce9);    // bright blue
        if(!found)
        {
            //printf("4");
            mouse.clickl_xy(&mouse, locate.x, locate.y);
        }
        
        //locate.get_color_rgb_xy(&locate, p.x, p.y);
        //uint32_t col = locate.get_color_xy(&locate, p.x, p.y);
        //printf("%d/%d:%d,%d,%d|%x\n", p.x, p.y, locate.r, locate.g, locate.b, col);
        
        //printf("%d/%d:%x\n", p.x, p.y, col);
    }
    */
    
    /*
    for(;;)
    {
        for(int i = 0; i < 999999999; i++) {};
        mouse.clickl_xy(&mouse, 60000, 60000);
    }*/
    
    
    
    
    /*
    int interval = 99999;
    int delta = 1;
    int direction = 0;
    int changedirection = 1;
    int sum = 0;
    for(;;)
    {
        sum += delta;
        if(sum > changedirection)
        {
            direction = (direction + 1) % 4;
            sum = 0;
        }
        
        switch(direction)
        {
            case 0:
                mouse.move_xy(&mouse, delta, delta);
                break;
            case 1:
                mouse.move_xy(&mouse, -delta, delta);
                break;
            case 2:
                mouse.move_xy(&mouse, -delta, -delta);
                break;
            case 3:
                mouse.move_xy(&mouse, delta, -delta);
                break;
            default:
                break;
        }
        for(int i = 0; i < interval; i++) {};
    }
    */
    //UnhookWindowsHookEx(hhkLowLevelKybd);
    
    sra_mouse_free(&mouse);
//    UnhookWindowsHookEx(hhkLowLevelKybd);
    
    return 0;
}
