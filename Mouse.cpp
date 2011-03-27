#include "Mouse.h"
#include "Graphics.h"
#include "Camera.h"

extern Camera *gGameCamera;

Mouse *gMouse			 = 0;

Mouse::Mouse(HWND hwnd)
{
	mMainWnd = hwnd;

	// start position for the mouse
	GetCursorPos(&mPos);

	RECT windowRect;	
	GetWindowRect(mMainWnd, &windowRect);

	mPos.x -= windowRect.left + 8;
	mPos.y -= windowRect.top + 30;
}

Mouse::~Mouse()
{
	//dtor
}
void Mouse::updateMouseWIN(void)
{
	static POINT tmpPos;
	static int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	static int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT windowRect;	
	
	GetCursorPos(&tmpPos);
	GetWindowRect(mMainWnd, &windowRect);
	
	// s� det inte blir negativa v�rden
	if(tmpPos.x <= windowRect.left)
		mPos.x = windowRect.left;
	//else if(tmpPos.x >= windowRect.right) 
	//	mPos.x = windowRect.right;
	else if(tmpPos.y <= windowRect.top)
		mPos.y = windowRect.top;
	else if(tmpPos.y >= windowRect.bottom)
		mPos.y = windowRect.bottom;
	else	{
		// jazzy - f�r att f� bra v�rden!
		mPos.x = tmpPos.x - windowRect.left;
		mPos.y = tmpPos.y - windowRect.top;

		mPos.x-=8;
		mPos.y-=30;		
	}	
}

void Mouse::updateMouseDX(void)
{
	POINT tmpPos;
	RECT windowRect;

	// right now the position can be negative, if the players moves outside the screen
	mPos.x += gDInput->mouseDX();
	mPos.y += gDInput->mouseDY();

}

bool Mouse::buttonDown(MouseButton button)
{
	if(gDInput->mouseButtonDown(button))
		return true;
	// l�gg till mer vid behov
}

void Mouse::drawMousePos(void)
{
	char buffer[256];
	sprintf(buffer, "Mouse x: %i\nMouse y: %i", (int)mPos.x, (int)mPos.y);
	gGraphics->drawText(buffer, GAME_WIDTH + 10	, 740); 
}

void Mouse::setMousePos(int x, int y)
{
	RECT windowRect;	
	GetWindowRect(mMainWnd, &windowRect);

	mPos.x = x;
	mPos.y = y;

	SetCursorPos(windowRect.left - 8 + x - gGameCamera->getOffset(), windowRect.top + 30 + y);	
}

void Mouse::setX(int x)
{
	RECT windowRect;	
	GetWindowRect(mMainWnd, &windowRect);

	SetCursorPos(windowRect.left + 8 + x, windowRect.top  + mPos.y);
	mPos.x = x + gGameCamera->getOffset();
}

void Mouse::setY(int y)
{

}

void Mouse::move(int dx, int dy)
{
	RECT windowRect;	
	GetWindowRect(mMainWnd, &windowRect);

	mPos.x += dx;
	mPos.y += dy;

	SetCursorPos(windowRect.left + 8 + mPos.x - gGameCamera->getOffset(), windowRect.top + 30 + mPos.y);
}

POINT Mouse::getScreenPos(void)
{
	POINT tmpPos;
	POINT returnPos;
	RECT windowRect;

	GetCursorPos(&tmpPos);
	GetWindowRect(mMainWnd, &windowRect);

	returnPos.x = tmpPos.x - windowRect.left - 8;
	returnPos.y = tmpPos.y - windowRect.top - 30;

	return returnPos;
}

void Mouse::restore(void)
{
	mPos = getScreenPos();
}
/* DIRECT INPUT STYLISHH
static int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	static int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT windowRect;
	tx += gDInput->mouseDX();
	ty += gDInput->mouseDY();

	GetCursorPos(&mouseStartPos);

	GetWindowRect(mhMainWnd, &windowRect);

	sprintf(rect_buffer, "RECT\ntop: %i\nleft: %i\nbottom: %i\nright: %i\nWIDTH: %i\nHEIGHT: %i\n", windowRect.top,
	windowRect.left, windowRect.bottom, windowRect.right, screenWidth, screenHeight);	

	// A LA DX SPECIAL, annars �kar de fast man �r vid kanten >_>
	if(tx <= 0)
		tx = 0;
	if(tx >= 2959)
		tx = 2959;
	if(ty <= 0)
		ty = 0;
	if(ty >= 1049)
		ty = 1049;
	
	// s� det inte blir negativa v�rden
	if(tx <= windowRect.left)
		mx = windowRect.left;
	else if(tx >= windowRect.right) 
		mx = windowRect.right;
	else if(ty <= windowRect.top)
		my = windowRect.top;
	else if(ty >= windowRect.bottom)
		my = windowRect.bottom;
	else	{
		// jazzy
		mx = tx - windowRect.left;
		my = ty - windowRect.top;

		mx-=8;
		my-=30;		
	}	*/
