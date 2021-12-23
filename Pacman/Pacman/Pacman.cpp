#include "Pacman.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool once = true;
gameStates gameState = MainMenu;

Pacman::Pacman(int argc, char* argv[])
	: Game(argc, argv), _pacman(), _munchie(), _cSpeed(0.25f), _cPacmanFrameTime(250), _cMunchieFrameTime(25000), _cCherryFrameTime(500)
{
	_pacman  = new Player();
	_selfCollision = false;

	for (int i = 0; i < GHOSTCOUNT; i++) {
		_ghost[i] = new MovingEnemy();
		_ghost[i]->_Direction = 0;
		_ghost[i]->_Speed = 0.2f;
	}

	_cherry	 = new Enemy();
	
	_pacman->_currentFrameTime = 0;
	_pacman->_Frame = 0;
	_pacman->_movementState = 5;
	_pacman->_Score = 0;
	_pacman->_pKeyDown = false;

	_pop = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->_Texture;
	delete _pacman->_sourceRect;
	delete _pacman->_Position;
	delete _pacman;

	for (int i = 0; i < munchie; i++) {
		delete _munchie[i]->_Texture;
		delete _munchie[i]->_Rect;
		delete _munchie[i];
	}
	
	delete[] _munchie;
	delete _cherry->_Texture;
	delete _cherry->_Rect;
	delete _cherry;
	delete _pop;
}

void Pacman::LoadLevel(string levelName)
{
	ifstream ifs(levelName, ios::binary | ios::in);
	string line;
	wall = 0;	
	munchie = 0;

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);

	Texture2D* wallTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);

	while (!ifs.eof()) {
		while (getline(ifs, line))
		{
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == 'x') {
					_wall[wall] = new ColliderObject();
					_wall[wall]->_Texture = wallTex;
					_wall[wall]->_Position = new Vector2(i * 32, int(ifs.tellg()) - 32);
					_wall[wall]->_SourceRect = new Rect(_wall[wall]->_Position->X, _wall[wall]->_Position->Y, 32, 34);
					wall++;
				}
				else if (line[i] == 'o') {
					_munchie[munchie] = new Enemy();
					_munchie[munchie]->_currentFrameTime = 0;
					_munchie[munchie]->_Frame = 0;
					_munchie[munchie]->_Texture = munchieTex;
					_munchie[munchie]->_Position = new Vector2(((i * 32) + 10), ((int(ifs.tellg()) - 32)) + 10);
					_munchie[munchie]->_Rect = new Rect(_munchie[munchie]->_Position->X, _munchie[munchie]->_Position->Y, 12, 12);
					munchie++;
				}
			}
		}
	}
}

