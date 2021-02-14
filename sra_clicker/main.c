
#include "main.h"

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
            
            // /*
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

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main(void)
{
    //ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);         // hide console
    
    // Install the low-level keyboard & mouse hooks
    //HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    
    // create mouse and initialize it
    sra_mouse_t mouse;
    sra_mouse_setup(&mouse);
    
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
    
    
    //for(int i = 0; i < 999999999; i++) {};
    //mouse.clickl_xy(&mouse, 1000, 1000);
    
    
    for(;;)
    {
        
        for(int i = 0; i < 999999999; i++) {};
        mouse.pressl(&mouse);
        for(int i = 0; i < 999999999; i++) {};
        mouse.releasel(&mouse);
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
    }*/
    
    //UnhookWindowsHookEx(hhkLowLevelKybd);
    
    sra_mouse_free(&mouse);
    
    return 0;
}
