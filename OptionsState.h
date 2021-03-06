#include "GameState.h"

class WindowHandler;
class CheckBox;
class Settings;
class Slidebar;

class OptionsState : public GameState
{
public:
	void init(Game* game);
	void cleanup();

	void pause();
	void resume();

	bool messageHandler(WindowID id, WindowMessage msg);
	void handleEvents(UINT msg, WPARAM wParam, LPARAM lParam);
	void update(double dt);
	void drawMain(void);
	void drawGui(void);

	static OptionsState* Instance() {
		return &mOptionsState;
	}
protected:
	OptionsState() {};
private:
	static OptionsState mOptionsState;

	WindowHandler *mWindowHandler;
	IDirect3DTexture9 *mBackgroundTexture;
	Settings *mSettings;

	/* windows */
	CheckBox *mMuteMusic;
	CheckBox *mMuteEffects;
	Slidebar *mSlidebar;
};

