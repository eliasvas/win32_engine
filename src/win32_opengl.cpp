#include <windows.h>
#include <gl/gl.h>
#include "ext/glext.h"
#include "ext/wglext.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "help.h"

#pragma comment (lib, "opengl32.lib")

/* In short: We get a Device Context with GetDC(),
 * we pick a hardware accelerated pixel format with ChoosePixelFormat(),
 * and associate it with a Device Context via SetPixelFormat(),
 * then we create a Rendering Context with wglCreateContext() and
 * set it as current with wglMakeCurrent()*/

/*The problem is ChoosePixelFormat() and SetPixelFormat() are not extensible
 * so you can't enable multisampling, floating-point framebuffers etc.
 * So we need two new functions, wglChoosePixelFormatARB() and wglCreateContextAttribsARB()
 * but to get their function pointers.. we need a Rendering Context! So we have to make a 
 * fake one first obtain the function pointers and then destroy it and make a proper one!*/

static HGLRC win32_opengl_context;

//TODO(ilias): automate the whole process..
static PFNWGLCHOOSEPIXELFORMATARBPROC     wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC  wglCreateContextAttribsARB;
static PFNWGLMAKECONTEXTCURRENTARBPROC    wglMakeContextCurrentARB;
static PFNWGLSWAPINTERVALEXTPROC          wglSwapIntervalEXT;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLDELETESHADERPROC glDeleteShader;
static PFNGLDELETEPROGRAMPROC glDeleteProgram;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;
static PFNGLUNIFORM1IPROC glUniform1i;
static PFNGLUNIFORM1FPROC glUniform1f;
static PFNGLACTIVETEXTUREPROC glActiveTexture;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
//static PFNGLTEXTUREPARAMETERIPROC glTexParameteri;
static PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

void *GetAnyGLFuncAddress(const char *name)
{
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1))
  {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }

  return p;
}

//NOTE(ilias): maybe this should happen in fake window stage?
static void 
LoadAllOpenGLProcedures()
{
   glGenBuffers = (PFNGLGENBUFFERSPROC)GetAnyGLFuncAddress("glGenBuffers"); 
   glBindBuffer = (PFNGLBINDBUFFERPROC)GetAnyGLFuncAddress("glBindBuffer"); 
   glUseProgram = (PFNGLUSEPROGRAMPROC)GetAnyGLFuncAddress("glUseProgram"); 
   glShaderSource = (PFNGLSHADERSOURCEPROC)GetAnyGLFuncAddress("glShaderSource"); 
   glCompileShader = (PFNGLCOMPILESHADERPROC)GetAnyGLFuncAddress("glCompileShader"); 
   glGetShaderiv = (PFNGLGETSHADERIVPROC)GetAnyGLFuncAddress("glGetShaderiv"); 
   glCreateShader = (PFNGLCREATESHADERPROC)GetAnyGLFuncAddress("glCreateShader"); 
   glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetAnyGLFuncAddress("glGetShaderInfoLog"); 
   glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetAnyGLFuncAddress("glCreateProgram"); 
   glAttachShader = (PFNGLATTACHSHADERPROC)GetAnyGLFuncAddress("glAttachShader"); 
   glLinkProgram = (PFNGLLINKPROGRAMPROC)GetAnyGLFuncAddress("glLinkProgram"); 
   glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetAnyGLFuncAddress("glGetProgramiv"); 
   glUniform1i = (PFNGLUNIFORM1IPROC)GetAnyGLFuncAddress("glUniform1i"); 
   glUniform1f = (PFNGLUNIFORM1FPROC)GetAnyGLFuncAddress("glUniform1f"); 
   glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetAnyGLFuncAddress("glUniformMatrix4fv");
   glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetAnyGLFuncAddress("glGetUniformLocation"); 
   glDeleteShader = (PFNGLDELETESHADERPROC)GetAnyGLFuncAddress("glDeleteShader"); 
   glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetAnyGLFuncAddress("glDeleteProgram"); 
   glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)GetAnyGLFuncAddress("glGenVertexArrays"); 
   glBufferData = (PFNGLBUFFERDATAPROC)GetAnyGLFuncAddress("glBufferData"); 
   glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetAnyGLFuncAddress("glBindVertexArray"); 
   glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GetAnyGLFuncAddress("glGetProgramInfoLog"); 
   glActiveTexture = (PFNGLACTIVETEXTUREPROC)GetAnyGLFuncAddress("glActiveTexture");
   glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetAnyGLFuncAddress("glVertexAttribPointer"); 
   glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetAnyGLFuncAddress("glEnableVertexAttribArray"); 
   //glTexParameteri = (PFNGLTEXTUREPARAMETERIPROC)GetAnyGLFuncAddress("glTextureParameteri"); 
   glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)GetAnyGLFuncAddress("glGenerateMipmap"); 
   //and on and on and on......
}



b32 Win32InitOpenGL(HDC* device_context, HINSTANCE Instance){
   
    PIXELFORMATDESCRIPTOR fakePFD;
    ZeroMemory(&fakePFD, sizeof(fakePFD));
    fakePFD.nSize = sizeof(fakePFD);
    fakePFD.nVersion = 1;
    fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePFD.iPixelType = PFD_TYPE_RGBA;
    fakePFD.cColorBits = 32;
    fakePFD.cAlphaBits = 8;
    fakePFD.cDepthBits = 24;
     
    int fakePFDID = ChoosePixelFormat(*device_context, &fakePFD);
    //NOTE(ilias): we can assign a PIXELFORMAT ONCE for every window!!
    SetPixelFormat(*device_context, fakePFDID, &fakePFD);


    //we create a temporary rendering context and set it as current
    HGLRC fakeRC = wglCreateContext(*device_context);    // Rendering Contex
    wglMakeCurrent(*device_context, fakeRC);

    {
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)GetAnyGLFuncAddress("wglChoosePixelFormatARB");
        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)GetAnyGLFuncAddress("wglCreateContextAttribsARB");
        wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)GetAnyGLFuncAddress("wglMakeContextCurrentARB");
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)GetAnyGLFuncAddress("wglSwapIntervalEXT");
        LoadAllOpenGLProcedures();
    }
    //now we can call wglChoosePixelFormatARB() and wglCreateContextAttribsARB()
    //we need to repeat for every function we need to get


    const int pixelAttribs[] = {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    WGL_SAMPLES_ARB, 4,
    0
    };
    i32 pixelFormatID;
    u32 numFormats;
    b32 status = wglChoosePixelFormatARB(*device_context, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
    
    //DescribePixelFormat(*device_context, pixelFormatID, sizeof(PFD), &PFD); //maybe not needed
    //SetPixelFormat(*device_context, pixelFormatID, &PFD); //maybe not needed


    const int major_min = 3, minor_min = 3; //NOTE(ilias): maybe make it 3.3?
    int  contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
     
    win32_opengl_context = wglCreateContextAttribsARB(*device_context, 0, contextAttribs);
    if (win32_opengl_context) {
        //NOTE(ilias): we make the new Rendering Context the current one!
        wglMakeCurrent(*device_context, NULL);
        wglDeleteContext(fakeRC);
        wglMakeCurrent(*device_context, win32_opengl_context);
        wglSwapIntervalEXT(0);
        status = 1;
    }
        return status;
}


