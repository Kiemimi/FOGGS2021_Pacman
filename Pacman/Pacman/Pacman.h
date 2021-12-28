#pragma once
#define MUNCHIECOUNT 500
#define GHOSTCOUNT 1
#define WALLCOUNT 501

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
#include <iostream>
#include <string>

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

enum gameStates
{
	MainMenu = 0,
	Active,
	Dead,
	Paused,
	Settings,
};

struct Player
{
	// Data to represent Pacman
	Vector2*	_Position;
	Vector2*	_centerPosition;
	Rect*		_sourceRect;
	int			_gridPosX;
	int			_gridPosY;

	Texture2D*	_Texture;
	int			_Score;
	int			_movementState;
	int			_currentFrameTime;
	int			_Frame;
	int			_soundState;
	float		_speedMultiplier;
	bool		_pKeyDown;
};

struct Enemy
{
	// Data to represent Munchie
	Rect*		_Rect;
	Vector2*	_Position;
	Texture2D*	_Texture;
	int			_frameCount;
	int			_Frame;
	int			_currentFrameTime;
};

struct MovingEnemy {
	// Data to represent Ghosts
	Vector2*	_Position;
	Vector2*	_centerPosition;
	Rect*		_sourceRect;
	int			_Direction;

	Texture2D*	_Texture;
	float		_Speed;
};

struct ColliderObject {
	// Data to represent walls and the like
	Vector2*	_Position;
	Texture2D*	_Texture;
	Rect*		_SourceRect;
	int			wall;
};

class Pacman : public Game
{
private:

	// Entities In game
	Player*			_pacman;
	Enemy*			_munchie[MUNCHIECOUNT];
	Enemy*			_cherry;
	MovingEnemy*	_ghost[GHOSTCOUNT];
	ColliderObject* _wall[WALLCOUNT];

	// Variables counting the amount in-game
	int				wall;
	int				munchie;
	int				ghost;

	// Assets
	SoundEffect*	_eat1;
	SoundEffect*	_eat2;

	// Tile System Variable
	void		UpdateGhost(MovingEnemy*, int elapsedTime);
	int			masterGrid[24][32];
	int			gridX;
	int			gridY;
	
	// Input Methods
	void		Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	// Update Methods
	void		UpdateMunchie(Enemy*, int elapsedTime);
	void		UpdateCherry(int elapsedTime);
	float		distanceToPacman(float input, float input2);
	
	// Pacman data
	void		CheckViewportCollision();
	void		UpdatePacman(int elapsedTime);
	const float _cSpeed;
	const int	_cPacmanFrameTime;
	const int	_cMunchieFrameTime;
	const int	_cCherryFrameTime;

	// Data for Menu
	void		CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	Vector2*	_stringPosition;
	Texture2D*	_menuBackground; 
	Rect*		_menuRectangle; 
	Vector2*	_menuStringPosition; 

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

	/// <summary> Handles the baggage of loading a level from file. </summary>
	void virtual LoadLevel(string levelName);

	/// <summary> Takes collision with munchies. </summary>
	bool virtual Collision(Vector2* Actor, Rect* ActorRect, Vector2* Target, Rect* TargetRect);
	bool virtual Collision(float actorX, float actorY, Vector2* Target, Rect* TargetRect);

	/// <summary> Checks valid AI pathfinding. </summary>
	void virtual gridCheck(int levelArray[24][32], MovingEnemy*, int Offset, int elapsedTime);
};