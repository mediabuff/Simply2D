#ifndef WINDOWHANDLER_H
#define WIDNOWHANDLER_H

#include "Window.h"
#include <vector>

using namespace std;
class Window;

class WindowHandler
{
public:
	WindowHandler();											// for a window that covers the whole screen
	WindowHandler(int x, int y, int width, int height);			// for a window that is local
	~WindowHandler();

	void update(double dt);										// updates the active window, checks for mouse presses/hoovers
	void draw(void);											// draw all windows in the list

	void addWindow(Window* window);								// adds window to list
	void removeWindow(Window* window);							// deletes window with same ID

	RECT getRect(void);											// returns the rect, used to set windows positions

private:
	vector<Window*> mWindowList;
	Window* mActiveWindow;

	int mX;
	int mY;
	int mWidth;
	int mHeight;
};

#endif