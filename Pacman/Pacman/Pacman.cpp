#include "Pacman.h"
#include <iostream>
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.20f), _pKeyDown(false), gameStarted(false), _cPacmanFrameTime(250), _cMunchieFrameTime(250)
{
	_munchieFrameCount = 0;
	_pacmanCurrentFrameTime = 0;
	_pacmanFrame = 0;
	_munchieCurrentFrameTime = 0;
	movementState = 5;
	Score = 0;
	_paused = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacmanTexture;
	delete _pacmanSourceRect;
	delete _munchieBlueTexture;
	delete _munchieRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.png", false);
	_pacmanPosition = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect = new Rect(350.0f, 350.0f, 32, 32);

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.png", true);
	_munchieRect = new Rect(0.0f, 0.0f, 12, 12);
	_munchiePosition = new Vector2(250.0f, 250.0f);
	

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

	// Creates the condition for exiting the start menu
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && !gameStarted) {
		gameStarted = true;
	}

	if (gameStarted)
	{
		// Handles pausing, stops the game loop running while true
		if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown) {
			_pKeyDown = true;
			_paused = !_paused;
		}
		if (keyboardState->IsKeyUp(Input::Keys::P))
			_pKeyDown = false;

		if (!_paused)
		{
			// Controls the player's movement by taking an "enum" for movement states
			if (keyboardState->IsKeyDown(Input::Keys::D))
				movementState = 1;
			if (keyboardState->IsKeyDown(Input::Keys::A))
				movementState = 2;
			if (keyboardState->IsKeyDown(Input::Keys::W))
				movementState = 3;
			if (keyboardState->IsKeyDown(Input::Keys::S))
				movementState = 4;

			switch (movementState) {
			case 1:
				_pacmanPosition->X += _cPacmanSpeed * elapsedTime;
				_pacmanSourceRect->Y = 0;
				break;
			case 2:
				_pacmanPosition->X -= _cPacmanSpeed * elapsedTime;
				_pacmanSourceRect->Y = 64;
				break;
			case 3:
				_pacmanPosition->Y -= _cPacmanSpeed * elapsedTime;
				_pacmanSourceRect->Y = 96;
				break;
			case 4:
				_pacmanPosition->Y += _cPacmanSpeed * elapsedTime;
				_pacmanSourceRect->Y = 32;
				break;
			}

			// Controls Pacman's screen wrapping, teleporting him to the other end of the screen if he reaches the end of the viewport
			if (_pacmanPosition->X > Graphics::GetViewportWidth() + _pacmanSourceRect->Width)
				_pacmanPosition->X = 0 - _pacmanSourceRect->Width;

			if (_pacmanPosition->X < 0 - _pacmanSourceRect->Width)
				_pacmanPosition->X = Graphics::GetViewportWidth() + _pacmanSourceRect->Width;

			if (_pacmanPosition->Y > Graphics::GetViewportHeight() + _pacmanSourceRect->Width)
				_pacmanPosition->Y = 0 - _pacmanSourceRect->Width;

			if (_pacmanPosition->Y < 0 - _pacmanSourceRect->Width)
				_pacmanPosition->Y = Graphics::GetViewportHeight() + _pacmanSourceRect->Width;

			// Controls other Pacman collision
			if (Collision(_munchiePosition, _munchieRect)) {
				delete _munchieRect;
				delete _munchiePosition;
				Score += 100;
			}
		}
	}
	_pacmanCurrentFrameTime += elapsedTime;

	if (_pacmanCurrentFrameTime > _cPacmanFrameTime)
	{
		_pacmanFrame++;

		if (_pacmanFrame >= 3)
			_pacmanFrame = 0;

		_pacmanCurrentFrameTime = 0;
	}

	_munchieCurrentFrameTime += elapsedTime;

	if (_munchieCurrentFrameTime > _cMunchieFrameTime) {
		_munchieFrameCount++;

		if (_munchieFrameCount >= 2)
			_munchieFrameCount = 0;

		_munchieCurrentFrameTime = 0;
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score: " << Score;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman
	SpriteBatch::Draw(_munchieBlueTexture, _munchiePosition, _munchieRect);

	_munchieRect->X = _munchieRect->Width * _munchieFrameCount;
	_pacmanSourceRect->X = _pacmanSourceRect->Width * _pacmanFrame;
	
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
	if (_pacmanPosition->X + _pacmanSourceRect->Width > Actor->X
		&& _pacmanPosition->X < Actor->X + ActorRect->Width
		&& _pacmanPosition->Y + _pacmanSourceRect->Height > Actor->Y
		&& _pacmanPosition->Y < Actor->Y + ActorRect->Height)
	{
		return true;
	}
	else return false;
}