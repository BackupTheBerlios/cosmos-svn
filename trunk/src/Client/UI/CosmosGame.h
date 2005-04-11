/***************************************************************************\
 *   CosmosGame.h - the class encapsulating the whole game                 *
 *                                                                         *
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

#ifndef __COSMOSGAME_H__
#define __COSMOSGAME_H__

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "CosmosGame_FrameListener.h"

using namespace Ogre;	// Import the Ogre namespace so we can render stuff

//namespace Cosmos::Client::UI
//{
class CosmosGame
{
public:
	CosmosGame();
	~CosmosGame();

	void go(); // Start the game

protected:
	Root *root;
	Camera *camera;
	SceneManager *sceneManager;
	CosmosGame_FrameListener *frameListener;
	RenderWindow *renderWindow;

	// These internal methods package up the stages in the startup process
	// Sets up the application - returns false if the user chooses to abandon configuration.
	bool setup();

	// Configures the application - returns false if the user chooses to abandon configuration.
	bool configure();

	void chooseSceneManager();

	void createCamera();

	void createFrameListener();

	void createScene();

	void destroyScene();

	void createViewports();

	// Method which will define the source of resources (other than current folder)
	void setupResources();

	// Optional override method where you can create resource listeners (e.g. for loading screens)
	void createResourceListener();

	// Optional override method where you can perform resource group loading
	// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	void loadResources();
};
//};
#endif // __COSMOSGAME_H__
