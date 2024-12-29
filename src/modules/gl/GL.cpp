#include <anex/modules/gl/GL.hpp>
#include <stdexcept>
#include <iostream>
using namespace anex::modules::gl;

GLWindow::GLWindow(const char* title, const int& windowWidth, const int& windowHeight, const int& framerate):
	IWindow(windowWidth, windowHeight, framerate),
	title(title)
{
	run();
};

GLWindow::~GLWindow()
{
};
#ifdef _WIN32
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
typedef BOOL (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int interval);
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
static LRESULT CALLBACK gl_wndproc(HWND hwnd, UINT msg, WPARAM wParam,
																	LPARAM lParam)
{
	struct GLWindow* glWindow = (struct GLWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (msg)
	{
	case WM_CREATE:
		{
			CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
			glWindow = (GLWindow*)createStruct->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)glWindow);
			glWindow->hDeviceContext = GetDC(hwnd);
			glWindow->renderInit();
		};
	case WM_PAINT:
		{
			// PAINTSTRUCT ps;
			// HDC hdc = BeginPaint(hwnd, &ps);
			// HDC memdc = CreateCompatibleDC(hdc);
			// HBITMAP hbmp = CreateCompatibleBitmap(hdc, f->width, f->height);
			// HBITMAP oldbmp = static_cast<HBITMAP>(SelectObject(memdc, hbmp));
			// BINFO bi = {{sizeof(bi), f->width, -f->height, 1, 32, BI_BITFIELDS}};
			// bi.bmiColors[0].rgbRed = 0xff;
			// bi.bmiColors[1].rgbGreen = 0xff;
			// bi.bmiColors[2].rgbBlue = 0xff;
			// SetDIBitsToDevice(memdc, 0, 0, f->width, f->height, 0, 0, 0, f->height,
			// 									f->buf, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
			// BitBlt(hdc, 0, 0, f->width, f->height, memdc, 0, 0, SRCCOPY);
			// SelectObject(memdc, oldbmp);
			// DeleteObject(hbmp);
			// DeleteDC(memdc);
			// EndPaint(hwnd, &ps);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		// f->mouse = (msg == WM_LBUTTONDOWN);
		break;
	case WM_MOUSEMOVE:
		// f->y = HIWORD(lParam), f->x = LOWORD(lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			// f->mod = ((GetKeyState(VK_CONTROL) & 0x8000) >> 15) |
			// 				 ((GetKeyState(VK_SHIFT) & 0x8000) >> 14) |
			// 				 ((GetKeyState(VK_MENU) & 0x8000) >> 13) |
			// 				 (((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) >> 12);
			// f->keys[FENSTER_KEYCODES[HIWORD(lParam) & 0x1ff]] = !((lParam >> 31) & 1);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
#endif
void GLWindow::startWindow()
{
#ifdef _WIN32
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = gl_wndproc;
	wc.hInstance = hInstance;
	wc.lpszClassName = title;
	RegisterClassEx(&wc);
	RECT desiredRect = {0, 0, windowWidth, windowHeight};
	AdjustWindowRectEx(&desiredRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
	int adjustedWidth = desiredRect.right - desiredRect.left;
	int adjustedHeight = desiredRect.bottom - desiredRect.top;
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, title, title,
												WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
												adjustedWidth, adjustedHeight, NULL, NULL, hInstance, this);

	if (hwnd == NULL)
		return;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);
	MSG msg;
	while (true)
	{
		BOOL msgReceived = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msgReceived)
		{
			if (msg.message == WM_QUIT)
				goto _exit;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		runRunnables();
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		render();
		SwapBuffers(hDeviceContext);
	}
#endif
_exit:
	scene.reset();
#ifdef _WIN32
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRenderingContext);
#endif
};

#ifdef _WIN32
void SetupPixelFormat(HDC hDeviceContext)
{
	int pixelFormat;
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1 };
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pixelFormat = ChoosePixelFormat(hDeviceContext, &pfd);
	if (pixelFormat == 0)
	{
		throw std::runtime_error("pixelFormat is null!");
	}
	BOOL result = SetPixelFormat(hDeviceContext, pixelFormat, &pfd);
	if (!result)
	{
		throw std::runtime_error("failed to setPixelFormat!");
	}
};
#endif

void GLWindow::renderInit()
{
#ifdef _WIN32
	SetupPixelFormat(hDeviceContext);
	HGLRC hTempRC = wglCreateContext(hDeviceContext);
	wglMakeCurrent(hDeviceContext, hTempRC);
	gladLoadGL();
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglMakeCurrent(nullptr, nullptr);
	hRenderingContext = wglCreateContextAttribsARB(hDeviceContext, 0, 0);
	wglDeleteContext(hTempRC);
	wglMakeCurrent(hDeviceContext, hRenderingContext);
	gladLoadGL();
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearDepth(0.0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback([](GLuint source, GLuint type, GLuint id, GLuint severity, GLsizei length, const GLchar* message, const void* userParam) {
			std::cerr << "OpenGL Debug Message: " << message << std::endl;
	}, nullptr);
#endif
};

void GLWindow::updateKeys()
{
	for (unsigned int i = 0; i < 256; ++i)
	{
		int pressed = false;
		//		int& pressed = f->keys[i];
		if (keys[i] != pressed)
		{
			callKeyPressHandler(i, pressed);
		}
		if (pressed)
		{
			callKeyUpdateHandler(i);
		}
	}
};

void GLWindow::close()
{
#ifdef _WIN32
  PostMessage(hwnd, WM_CLOSE, 0, 0);
#endif
};

void GLWindow::drawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
};

void GLWindow::drawRectangle(int x, int y, int w, int h, uint32_t color)
{
};

void GLWindow::drawCircle(int x, int y, int radius, uint32_t color)
{
};

void GLWindow::drawText(int x, int y, const char* text, int scale, uint32_t color)
{
};
