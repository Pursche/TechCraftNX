#pragma once

#include <switch.h>
#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)

class CRenderer
{
public:
	bool Init();
	void Deinit();

	void Render();
	void Present();
private:

	EGLDisplay _display;
	EGLContext _context;
	EGLSurface _surface;
};