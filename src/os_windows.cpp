#include "core.h"

#ifdef OS_WINDOWS

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <map>

//----------------------------------------------------------------------------
// Globals
HINSTANCE __instance;
std::map<HWND, EggWindow *> windows;

typedef struct Win32EggWindowInternalsStruct {
	HWND wnd;
	HDC dc;
	HGLRC rc;

	LARGE_INTEGER timerFrequency;
	LARGE_INTEGER lastTimerValue;
	double timerTickLength;
	double runtime;
} Internals;

//----------------------------------------------------------------------------
// Entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	__instance = hInst;

	return _main(std::string(lpCmdLine));
}

//----------------------------------------------------------------------------
// Message processing function
LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	EggWindow *win = windows[hwnd];

	if (!win) { return DefWindowProc(hwnd, msg, wparam, lparam); }

	switch (msg) {
		case WM_SIZE:
			win->setWidth(LOWORD(lparam));
			win->setHeight(HIWORD(lparam));
			break;
		case WM_CLOSE:
			if (windows.size() <= 1) core->stop();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
};

//----------------------------------------------------------------------------
// Exit function
void sysExit(int code) {
	exit(code);
}

//----------------------------------------------------------------------------
// EggWindow implementation
//  Constructor(w,h)
EggWindow::EggWindow(unsigned int _w, unsigned int _h):w(_w), h(_h) {
	Log::debug("Creating window with size %d x %d", w, h);

	internals = malloc(sizeof(Internals));
	Internals *i = (Internals *)internals;

	i->runtime = 0;

	QueryPerformanceFrequency(&i->timerFrequency);
	i->timerTickLength = 1.0 / i->timerFrequency.QuadPart;
	Log::debug("Timer Frequency: %dHz", i->timerFrequency.QuadPart);
	QueryPerformanceCounter(&i->lastTimerValue);
	Log::debug("Zeroed timer.");

	Log::debug("Creating window.");
	int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);

	WNDCLASS cls;
	cls.style = CS_OWNDC;
	cls.lpfnWndProc = windowProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.hInstance = __instance;
	cls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	cls.hCursor = LoadCursor(NULL, IDC_ARROW);
	cls.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	cls.lpszMenuName = NULL;
	cls.lpszClassName = "EGG_MainWindow";
	//hIconSm = 0;
	ATOM registeredClass = RegisterClass(&cls);

	if (!registeredClass) {
		Log::error("Failed registering window class.");
	}

	DWORD style = WS_CAPTION | WS_POPUP | WS_VISIBLE;

	i->wnd = CreateWindow("EGG_MainWindow", "", style, sw/2 - w/2, sh/2 - h/2, w, h, NULL, NULL, __instance, NULL);
	windows[i->wnd] = this;

	// I can't believe we have to do this to set the client area size
	RECT clientArea, windowArea;
	int ww, wh;
	GetClientRect(i->wnd, &clientArea);
	GetWindowRect(i->wnd, &windowArea);
	ww = w + ((windowArea.right - windowArea.left) - (clientArea.right - clientArea.left));
	wh = h + ((windowArea.bottom - windowArea.top) - (clientArea.bottom - clientArea.top));
	MoveWindow(i->wnd, sw/2 - ww/2, sh/2 - wh/2, ww, wh, false);
	// end resizing ridiculousness

	Log::debug("Get DC.");
	i->dc = GetDC(i->wnd);

	Log::debug("Get PFD.");
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iFormat = ChoosePixelFormat(i->dc, &pfd);
	SetPixelFormat(i->dc, iFormat, &pfd);

	Log::debug("Creating OpenGL context.");

	i->rc = wglCreateContext(i->dc);

	Log::debug("Window created.");
}

EggWindow::~EggWindow() {
	Internals *i = (Internals *)internals;

	Log::debug("Destroying a window.");

	windows.erase(i->wnd);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(i->rc);
	ReleaseDC(i->wnd, i->dc);
	DestroyWindow(i->wnd);
}

void EggWindow::process() {
	Internals *i = (Internals *)internals;

	// Process system messages
	MSG msg;

	while(PeekMessage(&msg, i->wnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// Do timer update
	LARGE_INTEGER t; 
	QueryPerformanceCounter(&t);
	long long d = t.QuadPart - i->lastTimerValue.QuadPart;
	i->runtime += d * i->timerTickLength;
	i->lastTimerValue = t;

	// Let OS do any multitasking that it needs to
	Sleep(0);
}

void EggWindow::prepareRender() {
	Internals *i = (Internals *)internals;
	wglMakeCurrent(i->dc, i->rc);
}

void EggWindow::teardownRender() {
	Internals *i = (Internals *)internals;
	wglMakeCurrent(NULL, NULL);

	// Swap video buffers
	SwapBuffers(i->dc);
}

double EggWindow::getTime() {
	Internals *i = (Internals *)internals;
	return i->runtime;
}

unsigned int EggWindow::getHeight() { return h; }
unsigned int EggWindow::getWidth() { return w; }
void EggWindow::setHeight(unsigned int _h) { h = _h; }
void EggWindow::setWidth(unsigned int _w) { w = _w; }

void EggWindow::setTitle(std::string title) {
	Internals *i = (Internals *)internals;
	SetWindowText(i->wnd, title.c_str());
}

#endif