#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h>

#define GL_GLEXT_PROTOTYPES
#include <SDL3/SDL_opengles2_gl2ext.h>

// 全局 OpenGL 函数指针
static PFNGLCLEARCOLORPROC glClearColor2 = NULL;
static PFNGLCLEARPROC glClear2 = NULL;
static PFNGLGENBUFFERSPROC glGenBuffers2 = NULL;
static PFNGLBINDBUFFERPROC glBindBuffer2 = NULL;
static PFNGLBUFFERDATAPROC glBufferData2 = NULL;
static PFNGLCREATESHADERPROC glCreateShader2 = NULL;
static PFNGLSHADERSOURCEPROC glShaderSource2 = NULL;
static PFNGLCOMPILESHADERPROC glCompileShader2 = NULL;
static PFNGLCREATEPROGRAMPROC glCreateProgram2 = NULL;
static PFNGLATTACHSHADERPROC glAttachShader2 = NULL;
static PFNGLLINKPROGRAMPROC glLinkProgram2 = NULL;
static PFNGLUSEPROGRAMPROC glUseProgram2 = NULL;
static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation2 = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray2 = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer2 = NULL;
static PFNGLDRAWARRAYSPROC glDrawArrays2 = NULL;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers2 = NULL;
static PFNGLDELETESHADERPROC glDeleteShader2 = NULL;
static PFNGLDELETEPROGRAMPROC glDeleteProgram2 = NULL;

// 全局资源
static SDL_Window* g_window = NULL;
static SDL_GLContext g_gl_ctx = NULL;

// 矩形渲染资源
static GLuint g_program = 0;
static GLuint g_vbo = 0;
static GLint g_pos_attr = -1;

// 顶点着色器
static const char* vertex_shader =
"attribute vec2 aPos;\n"
"void main() {\n"
"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}";

// 片段着色器
static const char* fragment_shader =
"precision mediump float;\n"
"void main() {\n"
"    gl_FragColor = vec4(0.2f, 0.8f, 0.4f, 1.0f);\n"
"}";

// 矩形顶点数据（NDC 坐标，居中显示）
static const float rect_vertices[] = {
    -0.5f,  0.5f,
    -0.5f, -0.5f,
     0.5f,  0.5f,
     0.5f, -0.5f,
};

// 加载所有 GLES2 函数指针
static int LoadGLES2Functions(void)
{
        glClearColor2 = (PFNGLCLEARCOLORPROC)SDL_GL_GetProcAddress("glClearColor");
        glClear2 = (PFNGLCLEARPROC)SDL_GL_GetProcAddress("glClear");
        glGenBuffers2 = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
        glBindBuffer2 = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
        glBufferData2 = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
        glCreateShader2 = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
        glShaderSource2 = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
        glCompileShader2 = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
        glCreateProgram2 = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
        glAttachShader2 = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
        glLinkProgram2 = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
        glUseProgram2 = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
        glGetAttribLocation2 = (PFNGLGETATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glGetAttribLocation");
        glEnableVertexAttribArray2 = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
        glVertexAttribPointer2 = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
        glDrawArrays2 = (PFNGLDRAWARRAYSPROC)SDL_GL_GetProcAddress("glDrawArrays");
        glDeleteBuffers2 = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
        glDeleteShader2 = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
        glDeleteProgram2 = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");

        if (!glClearColor2 || !glClear2 || !glGenBuffers2 || !glBindBuffer2 ||
                !glBufferData2 || !glCreateShader2 || !glShaderSource2 || !glCompileShader2 ||
                !glCreateProgram2 || !glAttachShader2 || !glLinkProgram2 || !glUseProgram2 ||
                !glGetAttribLocation2 || !glEnableVertexAttribArray2 || !glVertexAttribPointer2 ||
                !glDrawArrays2) {
                SDL_Log("Failed to load GLES2 functions");
                return -1;
        }
        return 0;
}

// 编译着色器
static GLuint CompileShader(GLenum type, const char* source)
{
        GLuint shader = glCreateShader2(type);
        glShaderSource2(shader, 1, &source, NULL);
        glCompileShader2(shader);
        return shader;
}

// 创建着色器程序
static int CreateShaderProgram(void)
{
        GLuint vs = CompileShader(GL_VERTEX_SHADER, vertex_shader);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

        g_program = glCreateProgram2();
        glAttachShader2(g_program, vs);
        glAttachShader2(g_program, fs);
        glLinkProgram2(g_program);

        glDeleteShader2(vs);
        glDeleteShader2(fs);

        g_pos_attr = glGetAttribLocation2(g_program, "aPos");
        if (g_pos_attr < 0) {
                SDL_Log("Attrib location aPos not found");
                return -1;
        }
        return 0;
}

// 创建顶点缓冲区
static int CreateRectangleBuffer(void)
{
        glGenBuffers2(1, &g_vbo);
        glBindBuffer2(GL_ARRAY_BUFFER, g_vbo);
        glBufferData2(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);
        return 0;
}

// 初始化回调
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
        (void)argc;
        (void)argv;
        *appstate = NULL;

        if (!SDL_Init(SDL_INIT_VIDEO)) {
                SDL_Log("SDL_Init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
        }

        // GLES 2.0 配置
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        g_window = SDL_CreateWindow("SDL3 GLES2 Rectangle", 800, 600,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!g_window) {
                SDL_Log("Create window failed");
                return SDL_APP_FAILURE;
        }

        g_gl_ctx = SDL_GL_CreateContext(g_window);
        if (!g_gl_ctx) {
                SDL_Log("Create GL context failed");
                return SDL_APP_FAILURE;
        }

        if (LoadGLES2Functions() != 0) return SDL_APP_FAILURE;
        if (CreateShaderProgram() != 0) return SDL_APP_FAILURE;
        if (CreateRectangleBuffer() != 0) return SDL_APP_FAILURE;

        glUseProgram2(g_program);
        glVertexAttribPointer2(g_pos_attr, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray2(g_pos_attr);

        glClearColor2(0.1f, 0.1f, 0.15f, 1.0f);
        return SDL_APP_CONTINUE;
}

// 事件处理
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
        (void)appstate;
        if (event->type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
        }
        return SDL_APP_CONTINUE;
}

// 主循环：绘制矩形
SDL_AppResult SDL_AppIterate(void* appstate)
{
        (void)appstate;

        glClear2(GL_COLOR_BUFFER_BIT);
        // 绘制矩形（GL_TRIANGLE_STRIP 绘制4个顶点）
        glDrawArrays2(GL_TRIANGLE_STRIP, 0, 4);

        SDL_GL_SwapWindow(g_window);
        return SDL_APP_CONTINUE;
}

// 资源释放
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
        (void)appstate;
        (void)result;

        if (g_vbo) glDeleteBuffers2(1, &g_vbo);
        if (g_program) glDeleteProgram2(g_program);

        SDL_GL_DestroyContext(g_gl_ctx);
        SDL_DestroyWindow(g_window);
        SDL_Quit();
}