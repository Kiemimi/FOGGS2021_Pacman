#include "Pacman.h"
#include <iostream>
#include <sstream>

Pacman::Pacman(int argc, char* argv[])
	: Game(argc, argv), _pacman(), _munchie(), gameStarted(false), _cSpeed(0.25f), _cPacmanFrameTime(250), _cMunchieFrameTime(25000), _cCherryFrameTime(500)
{
	_pacman  = new Player();
	
	int i;

	for (i = 0; i < MUNCHIECOUNT; i++) {
		_munchie[i] = new Enemy();
		_munchie[i]->_currentFrameTime = 0;
		_munchie[i]->_Frame = 0;
	}

	_cherry	 = new Enemy();

	_pacman->_currentFrameTime = 0;
	_pacman->_Frame = 0;
	_pacman->_movementState = 5;
	_pacman->_Score = 0;
	_pacman->_pKeyDown = false;
	
	_paused = false;

	//Initialise important Game aspects
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

	for (int i = 0; i < MUNCHIECOUNT; i++) {
		delete _munchie[i]->_Texture;
		delete _munchie[i]->_Rect;
		delete _munchie[i];
	}
	
	delete[] _munchie;
	delete _cherry->_Texture;
	delete _cherry->_Rect;
	delete _cherry;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->_Texture = new Texture2D();
	_pacman->_Texture->Load("Textures/Pacman.png", false);
	_pacman->_Position = new Vector2(350.0f, 350.0f);
	_pacman->_sourceRect = new Rect(350.0f, 350.0f, 32, 32);

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);

	// Load Munchie
	for (int i = 0; i < MUNCHIECOUNT; i++) {
		_munchie[i]->_Texture = munchieTex;
		_munchie[i]->_Rect = new Rect(0.0f, 0.0f, 12, 12);
		_munchie[i]->_Position = new Vector2(rand() % Graphics::GetViewportWidth(), (rand() % Graphics::GetViewportHeight()));
	}
	
	// Load Cherry
	_cherry->_Texture = new Texture2D();
	_cherry->_Texture->Load("Textures/Cherry.png", true);
	_cherry->_Rect = new Rect(0.0f, 0.0f, 32, 32);
	_cherry->_Position = new Vector2(500.0f, 250.0f);

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
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && !gameStarted) {
		gameStarted = true;
	}	
	else
	{
		if (!_paused && gameStarted)
		{
			Input(elapsedTime, keyboardState, mouseState);
			CheckViewportCollision();
			UpdatePacman(elapsedTime);

			for (int i = 0; i < MUNCHIECOUNT; i++) {
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

	for (int i = 0; i < MUNCHIECOUNT; i++) {
		SpriteBatch::Draw(_munchie[i]->_Texture, _munchie[i]->_Position, _munchie[i]->_Rect);
		_munchie[i]->_Rect->X = _munchie[i]->_Rect->Width * _munchie[i]->_frameCount;
	}

	SpriteBatch::Draw(_cherry->_Texture, _cherry->_Position, _cherry->_Rect);

	_pacman->_sourceRect->X = _pacman->_sourceRect->Width * _pacman->_Frame;
	_cherry->_Rect->X = _cherry->_Rect->Width * _cherry->_frameCount;
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::White);
	
	if (_paused)
	{
		std::stringstream menuStream; 
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (!gameStarted)
	{
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO START!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Green);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
}

bool Pacman::Collision(Vector2* Actor, Rect* ActorRect) {
	if (_pacman->_Position->X + _pacman->_sourceRect->Width > Actor->X
		&& _pacman->_Position->X < Actor->X + ActorRect->Width
		&& _pacman->_Position->Y + _pacman->_sourceRect->Height > Actor->Y
		&& _pacman->_Position->Y < Actor->Y + ActorRect->Height)
	{
		return true;
	}
	else return false;
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState) {
	float pacmanSpeed = _cSpeed * elapsedTime * _pacman->_speedMultiplier;

	// Controls the player's movement by taking an "enum" for movement states
	#pragma region KeyboardInputHandler
		if (state->IsKeyDown(Input::Keys::D))
			_pacman->_movementState = 1;
		if (state->IsKeyDown(Input::Keys::A))
			_pacman->_movementState = 2;
		if (state->IsKeyDown(Input::Keys::W))
			_pacman->_movementState = 3;
		if (state->IsKeyDown(Input::Keys::S))
			_pacman->_movementState = 4;
		if (state->IsKeyDown(Input::Keys::R)) {
			_cherry->_Position->X = rand() % Graphics::GetViewportWidth() - 64;
			_cherry->_Position->Y = rand() % Graphics::GetViewportWidth() - 64;
		}
		state->IsKeyDown(Input::Keys::LEFTSHIFT) ? _pacman->_speedMultiplier = 2.25f : _pacman->_speedMultiplier = 1.0f;
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
		_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pacman->_pKeyDown = false;
};

void Pacman::UpdatePacman(int elapsedTime) {
	_pacman->_currentFrameTime += elapsedTime;
	if (_pacman->_currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->_Frame++;
		if (_pacman->_Frame >= 3)
			_pacman->_Frame = 0;

		_pacman->_currentFrameTime = 0;
	}

	// Controls other Pacman collision
	for (int i = 0; i < MUNCHIECOUNT; i++) {
		if (Collision(_munchie[i]->_Position, _munchie[i]->_Rect)) {
			delete _munchie[i]->_Rect;
			delete _munchie[i]->_Position;
			_pacman->_Score += 100;
		}
	}

	if (Collision(_cherry->_Position, _cherry->_Rect)) {
		delete _cherry->_Rect;
		delete _cherry->_Position;
		_pacman->_Score += 250;
	}
}

void Pacman::UpdateMunchie(Enemy*, int elapsedTime) {
	for (int i = 0; i < MUNCHIECOUNT; i++) {
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