#include <windows.h>
#include <glad/glad.h>
#include "raytracer.h"

#define BENCHMARK

#ifdef BENCHMARK
#include <chrono>
#include <cinttypes>
#include <string>
#endif

void* bitmap_memory;
BITMAPINFO bitmap_info;
HGLRC gl_context;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param);

int WINAPI WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, PSTR lp_cmd_line, INT n_cmd_show)
{
    // Setting up the window class    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = h_instance;
    wc.lpszClassName = "Another Ray Tracer Class";
    RegisterClass(&wc);

    // Creating the window
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "Another Ray Tracer",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, //x
        CW_USEDEFAULT, //y
        1938, //width
        1127, //height
        NULL,
        NULL,
        h_instance,
        NULL
    );

    if(hwnd == NULL) {
        return 0;
    }

    //Create image here for now
    RECT rect;
    GetClientRect(hwnd, &rect);
    auto width = rect.right;
    auto height = rect.bottom;
    bitmap_memory = malloc(width * height * 4 * sizeof(uint8_t));

    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = width;
    bitmap_info.bmiHeader.biHeight = -height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    create_raytracing_buffer(width, height);

    #ifdef BENCHMARK
    int step = 0;
    int64_t total_time = 0;
    #endif

    // Create the window message loop
    while(true) {
        MSG msg = {};
        bool running = true;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
            if(msg.message == WM_QUIT)
            {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if(!running) {
            break;
        }

        #ifdef BENCHMARK
        auto start = std::chrono::high_resolution_clock::now();
        #endif

        run_raytracer(width, height, static_cast<uint8_t*>(bitmap_memory));
        InvalidateRect(hwnd, 0, TRUE);
        UpdateWindow(hwnd);

        #ifdef BENCHMARK
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        step++;
        auto str = "Average time per step: " + std::to_string((float) total_time / step);
        SetWindowTextA(hwnd, str.c_str());
        #endif
    }
        
    wglDeleteContext(gl_context);
    return 0;
}

void DrawPixels(HWND hwnd) {
    PAINTSTRUCT ps;
    RECT rect;

    GetClientRect(hwnd, &rect);
    auto width = rect.right;
    auto height = rect.bottom;

    HDC hdc = BeginPaint(hwnd, &ps);
    {
        StretchDIBits(
            hdc,
            0, 0, width, height,
            0, 0, width, height,
            bitmap_memory,
            &bitmap_info,
            DIB_RGB_COLORS,
            SRCCOPY
        );
    }
    

    //glViewport(0, 0, width, height);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(1, 0, 0, 1);
    //glFinish();

    EndPaint(hwnd, &ps);
    
    //SwapBuffers(hDC);
}

bool CreateGLContext(HWND hwnd) {
    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

    HDC hdc = GetDC(hwnd);

    int pixel_format;
    pixel_format = ChoosePixelFormat(hdc, &pfd); 
    SetPixelFormat(hdc, pixel_format, &pfd);

    gl_context = wglCreateContext(hdc);
    wglMakeCurrent (hdc, gl_context);

    if(!gladLoadGL()) {
        PostQuitMessage(0);
        return false;
    }

    return true;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
    switch(u_msg) {
        case WM_CREATE:
            if(!CreateGLContext(hwnd)) {
                PostQuitMessage(0);
            }
            //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
            return 0;
        case WM_PAINT:
            DrawPixels(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, u_msg, w_param, l_param);
}

