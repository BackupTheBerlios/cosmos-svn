/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 FIXME: clean up everything (like stats), use CEGUI for overlays
 */

/*
-----------------------------------------------------------------------------
Filename:    CosmosGame_FrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
-----------------------------------------------------------------------------
*/

#ifndef __COSMOSGAME_FRAMELISTENER_H__
#define __COSMOSGAME_FRAMELISTENER_H__

#include "Ogre.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

using namespace Ogre;

class CosmosGame_FrameListener : public FrameListener, public KeyListener
{
public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	CosmosGame_FrameListener(RenderWindow *win, Camera *cam, bool useBufferedInputKeys /*= false*/, bool useBufferedInputMouse /*= false*/);

	~CosmosGame_FrameListener();

	bool processUnbufferedKeyInput(const FrameEvent &evt);

	bool processUnbufferedMouseInput(const FrameEvent &evt);

	void moveCamera();

	void showDebugOverlay(const bool show);

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const FrameEvent &evt);

	bool frameEnded(const FrameEvent &evt);

	void switchMouseMode();

	void switchKeyMode();

	void keyClicked(KeyEvent *e);

	void keyPressed(KeyEvent *e);

	void keyReleased(KeyEvent *e);

protected:
	int sceneDetailIndex;
	Real movementSpeed;
	Degree rotationSpeed;
	Overlay *debugOverlay;

	void updateStats();

	EventProcessor *eventProcessor;
	InputReader *inputDevice;
	Camera *camera;

	Vector3 translateVector;
	RenderWindow *renderWindow;
	bool statsOn, useBufferedInputKeys, useBufferedInputMouse, inputTypeSwitchingOn;
	unsigned int numScreenShots;
	float moveScale;
	Degree rotScale;
	Real timeUntilNextToggle;    // just to stop toggles flipping too fast
	Radian rotX, rotY;
	TextureFilterOptions texFilterOpts;
	int aniso;
};

#endif // __COSMOSGAME_FRAMELISTENER_H__
