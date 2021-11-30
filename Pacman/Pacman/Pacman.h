#pragma once
#define MUNCHIECOUNT 200
#define GHOSTCOUNT 10

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

struct Player
{
	// Data to represent Pacman
	bool		_Dead;
	Vector2*	_Position;
	Rect*		_sourceRect;
	Texture2D*	_Texture;
	int			_Score;
	int			_movementState;
	int			_currentFrameTime;
	int			_Frame;
	float		_speedMultiplier;
	bool		_pKeyDown;
};

struct Enemy
{
	// Data to represent Munchie
	int			_frameCount;
	Rect*		_Rect;
	Vector2*	_Position;
	Texture2D*	_Texture;
	int			_Frame;
	int			_currentFrameTime;
};

struct MovingEnemy {
	// Data to represent Ghosts
	Vector2*	_Position;
	Texture2D*	_Texture;
	Rect*		_sourceRect;
	int			_Direction;
	float		_Speed;
};

class Pacman : public Game
{
private:

	Player* _pacman;
	Enemy* _munchie[MUNCHIECOUNT];
	Enemy* _cherry;
	MovingEnemy* _ghost[GHOSTCOUNT];

	SoundEffect* _pop;

	// Input Methods
	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);

	// Check Methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();

	// Update Methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy*, int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void UpdateGhost(MovingEnemy*, int elapsedTime);

	// Pacman data
	const float _cSpeed;
	const int	_cPacmanFrameTime;
	const int	_cMunchieFrameTime;
	const int	_cCherryFrameTime;

	// Position for String
	Vector2*	_stringPosition;

	// Data for Menu
	Texture2D*	_menuBackground; 
	Rect*		_menuRectangle; 
	Vector2*	_menuStringPosition; 
	bool		gameStarted;
	bool		_paused;

	bool		_selfCollision;

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

	/// <summary> Takes collision with munchies </summary>
	bool virtual Collision(Vector2* Actor, Rect* ActorRect, Vector2* Target, Rect* TargetRect);
};