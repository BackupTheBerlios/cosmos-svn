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

// The all-important entry point

#include <iostream>
#include <cstring>    // For argument parsing
#include "CosmosGame.h"

using namespace std;

// The execution summary enumeration (to tell the user a summary of what happened)
enum ExecutionSummary
{
	ES_GOOD = EXIT_SUCCESS,    // Set ES_GOOD to EXIT_SUCCESS, because we'll be returning the ExecutionSummary
	ES_INVALID_ARGUMENTS,
	ES_EXCEPTION_OGRE,
	ES_EXCEPTION_GAME,
	ES_EXCEPTION_UNKNOWN
};

// Function prototypes
void _printExecutionSummary(const ExecutionSummary status);	// Prints what happened over the course of
															// execution after the game has exited/aborted

ExecutionSummary _processArguments(const int argc, char *argv[]);

void _showUsage();

/*
 * First, a few notes:
 * All debug and error messages go to stderr
 * All standard messages go to stdout
 * All final messages (the execution summaries) go to stdout
 * All functions that are to be used only within its file/class (i.e. is private) are to be preceded
 *  with a _ (e.g. _square(int x) { return x*x; })
 * i'm sleepy
 * oh yeah, and don't do some stupid code like:
 	for (int i = 0; i < 10; i++)
	{
		switch (i)
		{
		case 0: // do something
		case 1: // ...
		// ...
		}
	}
	or:
	while (false)
	{
		std::cerr << "this is going to stderr because this certifies that your computer is messed up" << std::endl;
	}
	believe me. it will make you look like an idiot.
*/
int main(int argc, char *argv[])
{
	// Check the arguments
	if (_processArguments(argc, argv) != ES_GOOD)
		return ES_INVALID_ARGUMENTS;

	ExecutionSummary es = ES_GOOD;

	// The legal stuff
	cout << "***** Welcome to Cosmos! *****\n"
			"This program is free software; you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License as published by\n"
			"the Free Software Foundation; either version 2 of the License, or\n"
			"(at your option) any later version.\n\n"

			"This program is distributed in the hope that it will be useful,\n"
			"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
			"GNU General Public License for more details.\n\n"

			"You should have received a copy of the GNU General Public License\n"
			"along with this program; if not, write to the\n"
			"Free Software Foundation, Inc.,\n"
			"59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n"
		 << endl;

	// Now, start the game!
	cerr << "Starting game...\n\n"
			"~~~~~~~~~~~~~~~~\n"
		 << endl;

	CosmosGame *game = new CosmosGame();	// The game object

	try
	{
		game->go();	// Try to start the game
	}
	catch (Ogre::Exception &e)	// Catch any errors and abort
	{
		cerr << "Cannot initialize game: exception thrown!\n"
				"Details: " << e.getFullDescription()
			 << endl;
		es = ES_EXCEPTION_OGRE;
		goto end;
	}
	catch (...)    // Shouldn't happen, but who knows?...
	{
		cerr << "Cannot initialize game: exception thrown!\n"
				"Details: unknown"
			 << endl;
		es = ES_EXCEPTION_UNKNOWN;
		goto end;
	}

end:
	// Deallocate the game object
	delete game;

	// Tell the user a summary of what happened
	_printExecutionSummary(es);

	// Return the execution summary ID
	return es;
}

void _printExecutionSummary(const ExecutionSummary status)
{
	cout << "\n~~~~~~~~~~~~~~~~\n"
			"Execution summary: ";

	switch (status)
	{
		case ES_GOOD:
			cout << "Everything went fine."
				 << endl;
			break;

		case ES_EXCEPTION_OGRE:
			cout << "OGRE rendering engine failed to initialize.\n"
					"If you are sure this message is in error,\n"
					"please report it to us."
				 << endl;
			break;

		case ES_EXCEPTION_GAME:
			cout << "The game itself caused an error.\n"
					"If you are sure this message is in error,\n"
					"please report it to us."
				 << endl;
			break;

		case ES_EXCEPTION_UNKNOWN:
			cout << "An unknown error occurred.\n"
					"Please report this to us."
				 << endl;
			break;
	}
}

ExecutionSummary _processArguments(const int argc, char *argv[])
{
	// Test the arguments
	if (argc == 1)
	{
		return ES_GOOD;
	}

	for (int i = 1; i < argc; i++)
	{
		if ((argv[i][0] == '-') && (argv[i][1] == '-'))
		{
			if (!strcmp(argv[i], "--help"))
			{
				_showUsage();
			}
			else
			{
				cout << "Invalid arguments." << endl;
				_showUsage();
			}
		}
		else
		{
			cout << "Invalid arguments." << endl;
			_showUsage();
		}
	}

	return ES_INVALID_ARGUMENTS;
}

void _showUsage()
{
	cout << "asfd" << endl;
}
