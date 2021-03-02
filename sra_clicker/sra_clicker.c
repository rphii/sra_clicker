
#include "sra_clicker.h"

/*LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    // thanks to
    //https://stackoverflow.com/questions/22975916/global-keyboard-hook-with-wh-keyboard-ll-and-keybd-event-windows
    
    BOOL fEatKeystroke = FALSE;
    
    switch (wParam)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
            
            if((fEatKeystroke = (p->vkCode == VK_F8)))
            {
                
            }
            
            // 
            if ((fEatKeystroke = (p->vkCode == 0x41)))  //redirect a to b
            {     
                printf("Hello a\n");

                if ( (wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN) ) // Keydown
                {
                    keybd_event('B', 0, 0, 0);
                }
                else if ( (wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP) ) // Keyup
                {
                    keybd_event('B', 0, KEYEVENTF_KEYUP, 0);
                }
                break;
            // 
            
            break;
        }
        default:
            break;
    }
    
    return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}*/


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int main(void)
{
    //ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);         // hide console
    
    // Install the low-level keyboard & mouse hooks
    //HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    
    // create mouse and initialize it
    sra_mouse_t mouse = sra_mouse_new();
    sra_mouse_setup(&mouse);
    
    // create locate and initialize it
    sra_locate_t locate = sra_locate_new();
    sra_locate_setup(&locate);
    
    //if(locate.restrict_window(&locate, "Notepad++ : a free (GNU) source code editor (32 Bit)"))
//    for(;;)
//    {
        if(!locate.restrict_window(&locate, L"Task-Manager"))
        {
//            printf("ok1\n");
            if(!locate.refresh(&locate))
            {
//                printf("ok2\n");
                if(!locate.save_bmp(&locate, L"image.bmp"))
                {
                    printf("ok3\n");
                }
                //printf(".");
            }
            
        }
//    }
    
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
    
    return 0;
}
