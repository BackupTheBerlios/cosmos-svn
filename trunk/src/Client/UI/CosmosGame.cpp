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

#include "CosmosGame.h"

CosmosGame::CosmosGame()
{
	frameListener = 0;
	root = 0;
}

CosmosGame::~CosmosGame()
{
	if (frameListener)
		delete frameListener;
	if (root)
		delete root;
}

void CosmosGame::go()
{
	if (!setup())
		return;

	root->startRendering();

	destroyScene(); // Clean up
}

bool CosmosGame::setup()
{
	root = new Root();

	setupResources();

	if (!configure())
		return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	createResourceListener();

	// Load resources
	loadResources();

	// Create the scene
	createScene();

	// Initialize the frame listener to respond to events (mouse, keyboard, etc.)
	createFrameListener();

	return true;
}

bool CosmosGame::configure()
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if (root->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		renderWindow = root->initialise(true);
		return true;
	}
	return false;
}

void CosmosGame::chooseSceneManager()
{
	// Get the SceneManager, in this case a generic one
	sceneManager = root->getSceneManager(ST_GENERIC);
}

void CosmosGame::createCamera()
{
	// Create the camera
	camera = sceneManager->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	camera->setPosition(Vector3(0, 0, 500));

	// Look back along -Z
	camera->lookAt(Vector3(0, 0, -300));
	camera->setNearClipDistance(5);
}

void CosmosGame::createFrameListener()
{
	frameListener = new CosmosGame_FrameListener(renderWindow, camera, false, false);
	frameListener->showDebugOverlay(true);
	root->addFrameListener(frameListener);
}

void CosmosGame::createScene()
{
}

void CosmosGame::destroyScene()
{
}

void CosmosGame::createViewports()
{
	// Create one viewport, entire window
	Viewport *vp = renderWindow->addViewport(camera);
	vp->setBackgroundColour(ColourValue(0, 0, 0));

	// Alter the camera aspect ratio to match the viewport
	camera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

void CosmosGame::setupResources()
{
	// Load resource paths from config file
	ConfigFile cf;
	cf.load("resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); i++)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

void CosmosGame::createResourceListener()
{
}

void CosmosGame::loadResources()
{
	// Initialize, parse scripts, etc.
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
