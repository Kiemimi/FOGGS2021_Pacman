#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	// Input Methods
	void Input(int elapsedTime, Input::KeyboardState* state);

	// Check Methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();

	// Update Methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(int elapsedTime);
	void UpdateCherry(int elapsedTime);

	// Data to represent Pacman
	Vector2*	_pacmanPosition;
	Rect*		_pacmanSourceRect;
	Texture2D*	_pacmanTexture;
	const float _cPacmanSpeed;
	int			Score;

	// Data to represent Munchie
	int			_munchieFrameCount;
	Rect*		_munchieRect;
	Vector2*	_munchiePosition;
	Texture2D*	_munchieBlueTexture;
	const int	_cMunchieFrameTime;
	int			_munchieFrame;
	int			_munchieCurrentFrameTime;

	// Data to represent Cherry
	int			_cherryFrameCount;
	Rect*		_cherryRect;
	Vector2*	_cherryPosition;
	Texture2D*	_cherryTexture;
	const int	_ccherryFrameTime;
	int			_cherryFrame;
	int			_cherryCurrentFrameTime;

	// Position for String
	Vector2*	_stringPosition;

	// Enum for types of movement
	int			movementState;
	int			_pacmanCurrentFrameTime;
	int			_cPacmanFrameTime;
	int			_pacmanFrame;

	// Data for Menu
	Texture2D*	_menuBackground; 
	Rect*		_menuRectangle; 
	Vector2*	_menuStringPosition; 
	bool		_paused;
	bool		_pKeyDown;
	bool		gameStarted;
	
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

	bool virtual Collision(Vector2* Actor, Rect* ActorRect);
};