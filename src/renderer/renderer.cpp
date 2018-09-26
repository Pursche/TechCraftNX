#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
	//setenv("EGL_LOG_LEVEL", "debug", 1);
	//setenv("MESA_VERBOSE", "all", 1);
	//setenv("NOUVEAU_MESA_DEBUG", "1", 1);

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


static float r = 0;
static float g = 0;
static float b = 0;

void CRenderer::Render()
{
	r += 0.01f;
	g += 0.005f;
	b += 0.007f;

	if (r > 1.0f) r = 0;
	if (g > 1.0f) g = 0;
	if (b > 1.0f) b = 0;

	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CRenderer::Present()
{
	eglSwapBuffers(_display, _surface);
}

