#ifdef USE_GL
#include <zg/renderers/GLRenderer.hpp>
#include <zg/Window.hpp>
#include <zg/entities/Plane.hpp>
#include <zg/Logger.hpp>
#ifdef _WIN32
#include <zg/SharedLibrary.hpp>
#endif
#if defined(LINUX) || defined(MACOS)
#include <dlfcn.h>
#endif
#ifdef LINUX
#include <zg/windows/X11Window.hpp>
#endif
using namespace zg;
#ifdef _WIN32
typedef BOOL (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int interval);
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
zg::SharedLibrary opengl32Library("opengl32.dll");
#endif
void *getProc(const char* name) {
#ifdef _WIN32
	void* p = (void*)wglGetProcAddress(name);
	if(p == 0 ||
		 (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		 (p == (void*)-1) )
	{
		return opengl32Library.getProc<void*>(name);
	}
	return p;
#elif defined(LINUX) || defined(MACOS)
	return dlsym(RTLD_DEFAULT, name);
#endif
}
GLRenderer::GLRenderer():
	glContext(new GladGLContext)
{}
GLRenderer::~GLRenderer()
{
	delete glContext;
}
void GLRenderer::init(IPlatformWindow* platformWindowPointer)
{
	this->platformWindowPointer = platformWindowPointer;
	gladLoadGLContext(glContext, (GLADloadfunc)getProc);
	glContext->Enable(GL_DEPTH_TEST);
	GLcheck(*this, "glEnable");
	glContext->Enable(GL_CULL_FACE);
	GLcheck(*this, "glEnable");
	glContext->CullFace(GL_BACK);
	GLcheck(*this, "glCullFace");
	glContext->FrontFace(GL_CCW);
	GLcheck(*this, "glFrontFace");
	glContext->Viewport(0, 0, platformWindowPointer->renderWindowPointer->windowWidth, platformWindowPointer->renderWindowPointer->windowHeight);
	GLcheck(*this, "glViewport");
	glContext->ClearDepth(1.0);
	GLcheck(*this, "glClearDepth");
	glContext->DepthRange(0.0, 1.0);
	GLcheck(*this, "glDepthRange");
	glContext->Enable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	GLcheck(*this, "glEnable:GL_SAMPLE_ALPHA_TO_COVERAGE");
	glContext->Enable(GL_BLEND);
	GLcheck(*this, "glEnable:GL_BLEND");
	glContext->Enable(GL_DITHER);
	GLcheck(*this, "glEnable:GL_DITHER");
#ifndef MACOS
	glContext->BlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
	GLcheck(*this, "glBlendEquationSeparate");
	glContext->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLcheck(*this, "glBlendFunc");
	glContext->Enable(GL_DEBUG_OUTPUT);
	GLcheck(*this, "glEnable");
	glContext->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	GLcheck(*this, "glEnable");
	glContext->DebugMessageCallback([](GLuint source, GLuint type, GLuint id, GLuint severity, GLsizei length, const GLchar* message, const void* userParam) {
		if (type == GL_DEBUG_TYPE_ERROR )
		{
			std::cerr << "OpenGL Debug Message: " << message << std::endl;
		}
	}, nullptr);
#endif
#ifdef _WIN32
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(platformWindowPointer->renderWindowPointer->vsync);
#elif defined(LINUX)
	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalEXT");
	auto &x11Window = *(X11Window *)platformWindowPointer;
	glXSwapIntervalEXT(x11Window.display, x11Window.window, platformWindowPointer->renderWindowPointer->vsync);
#endif
}
void GLRenderer::render()
{
	auto &renderWindow = *platformWindowPointer->renderWindowPointer;
	renderWindow.render();
	for (auto &childWindowPointer : renderWindow.childWindows)
	{
		auto &childWindow = *childWindowPointer;
		if (childWindow.minimized)
			continue;
		childWindow.framebufferPlane->render();
	}
	platformWindowPointer->swapBuffers();
}
void GLRenderer::destroy(){}
std::shared_ptr<IVendorRenderer> zg::createVendorRenderer()
{
	return std::shared_ptr<IVendorRenderer>(new GLRenderer());
}
void GLRenderer::clearColor(glm::vec4 color)
{
	glContext->ClearColor(color.r, color.g, color.b, color.a);
	GLcheck(*this, "glClearColor");
}
void GLRenderer::clear()
{
	glContext->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	GLcheck(*this, "glClear");
}
void GLRenderer::viewport(glm::ivec4 vp)
{
	glContext->Viewport(vp.x, vp.y, vp.z, vp.w);
	GLcheck(*this, "glViewport");
}
void GLRenderer::setUniform(shaders::Shader &shader, const std::string_view name, const void *value, uint32_t size, enums::EUniformType uniformType)
{
	GLint location = glContext->GetUniformLocation(shader.program, name.data());
	GLcheck(*this, "glGetUniformLocation");
	auto pointer = &value;
	if (location == -1)
	{
		return;
	}
	switch (uniformType)
	{
	case enums::EUniformType::_1b:
	{
		glContext->Uniform1i(location, (int32_t)(*(bool*)pointer));
		GLcheck(*this, "glUniform1i");
		return;
	}
	case enums::EUniformType::_1i:
	{
		glContext->Uniform1i(location, *(int32_t*)pointer);
		GLcheck(*this, "glUniform1i");
		return;
	}
	case enums::EUniformType::_1ui:
	{
		glContext->Uniform1ui(location, *(uint32_t*)pointer);
		GLcheck(*this, "glUniform1ui");
		return;
	}
	case enums::EUniformType::_1fv:
	{
		glContext->Uniform1fv(location, size, &((float**)pointer)[0][0]);
		GLcheck(*this, "glUniform1fv");
		return;
	}
	case enums::EUniformType::_1f:
	{
		glContext->Uniform1f(location, *(float*)pointer);
		GLcheck(*this, "glUniform1f");
		return;
	}
	case enums::EUniformType::_2fv:
	{
		glContext->Uniform2fv(location, 1, &(*(glm::vec2*)pointer)[0]);
		GLcheck(*this, "glUniform2fv");
		return;
	}
	case enums::EUniformType::_3fv:
	{
		glContext->Uniform3fv(location, 1, &(*(glm::vec3*)pointer)[0]);
		GLcheck(*this, "glUniform3fv");
		return;
	}
	case enums::EUniformType::_4fv:
	{
		glContext->Uniform4fv(location, 1, &(*(glm::vec4*)pointer)[0]);
		GLcheck(*this, "glUniform4fv");
		return;
	}
	case enums::EUniformType::_Matrix2fv:
	{
		glContext->UniformMatrix2fv(location, 1, GL_FALSE, &(*(glm::mat2*)pointer)[0][0]);
		GLcheck(*this, "glUniformMatrix2fv");
		return;
	}
	case enums::EUniformType::_Matrix3fv:
	{
		glContext->UniformMatrix3fv(location, 1, GL_FALSE, &(*(glm::mat3*)pointer)[0][0]);
		GLcheck(*this, "glUniformMatrix3fv");
		return;
	}
	case enums::EUniformType::_Matrix4fv:
	{
		glContext->UniformMatrix4fv(location, 1, GL_FALSE, &(*(glm::mat4*)pointer)[0][0]);
		GLcheck(*this, "glUniformMatrix4fv");
		return;
	}
	}
	assert(false && "Should not reach here");
}
const bool zg::GLcheck(GLRenderer &renderer, const char* fn, const bool egl)
{
	while (true)
	{
		uint32_t err = 0;
		if (!egl)
			err = renderer.glContext->GetError();
#if defined(_Android)
		else if (egl)
			err = eglGetError();
#endif
		if (err == GL_NO_ERROR
#if defined(_Android)
			|| err == EGL_SUCCESS
#endif
			)
		{
			/*|
			  |*/
			break;
		}
		switch (err)
		{
		case GL_INVALID_ENUM:
		{
			Logger::print(Logger::Error, "GL_INVALID_ENUM", "(", fn, "): ", "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
			break;
		};
		case GL_INVALID_VALUE:
		{
			Logger::print(Logger::Error, "GL_INVALID_VALUE", "(", fn, "): ", "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
			break;
		};
		case GL_INVALID_OPERATION:
		{
			Logger::print(Logger::Error, "GL_INVALID_OPERATION", "(", fn, "): ", "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
			break;
		};
		case GL_INVALID_FRAMEBUFFER_OPERATION:
		{
			Logger::print(Logger::Error, "GL_INVALID_FRAMEBUFFER_OPERATION", "(", fn, "): ", "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.");
			break;
		};
		case GL_OUT_OF_MEMORY:
		{
			Logger::print(Logger::Error, "GL_OUT_OF_MEMORY", "(", fn, "): ", "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
			break;
		};
		case GL_STACK_UNDERFLOW:
		{
			Logger::print(Logger::Error, "GL_STACK_UNDERFLOW", "(", fn, "): ", "An attempt has been made to perform an operation that would cause an internal stack to underflow.");
			break;
		};
		case GL_STACK_OVERFLOW:
		{
			Logger::print(Logger::Error, "GL_STACK_OVERFLOW", "(", fn, "): ", "An attempt has been made to perform an operation that would cause an internal stack to overflow.");
			break;
		};
#if defined(_Android)
		case EGL_NOT_INITIALIZED:
		{
			Logger::print(Logger::Error, "EGL_NOT_INITIALIZED", "(", fn, "): ", "EGL is not initialized, or could not be initialized, for the specified EGL display connection.");
			break;
		};
		case EGL_BAD_ACCESS:
		{
			Logger::print(Logger::Error, "EGL_BAD_ACCESS", "(", fn, "): ", "EGL cannot access a requested resource (for example a context is bound in another thread).");
			break;
		};
		case EGL_BAD_ALLOC:
		{
			Logger::print(Logger::Error, "EGL_BAD_ALLOC", "(", fn, "): ", "EGL failed to allocate resources for the requested operation.");
			break;
		};
		case EGL_BAD_ATTRIBUTE:
		{
			Logger::print(Logger::Error, "EGL_BAD_ATTRIBUTE", "(", fn, "): ", "An unrecognized attribute or attribute value was passed in the attribute list.");
			break;
		};
		case EGL_BAD_CONTEXT:
		{
			Logger::print(Logger::Error, "EGL_BAD_CONTEXT", "(", fn, "): ", "An EGLContext argument does not name a valid EGL rendering context.");
			break;
		};
		case EGL_BAD_CONFIG:
		{
			Logger::print(Logger::Error, "EGL_BAD_CONFIG", "(", fn, "): ", "An EGLConfig argument does not name a valid EGL frame buffer configuration.");
			break;
		};
		case EGL_BAD_CURRENT_SURFACE:
		{
			Logger::print(Logger::Error, "EGL_BAD_CURRENT_SURFACE", "(", fn, "): ", "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid.");
			break;
		};
		case EGL_BAD_DISPLAY:
		{
			Logger::print(Logger::Error, "EGL_BAD_DISPLAY", "(", fn, "): ", "An EGLDisplay argument does not name a valid EGL display connection.");
			break;
		};
		case EGL_BAD_SURFACE:
		{
			Logger::print(Logger::Error, "EGL_BAD_SURFACE", "(", fn, "): ", "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering.");
			break;
		};
		case EGL_BAD_MATCH:
		{
			Logger::print(Logger::Error, "EGL_BAD_MATCH", "(", fn, "): ", "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface).");
			break;
		};
		case EGL_BAD_PARAMETER:
		{
			Logger::print(Logger::Error, "EGL_BAD_PARAMETER", "(", fn, "): ", "One or more argument values are invalid.");
			break;
		};
		case EGL_BAD_NATIVE_PIXMAP:
		{
			Logger::print(Logger::Error, "EGL_BAD_NATIVE_PIXMAP", "(", fn, "): ", "A NativePixmapType argument does not refer to a valid native pixmap.");
			break;
		};
		case EGL_BAD_NATIVE_WINDOW:
		{
			Logger::print(Logger::Error, "EGL_BAD_NATIVE_WINDOW", "(", fn, "): ", "A NativeWindowType argument does not refer to a valid native window.");
			break;
		};
		case EGL_CONTEXT_LOST:
		{
			Logger::print(Logger::Error, "EGL_CONTEXT_LOST", "(", fn, "): ", "A power management event has occurred. The application must destroy all contexts and reinitialise OpenGL ES state and objects to continue rendering.");
			break;
		};
#endif
		}
	}
	return true;
}
#endif