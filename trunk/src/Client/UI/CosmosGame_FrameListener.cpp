/***************************************************************************\
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
\***************************************************************************/

#include "CosmosGame_FrameListener.h"

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
FIXME: clean up everything (like stats), use CEGUI for overlays
*/

void CosmosGame_FrameListener::updateStats()
{
	static String currFps = "Current FPS: ",
				  avgFps = "Average FPS: ",
				  bestFps = "Best FPS: ",
				  worstFps = "Worst FPS: ",
				  tris = "Triangle Count: ";

	// Update stats when necessary
	try
	{
		OverlayElement *guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps"),
					   *guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps"),
					   *guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps"),
					   *guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats &stats = renderWindow->getStatistics();

		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS) + " "
				+ StringConverter::toString(stats.bestFrameTime) + " ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS) + " "
				+ StringConverter::toString(stats.worstFrameTime) + " ms");

		OverlayElement *guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement *guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(renderWindow->getDebugText());
	}
	catch(...) { }	// Ignore all other exceptions
}

// Constructor takes a RenderWindow because it uses that to determine input context
CosmosGame_FrameListener::CosmosGame_FrameListener(RenderWindow *rw, Camera *cam, const bool useBufferedInputKeys /*= false*/, const bool useBufferedInputMouse /*= false*/)
{
	debugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	this->useBufferedInputKeys = useBufferedInputKeys;
	this->useBufferedInputMouse = useBufferedInputMouse;
	inputTypeSwitchingOn = useBufferedInputKeys || useBufferedInputMouse;
	rotationSpeed = 36;
	movementSpeed = 100;

	if (inputTypeSwitchingOn)
	{
		eventProcessor = new EventProcessor();
		eventProcessor->initialise(rw);
		eventProcessor->startProcessingEvents();
		eventProcessor->addKeyListener(this);
		inputDevice = eventProcessor->getInputReader();
	}
	else
	{
		inputDevice = PlatformManager::getSingleton().createInputReader();
		inputDevice->initialise(rw, true, true);
	}

	camera = cam;
	renderWindow = rw;
	statsOn = true;
	numScreenShots = 0;
	timeUntilNextToggle = 0;
	sceneDetailIndex = 0;
	moveScale = 0.0f;
	rotScale = 0.0f;
	translateVector = Vector3::ZERO;
	aniso = 1;
	texFilterOpts = TFO_BILINEAR;

	showDebugOverlay(false);
}

CosmosGame_FrameListener::~CosmosGame_FrameListener()
{
	if (inputTypeSwitchingOn)
		delete eventProcessor;
	else
		PlatformManager::getSingleton().destroyInputReader(inputDevice);
}

