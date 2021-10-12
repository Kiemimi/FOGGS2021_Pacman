#include "Pacman.h"

#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv)
{
	_frameCount = 0;
	movementState = 5;

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
	delete _munchieInvertedTexture;
	delete _munchieRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.tga", false);
	_pacmanPosition = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(100.0f, 450.0f, 12, 12);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

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
		_pacmanPosition->X += 0.1f * elapsedTime;
		_pacmanSourceRect->Y = 0;
		break;
	case 2:
		_pacmanPosition->X -= 0.1f * elapsedTime;
		_pacmanSourceRect->Y = 64;
		break;
	case 3:
		_pacmanPosition->Y -= 0.1f * elapsedTime;
		_pacmanSourceRect->Y = 98;
		break;
	case 4:
		_pacmanPosition->Y += 0.1f * elapsedTime;
		_pacmanSourceRect->Y = 32;
		break;
	}

	if (_pacmanPosition->X > 1024 + _pacmanSourceRect->Width)
	{
		_pacmanPosition->X = 0 - _pacmanSourceRect->Width;
	}
	if (_pacmanPosition->X < 0 - _pacmanSourceRect->Width) {
		_pacmanPosition->X = 1024 + _pacmanSourceRect->Width;
	}
	if (_pacmanPosition->Y > 768 + _pacmanSourceRect->Width) {
		_pacmanPosition->Y = 0 - _pacmanSourceRect->Width;
	}
	if (_pacmanPosition->Y < 0 - _pacmanSourceRect->Width) {
		_pacmanPosition->Y = 768 + _pacmanSourceRect->Width;
	}

}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacmanPosition->X << " Y: " << _pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman

	if (_frameCount < 30)
	{
		// Draws Red Munchie
		SpriteBatch::Draw(_munchieInvertedTexture, _munchieRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		_pacmanSourceRect->X = 0;
		_frameCount++;
	}
	else
	{
		// Draw Blue Munchie
		SpriteBatch::Draw(_munchieBlueTexture, _munchieRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		_pacmanSourceRect->X = 32;
		_frameCount++;
		if (_frameCount >= 60)
			_frameCount = 0;
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}