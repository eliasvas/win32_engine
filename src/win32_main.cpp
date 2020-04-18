#define WIN32_LEAN_AND_MEAN
char infoLog[512]; //errors are written in here!
#include <windows.h>
#include "win32_opengl.cpp"
#include "game.c"
#include "tb.h"
#include "platform.h"
#include "text.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ext/stb_image.h"
#define HANDMADE_MATH_IMPLEMENTATION
#include "ext/HandmadeMath.h"

static LARGE_INTEGER fr,st,ft;
static HDC global_device_context;
static platform global_platform;

static LRESULT Win32WindowProc(HWND hWnd, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = {0};
    if (message == WM_SIZE)
        {
           RECT rect;
           GetClientRect(hWnd, &rect);
           glViewport(0, 0, (GLsizei)rect.right, (GLsizei)rect.bottom); //for some reason this is useless 
           global_platform.window_width = rect.right;
           global_platform.window_height = rect.bottom;
    }else if (message == WM_CLOSE || message == WM_DESTROY || message == WM_QUIT){
        global_platform.exit = 1;
    }else if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP || message == WM_KEYDOWN || message == WM_KEYUP){
        u64 vkey_code = w_param;
        i8 was_down = !!(l_param & (1 << 30));
        i8 is_down = !(l_param & (1 << 31));

        u64 key_input =0;

        if ((vkey_code >= 'A' && vkey_code <= 'Z') || (vkey_code >='0' && vkey_code <= '9')){
            key_input = (vkey_code >='A' && vkey_code <= 'Z') ? KEY_A + (vkey_code - 'A') : KEY_0 + (vkey_code - '0');
        }else {
            if (vkey_code == VK_F4){
                key_input = KEY_F4;
            }else if (vkey_code == VK_SPACE){
                key_input = KEY_SPACE;
            }else if (vkey_code == VK_MENU)
            {
                key_input= KEY_ALT;
            }else if (vkey_code == VK_TAB)
             {
                 key_input = KEY_TAB;
             }
            //handle more keys
        }
        if (is_down){
           if (!global_platform.key_down[key_input])
           {
               ++global_platform.key_pressed[key_input];
           }
           ++global_platform.key_down[key_input];
           global_platform.last_key = (i32)key_input;
           if(global_platform.key_down[KEY_ALT] && key_input == KEY_F4)
            {
                global_platform.exit = 1;
            }
        }else 
        {
            global_platform.key_down[key_input] = 0;
            global_platform.key_pressed[key_input] = 0;
        }
    }
    else{
        result = DefWindowProc(hWnd, message, w_param, l_param);
    }
    return result;
}

static void
Win32CleanUpOpenGL(HDC *device_context)
{
    wglMakeCurrent(*device_context, 0);
    wglDeleteContext(win32_opengl_context);
}

static void
Win32OpenGLSetVerticalSync(b32 vsync)
{
    wglSwapIntervalEXT(!!vsync);
}

static void
Win32OpenGLSwapBuffers(HDC device_context)
{
    wglSwapLayerBuffers(device_context, WGL_SWAP_MAIN_PLANE);
}

i32 CALLBACK
WinMain(HINSTANCE Instance,

        HINSTANCE PrevInstance,

        LPSTR CommandLine,

        i32 ShowCode)

{
    WNDCLASS windowClass = {0};
    {
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = Win32WindowProc;
        windowClass.hInstance = Instance;
        windowClass.lpszClassName = "WindowClass";
        windowClass.hCursor = LoadCursor(0, IDC_ARROW);

    }
    RegisterClass(&windowClass);
    HWND WND = CreateWindow(
            windowClass.lpszClassName, "Window",      // window class, title
            WS_OVERLAPPEDWINDOW, // style
            CW_USEDEFAULT,CW_USEDEFAULT,800,600,//CW_USEDEFAULT,CW_USEDEFAULT,
            NULL, NULL,                 // parent window, menu
            Instance, NULL);           // instance, param
     
    HDC DC = GetDC(WND);        // Device Context

    //initializing the platform layer
    {
        global_platform.exit = 0;
        global_platform.window_width = 800;
        global_platform.window_height = 600;
        global_platform.target_fps = 60.f;
        global_platform.current_time = 0.f;
    }

    Win32InitOpenGL(&DC, Instance); 
    //SetWindowText(WND, (LPCSTR)glGetString(GL_VERSION));
    ShowWindow(WND, ShowCode);

    MSG msg;
    QueryPerformanceFrequency(&fr);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

    init();
    while (!global_platform.exit){
        QueryPerformanceCounter(&st);
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        {
            RECT client_rect;
            GetClientRect(WND, &client_rect);
            global_platform.window_width = client_rect.right - client_rect.left;
            global_platform.window_height = client_rect.bottom - client_rect.top;
        }
        f32 dt = (st.QuadPart - ft.QuadPart)/ (float)fr.QuadPart; //NOTE(ilias): check on actual simulation!!
        global_platform.dt = dt;
        global_platform.current_time +=1.f/60;//dt;
        update(&global_platform);
        render(&DC, &global_platform);
        QueryPerformanceCounter(&ft);

        if (global_platform.vsync){
            //NOTE(ilias): wait remaining time
            i64 frame_count = ft.QuadPart - st.QuadPart;
            i64 desired_frame_count = (f32)fr.QuadPart / global_platform.target_fps;
            i64 counts_to_wait = desired_frame_count - frame_count;

            LARGE_INTEGER begin_wait_time_delta;
            LARGE_INTEGER end_wait_time_delta;
            QueryPerformanceCounter(&begin_wait_time_delta);
            //kills the CPU for the delta
            while(counts_to_wait> 0)
            {
                QueryPerformanceCounter(&end_wait_time_delta);
                counts_to_wait -= (end_wait_time_delta.QuadPart - begin_wait_time_delta.QuadPart);
                begin_wait_time_delta = end_wait_time_delta;
            }
        }
        if (strlen(infoLog) != 0){
            MessageBox(WND, infoLog, "FATAL ERROR", MB_OK);
            exit(1);
        }

    }
    return 0;
}