bool CosmosGame_FrameListener::processUnbufferedKeyInput(const FrameEvent &evt)
{
	if (inputDevice->isKeyDown(KC_A))
	{
		// Move camera left
		translateVector.x = -moveScale;
	}

	if (inputDevice->isKeyDown(KC_D))
	{
		// Move camera right
		translateVector.x = moveScale;
	}

	// Move camera forward by keypress.
	if (inputDevice->isKeyDown(KC_UP) || inputDevice->isKeyDown(KC_W) )
	{
		translateVector.z = -moveScale;
	}

	// Move camera backward by keypress.
	if (inputDevice->isKeyDown(KC_DOWN) || inputDevice->isKeyDown(KC_S))
	{
		translateVector.z = moveScale;
	}

	if (inputDevice->isKeyDown(KC_PGUP))
	{
		// Move camera up
		translateVector.y = moveScale;
	}

	if (inputDevice->isKeyDown(KC_PGDOWN))
	{
		// Move camera down
		translateVector.y = -moveScale;
	}

	if (inputDevice->isKeyDown(KC_RIGHT))
	{
		camera->yaw(-rotScale);
	}

	if (inputDevice->isKeyDown(KC_LEFT))
	{
		camera->yaw(rotScale);
	}

	if(inputDevice->isKeyDown(KC_ESCAPE))
	{
		return false;
	}

	// see if switching is on, and you want to toggle
	if (inputTypeSwitchingOn && inputDevice->isKeyDown(KC_M) && timeUntilNextToggle <= 0)
	{
		switchMouseMode();
		timeUntilNextToggle = 1;
	}

	if (inputTypeSwitchingOn && inputDevice->isKeyDown(KC_K) && timeUntilNextToggle <= 0)
	{
		// must be going from immediate keyboard to buffered keyboard
		switchKeyMode();
		timeUntilNextToggle = 1;
	}
	if (inputDevice->isKeyDown(KC_F) && timeUntilNextToggle <= 0)
	{
		statsOn = !statsOn;
		showDebugOverlay(statsOn);

		timeUntilNextToggle = 1;
	}
	if (inputDevice->isKeyDown(KC_T) && timeUntilNextToggle <= 0)
	{
		switch(texFilterOpts)
		{
			case TFO_BILINEAR:
				texFilterOpts = TFO_TRILINEAR;
				aniso = 1;
				break;

			case TFO_TRILINEAR:
				texFilterOpts = TFO_ANISOTROPIC;
				aniso = 8;
				break;

			case TFO_ANISOTROPIC:
				texFilterOpts = TFO_BILINEAR;
				aniso = 1;
				break;

			default:
				break;
		}

		MaterialManager::getSingleton().setDefaultTextureFiltering(texFilterOpts);
		MaterialManager::getSingleton().setDefaultAnisotropy(aniso);

		showDebugOverlay(statsOn);

		timeUntilNextToggle = 1;
	}

	if (inputDevice->isKeyDown(KC_SYSRQ) && (timeUntilNextToggle <= 0))
	{
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", ++numScreenShots);
		renderWindow->writeContentsToFile(tmp);
		timeUntilNextToggle = 0.5;
		renderWindow->setDebugText(String("Wrote ") + tmp);
	}

	if (inputDevice->isKeyDown(KC_R) && (timeUntilNextToggle <= 0))
	{
		sceneDetailIndex = (sceneDetailIndex + 1 ) % 3;
		switch (sceneDetailIndex)
		{
			case 0:
				camera->setDetailLevel(SDL_SOLID);
				break;

			case 1:
				camera->setDetailLevel(SDL_WIREFRAME);
				break;

			case 2:
				camera->setDetailLevel(SDL_POINTS);
				break;

		}
		timeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if (inputDevice->isKeyDown(KC_P) && (timeUntilNextToggle <= 0))
	{
		displayCameraDetails = !displayCameraDetails;
		timeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			renderWindow->setDebugText("");
	}
	if (displayCameraDetails)
	{
		// Print camera details
		renderWindow->setDebugText("P: " + StringConverter::toString(camera->getDerivedPosition()) + " " +
				"O: " + StringConverter::toString(camera->getDerivedOrientation()));
	}

	// Return true to continue rendering
	return true;
}

bool CosmosGame_FrameListener::processUnbufferedMouseInput(const FrameEvent &evt)
{
	// Rotation factors, may not be used if the second mouse button is pressed.

	// If the second mouse button is pressed, then the mouse movement results in sliding the camera, otherwise we rotate.
	if(inputDevice->getMouseButton(1))
	{
		translateVector.x += inputDevice->getMouseRelativeX() * 0.13;
		translateVector.y -= inputDevice->getMouseRelativeY() * 0.13;
	}
	else
	{
		rotX = Degree(-inputDevice->getMouseRelativeX() * 0.13);
		rotY = Degree(-inputDevice->getMouseRelativeY() * 0.13);
	}

	return true;
}

void CosmosGame_FrameListener::moveCamera()
{
	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
	camera->yaw(rotX);
	camera->pitch(rotY);
	camera->moveRelative(translateVector);
}

void CosmosGame_FrameListener::showDebugOverlay(const bool show)
{
	if (debugOverlay)
	{
		if (show)
			debugOverlay->show();
		else
			debugOverlay->hide();
	}
}

// Override frameStarted event to process that (don't care about frameEnded)
bool CosmosGame_FrameListener::frameStarted(const FrameEvent &evt)
{
	if(renderWindow->isClosed())
		return false;

	if (!inputTypeSwitchingOn)
		inputDevice->capture();

	if (!useBufferedInputMouse || !useBufferedInputKeys)
	{
		// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
		if (timeUntilNextToggle >= 0)
			timeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			moveScale = 1;
			rotScale = 0.1;
		}
		// Otherwise scale movement units by time passed since last frame
		else
		{
		// Move about 100 units per second,
			moveScale = movementSpeed * evt.timeSinceLastFrame;
		// Take about 10 seconds for full rotation
			rotScale = rotationSpeed * evt.timeSinceLastFrame;
		}
		rotX = rotY = 0;
		translateVector = Vector3::ZERO;
	}

	if (useBufferedInputKeys)
	{
		// no need to do any processing here, it is handled by event processor and
		// you get the results as KeyEvents
	}
	else if (!processUnbufferedKeyInput(evt))
	{
		return false;
	}
	if (useBufferedInputMouse)
	{
		// no need to do any processing here, it is handled by event processor and
		// you get the results as MouseEvents
	}
	else if (!processUnbufferedMouseInput(evt))
	{
		return false;
	}

	if (!useBufferedInputMouse || !useBufferedInputKeys)
	{
		// one of the input modes is immediate, so update the movement vector
		moveCamera();
	}

	return true;
}

bool CosmosGame_FrameListener::frameEnded(const FrameEvent &evt)
{
	updateStats();

	return true;
}

void CosmosGame_FrameListener::switchMouseMode()
{
	useBufferedInputMouse = !useBufferedInputMouse;
	inputDevice->setBufferedInput(useBufferedInputKeys, useBufferedInputMouse);
}
void CosmosGame_FrameListener::switchKeyMode()
{
	useBufferedInputKeys = !useBufferedInputKeys;
	inputDevice->setBufferedInput(useBufferedInputKeys, useBufferedInputMouse);
}

void CosmosGame_FrameListener::keyClicked(KeyEvent &e)
{
	const char input = e.getKeyChar();
	if (input == 'm')
		switchMouseMode();
	else if (input == 'k')
		switchKeyMode();
}

void CosmosGame_FrameListener::keyPressed(KeyEvent &e)
{
}

void CosmosGame_FrameListener::keyReleased(KeyEvent &e)
{
}
