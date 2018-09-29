#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>

// GLM headers
#define GLM_FORCE_PURE
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../stb_image.h"
#include "devkitlenny_png.h"

#include "../nxlink.h"

#include "model.h"

GLuint loadShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	printf("Reading vertex shader...\n");
	std::ifstream vertexShaderFile(vertexShaderPath);
	vertexShaderFile.seekg(0, std::ios::end);
	std::vector<char> vertexShaderData(vertexShaderFile.tellg());
	vertexShaderFile.seekg(0, std::ios::beg);
	vertexShaderFile.read(vertexShaderData.data(), vertexShaderData.size());

	printf("Reading fragment shader...\n");
	std::ifstream fragmentShaderFile(fragmentShaderPath);
	fragmentShaderFile.seekg(0, std::ios::end);
	std::vector<char> fragmentShaderData(fragmentShaderFile.tellg());
	fragmentShaderFile.seekg(0, std::ios::beg);
	fragmentShaderFile.read(fragmentShaderData.data(), fragmentShaderData.size());

	const char* vertexShaderCode = vertexShaderData.data();
	const char* fragmentShaderCode = fragmentShaderData.data();

	// Create the shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertexShaderPath.c_str());
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexShaderCode, NULL);
	glCompileShader(vertexShader);

	// Check Vertex Shader
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char* vertexShaderErrorMessage = (char*)malloc(infoLogLength + 1);
		glGetShaderInfoLog(vertexShader, infoLogLength, NULL, vertexShaderErrorMessage);
		fprintf(stderr, "%s\n", vertexShaderErrorMessage);
		free(vertexShaderErrorMessage);
		exit(1);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragmentShaderPath.c_str());
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);

	// Check Fragment Shader
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char* fragmentShaderErrorMessage = (char*)malloc(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShader, infoLogLength, NULL, fragmentShaderErrorMessage);
		fprintf(stderr, "%s\n", fragmentShaderErrorMessage);
		free(fragmentShaderErrorMessage);
		exit(1);
	}

	// Link the program
	printf("Linking program\n");
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	// Check the program
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char* programErrorMessage = (char*)malloc(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, NULL, programErrorMessage);
		printf("%s\n", programErrorMessage);
		free(programErrorMessage);
		exit(1);
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void GLAPIENTRY
MessageCallback( 
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    //if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    //{
    //    // these are just annoying
    //    return;
    //}

    printf("OpenGL message: %s\n", message);
    printf("type: ");
    switch (type) 
    {
    case GL_DEBUG_TYPE_ERROR:
        printf("ERROR");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        printf("DEPRECATED_BEHAVIOR");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        printf("UNDEFINED_BEHAVIOR");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        printf("PORTABILITY");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        printf("PERFORMANCE");
        break;
    case GL_DEBUG_TYPE_OTHER:
        printf("OTHER");
        break;
    }
    printf(", ");
 
    printf("id: %u, ", id);
    printf("severity: ");
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        printf("NOTIFICATION");
        break;
    case GL_DEBUG_SEVERITY_LOW:
        printf("LOW");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        printf("MEDIUM");
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        printf("HIGH");
        break;
    }
    printf("\n");
}

//-----------------------------------------------------------------------------
// EGL initialization
//-----------------------------------------------------------------------------

static bool initEgl(EGLDisplay& display, EGLSurface& surface, EGLContext& context)
{
	// Connect to the EGL default display
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (!display)
	{
		TRACE("Could not connect to display! error: %d", eglGetError());
		goto _fail0;
	}

	// Initialize the EGL display connection
	eglInitialize(display, nullptr, nullptr);

	// Select OpenGL (Core) as the desired graphics API
	if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
	{
		TRACE("Could not set API! error: %d", eglGetError());
		goto _fail1;
	}

	// Get an appropriate EGL framebuffer configuration
	EGLConfig config;
	EGLint numConfigs;
	static const EGLint framebufferAttributeList[] =
	{
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_NONE
	};
	eglChooseConfig(display, framebufferAttributeList, &config, 1, &numConfigs);
	if (numConfigs == 0)
	{
		TRACE("No config found! error: %d", eglGetError());
		goto _fail1;
	}

	// Create an EGL window surface
	surface = eglCreateWindowSurface(display, config, (char*)"", nullptr);
	if (!surface)
	{
		TRACE("Surface creation failed! error: %d", eglGetError());
		goto _fail1;
	}

	// Create an EGL rendering context
	static const EGLint contextAttributeList[] =
	{
		EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
		EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
		EGL_CONTEXT_MINOR_VERSION_KHR, 3,
		EGL_NONE
	};
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributeList);
	if (!context)
	{
		TRACE("Context creation failed! error: %d", eglGetError());
		goto _fail2;
	}

	// Connect the context to the surface
	eglMakeCurrent(display, surface, surface, context);
	return true;

_fail2:
	eglDestroySurface(display, surface);
	surface = nullptr;
_fail1:
	eglTerminate(display);
	display = nullptr;
_fail0:
	return false;
}

static void deinitEgl(EGLDisplay& display, EGLSurface& surface, EGLContext& context)
{
	if (display)
	{
		if (context)
		{
			eglDestroyContext(display, context);
			context = nullptr;
		}
		if (surface)
		{
			eglDestroySurface(display, surface);
			surface = nullptr;
		}
		eglTerminate(display);
		display = nullptr;
	}
}

//-----------------------------------------------------------------------------
// Main program
//-----------------------------------------------------------------------------

static void setMesaConfig()
{
	// Uncomment below to disable error checking and save CPU time (useful for production):
	//setenv("MESA_NO_ERROR", "1", 1);

	// Uncomment below to enable Mesa logging:
	setenv("EGL_LOG_LEVEL", "debug", 1);
	setenv("MESA_VERBOSE", "all", 1);
	setenv("NOUVEAU_MESA_DEBUG", "1", 1);

	// Uncomment below to enable shader debugging in Nouveau:
	//setenv("NV50_PROG_OPTIMIZE", "0", 1);
	//setenv("NV50_PROG_DEBUG", "1", 1);
	//setenv("NV50_PROG_CHIPSET", "0x120", 1);
}

bool CRenderer::Init()
{
	setMesaConfig();

	// Init EGL
	if (!initEgl(_display, _surface, _context))
	{
		TRACE("Egl could not be initialised");
		return false;
	}

	// Load OpenGL routines using glad
	gladLoadGL();

	printf("GL Vendor: %s\n", glGetString(GL_VENDOR));
    printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("GL Version: %s\n", glGetString(GL_VERSION));
    printf("GL Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));	

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	TRACE("Renderer initialised successfully!");

#if 0
	GLint n = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	for (GLint i = 0; i < n; i++)
	{
		const char* extension =
			(const char*)glGetStringi(GL_EXTENSIONS, i);
		TRACE("Ext %d: %s\n", i, extension);
	}
#endif

	CModel* model = CModel::LoadFromOBJ("Test", "romfs:/models/triangle.obj");
	if (model == nullptr)
		TRACE("Model load failed");
	else
		TRACE("Model load successful");

	return true;
}

void CRenderer::Deinit()
{
	deinitEgl(_display, _surface, _context);
}

void CRenderer::Render()
{
}

void CRenderer::Present()
{
	eglSwapBuffers(_display, _surface);
}