void Pacman::LoadContent()
{
	if (once) {
		LoadLevel("levels/1.txt");
		once = false;
	}

	// Load Pacman
	_pacman->_Texture = new Texture2D();
	_pacman->_Texture->Load("Textures/Pacman.png", false);
	_pacman->_Position = new Vector2(350.0f, 350.0f);
	_pacman->_sourceRect = new Rect(350.0f, 350.0f, 32, 32);

	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/GhostBlue.png", true);

	_pop->Load("Sounds/pop.wav");
	
	// Load Cherry
	_cherry->_Texture = new Texture2D();
	_cherry->_Texture->Load("Textures/Cherry.png", true);
	_cherry->_Rect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->_Position = new Vector2(500.0f, 250.0f);

	for (int i = 0; i < GHOSTCOUNT; i++) {
		_ghost[i]->_Texture = ghostTex;
		_ghost[i]->_Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghost[i]->_sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	// Load menu
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2, Graphics::GetViewportHeight() / 2);

	// Set string position
	_stringPosition = new Vector2(Graphics::GetViewportWidth() / 2 - 50, 50.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Initialises keyboard input
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	// Creates the condition for exiting the start menu
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && gameState == MainMenu) {
		gameState = Active;
	}	
	else
	{
		if (gameState != Paused)
		{
			Input(elapsedTime, keyboardState, mouseState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime);

			for (int i = 0; i < GHOSTCOUNT; i++) {
				UpdateGhost(_ghost[i], elapsedTime);
			}

			for (int i = 0; i < munchie; i++) {
				UpdateMunchie(_munchie[i], elapsedTime);
			}
			UpdateCherry(elapsedTime);
		}
		CheckPaused(keyboardState, Input::Keys::P);
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score: " << _pacman->_Score;

	SpriteBatch::BeginDraw(); // Starts Drawing

	SpriteBatch::Draw(_pacman->_Texture, _pacman->_Position, _pacman->_sourceRect); // Draws Pacman

	for (int i = 0; i < munchie; i++) {
		SpriteBatch::Draw(_munchie[i]->_Texture, _munchie[i]->_Position, _munchie[i]->_Rect);
		_munchie[i]->_Rect->X = _munchie[i]->_Rect->Width * _munchie[i]->_frameCount;
	}
	
	for (int i = 0; i < GHOSTCOUNT; i++) {
		SpriteBatch::Draw(_ghost[i]->_Texture, _ghost[i]->_Position, _ghost[i]->_sourceRect);
	}

	for (int i = 0; i < wall; i++)
		SpriteBatch::Draw(_wall[i]->_Texture, _wall[i]->_Position, _wall[i]->_SourceRect);

	_pacman->_sourceRect->X = _pacman->_sourceRect->Width * _pacman->_Frame;

	SpriteBatch::Draw(_cherry->_Texture, _cherry->_Position, _cherry->_Rect);
	_cherry->_Rect->X = _cherry->_Rect->Width * _cherry->_frameCount;
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::White);
	
	if (gameState == Paused)
	{
		std::stringstream menuStream; 
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (gameState == MainMenu)
	{
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO START!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Green);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

bool Pacman::Collision(Vector2* Actor, Rect* ActorRect, Vector2* Target, Rect* TargetRect) {
	if (	Actor->X + ActorRect->Width > Target->X
		&&	Actor->Y + ActorRect->Height > Target->Y
		&&	Actor->X < Target->X + TargetRect->Width
		&&	Actor->Y < Target->Y + TargetRect->Height)
	{
		return true;
	}
	else return false;
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState) {
	float pacmanSpeed = _cSpeed * elapsedTime * _pacman->_speedMultiplier;

	// Controls the player's movement by taking an "enum" for movement states
	if (gameState == Active) {
#pragma region KeyboardInputHandler
		if (state->IsKeyDown(Input::Keys::D))
			_pacman->_movementState = 1;
		if (state->IsKeyDown(Input::Keys::A))
			_pacman->_movementState = 2;
		if (state->IsKeyDown(Input::Keys::W))
			_pacman->_movementState = 3;
		if (state->IsKeyDown(Input::Keys::S))
			_pacman->_movementState = 4;

		state->IsKeyDown(Input::Keys::LEFTSHIFT) ? _pacman->_speedMultiplier = 1.5f : _pacman->_speedMultiplier = 1.0f;
#pragma endregion
#pragma region MouseInputHandler
		if (mouseState->LeftButton == Input::ButtonState::PRESSED)
		{
			_cherry->_Position->X = mouseState->X;
			_cherry->_Position->Y = mouseState->Y;
		}
#pragma endregion
		switch (_pacman->_movementState) {
		case 1:
			_pacman->_Position->X += pacmanSpeed;
			_pacman->_sourceRect->Y = 0;
			break;
		case 2:
			_pacman->_Position->X -= pacmanSpeed;
			_pacman->_sourceRect->Y = 64;
			break;
		case 3:
			_pacman->_Position->Y -= pacmanSpeed;
			_pacman->_sourceRect->Y = 96;
			break;
		case 4:
			_pacman->_Position->Y += pacmanSpeed;
			_pacman->_sourceRect->Y = 32;
			break;
		}
	}
};

void Pacman::CheckViewportCollision()
{
	// Controls Pacman's screen wrapping, teleporting him to the other end of the screen if he reaches the end of the viewport
	#pragma region ScreenWrapper
	if (_pacman->_Position->X > Graphics::GetViewportWidth() + _pacman->_sourceRect->Width)
		_pacman->_Position->X = 0 - _pacman->_sourceRect->Width;

	if (_pacman->_Position->X < 0 - _pacman->_sourceRect->Width)
		_pacman->_Position->X = Graphics::GetViewportWidth() + _pacman->_sourceRect->Width;

	if (_pacman->_Position->Y > Graphics::GetViewportHeight() + _pacman->_sourceRect->Width)
		_pacman->_Position->Y = 0 - _pacman->_sourceRect->Width;

	if (_pacman->_Position->Y < 0 - _pacman->_sourceRect->Width)
		_pacman->_Position->Y = Graphics::GetViewportHeight() + _pacman->_sourceRect->Width;
	#pragma endregion
}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey) {
	// Handles pausing, stops the game loop running while true

	if (state->IsKeyDown(pauseKey) && !_pacman->_pKeyDown) {
		_pacman->_pKeyDown = true;
		if (gameState == Active)
			gameState = Paused;
		else if (gameState == Paused)
			gameState = Active;
	}

	if (state->IsKeyUp(pauseKey)) {
		_pacman->_pKeyDown = false;
	}
};

void Pacman::UpdatePacman(int elapsedTime) {
	if (gameState == Active) {
		_pacman->_currentFrameTime += elapsedTime;
		if (_pacman->_currentFrameTime > _cPacmanFrameTime)
		{
			_pacman->_Frame++;
			if (_pacman->_Frame >= 3)
				_pacman->_Frame = 0;

			_pacman->_currentFrameTime = 0;
		}

		// Controls other Pacman collision
		for (int i = 0; i < munchie; i++) {
			if (Collision(_pacman->_Position, _pacman->_sourceRect, _munchie[i]->_Position, _munchie[i]->_Rect)) {
				delete _munchie[i]->_Rect;
				delete _munchie[i]->_Position;
				_pacman->_Score += 100;
				Audio::Play(_pop);
			}
		}

		for (int i = 0; i < wall; i++) {
			if (Collision(_pacman->_Position, _pacman->_sourceRect, _wall[i]->_Position, _wall[i]->_SourceRect)) {
				if (_pacman->_movementState == 1) {
					_pacman->_Position->X -= _cSpeed * elapsedTime * _pacman->_speedMultiplier;
				}
				else if (_pacman->_movementState == 2) {
					_pacman->_Position->X += _cSpeed * elapsedTime * _pacman->_speedMultiplier;
				}
				else if (_pacman->_movementState == 3) {
					_pacman->_Position->Y += _cSpeed * elapsedTime * _pacman->_speedMultiplier;
				}
				else if (_pacman->_movementState == 4) {
					_pacman->_Position->Y -= _cSpeed * elapsedTime * _pacman->_speedMultiplier;
				}

				/*if (_pacman->_Position->X + _pacman->_sourceRect->Width > _wall[i]->_Position->X + _wall[i]->_SourceRect->Width) {
					_pacman->_Position->X = _wall[i]->_Position->X - _wall[i]->_SourceRect->Width;
				}*/
			}
		}

		if (Collision(_pacman->_Position, _pacman->_sourceRect, _cherry->_Position, _cherry->_Rect)) {
			delete _cherry->_Rect;
			delete _cherry->_Position;
			_pacman->_Score += 250;
		}

		for (int i = 0; i < GHOSTCOUNT; i++) {
			if (Collision(_pacman->_Position, _pacman->_sourceRect, _ghost[i]->_Position, _ghost[i]->_sourceRect)) {
				gameState = Dead;
			}
		}
	}
	else if (gameState == Dead) {
		gameState = MainMenu;
	}
}

void Pacman::UpdateMunchie(Enemy*, int elapsedTime) {
	for (int i = 0; i < munchie; i++) {
		if (_munchie[i]->_currentFrameTime > _cMunchieFrameTime) {
			_munchie[i]->_frameCount++;

			if (_munchie[i]->_frameCount >= 2)
				_munchie[i]->_frameCount = 0;

			_munchie[i]->_currentFrameTime = 0;
		}
		_munchie[i]->_currentFrameTime += elapsedTime;
	}
}

void Pacman::UpdateCherry(int elapsedTime) {
	if (_cherry->_currentFrameTime > _cCherryFrameTime) {
		_cherry->_frameCount++;

		if (_cherry->_frameCount >= 2)
			_cherry->_frameCount = 0;

		_cherry->_currentFrameTime = 0;
	}
	_cherry->_currentFrameTime += elapsedTime;
}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedtime) {
	if (ghost->_Direction == 0) {
		ghost->_Position->X += ghost->_Speed * elapsedtime;
	}
	else if (ghost->_Direction == 1) {
		ghost->_Position->X -= ghost->_Speed * elapsedtime;
	}
	if (ghost->_Position->X + ghost->_sourceRect->Width >= Graphics::GetViewportWidth()) {
		ghost->_Direction = 1;
	}
	else if (ghost->_Position->X <= 0) {
		ghost->_Direction = 0;
	}
}