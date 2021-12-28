#include "Pacman.h"
#include <iostream>
#include <fstream>
#include <sstream>

gameStates gameState = MainMenu;

Pacman::Pacman(int argc, char* argv[])
	: Game(argc, argv), _pacman(), _munchie(), _cSpeed(0.25f), _cPacmanFrameTime(250), _cMunchieFrameTime(25000), _cCherryFrameTime(500)
{
	_pacman  = new Player();

	_cherry	 = new Enemy();
	
	_pacman->_currentFrameTime = 0;
	_pacman->_Frame = 0;
	_pacman->_movementState = 5;
	_pacman->_Score = 0;
	_pacman->_pKeyDown = false;
	_pacman->_soundState = 0;

	_eat1 = new SoundEffect();
	_eat2 = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	gridX = Graphics::GetViewportWidth() / 32;
	gridY = Graphics::GetViewportHeight() / 24;
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

	for (int i = 0; i < wall; i++) {
		delete _wall[i]->_Texture;
		delete _wall[i]->_SourceRect;
		delete _wall[i];
	}
	
	delete[] _munchie;
	delete[] _wall;
	delete _cherry->_Texture;
	delete _cherry->_Rect;
	delete _cherry;
	delete _eat1;
	delete _eat2;
}

void Pacman::LoadLevel(string levelName)
{
	ifstream ifs(levelName, ios::binary | ios::in);
	string line;

	wall = 0;	
	munchie = 0;
	ghost = 0;

	masterGrid[23][31] = { 0 };

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Munchie.png", false);

	Texture2D* wallTex = new Texture2D();
	wallTex->Load("Textures/Wall.png", false);

	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/GhostBlue.png", true);

	while (!ifs.eof()) {
		while (getline(ifs, line))
		{
			for (int i = 0; i < line.size(); i++) {

				int xOfset = i * 32;
				int yOfset = (int(ifs.tellg() / 34) * 32) - 32;

				if (line[i] == 'x') {
					_wall[wall] = new ColliderObject();
					_wall[wall]->_Texture = wallTex;
					_wall[wall]->_Position = new Vector2(xOfset, yOfset);
					_wall[wall]->_SourceRect = new Rect(0.0f, 0.0f, 32, 32);

					masterGrid[(int(ifs.tellg()) / 34) - 1][i] = 1;
					
					wall++;
				}
				else if (line[i] == 'o') {
					_munchie[munchie] = new Enemy();
					_munchie[munchie]->_currentFrameTime = 0;
					_munchie[munchie]->_Frame = 0;
					_munchie[munchie]->_Texture = munchieTex;
					_munchie[munchie]->_Position = new Vector2(xOfset + 10, yOfset + 10);
					_munchie[munchie]->_Rect = new Rect(0.0f, 0.0f, 12, 12);
					munchie++;
				}
				else if (line[i] == 'M') {
					_ghost[ghost] = new MovingEnemy();
					_ghost[ghost]->_Direction = 0;
					_ghost[ghost]->_Speed = 0.2f;
					_ghost[ghost]->_Texture = ghostTex;
					_ghost[ghost]->_Position = new Vector2(xOfset, yOfset);
					_ghost[ghost]->_centerPosition = new Vector2(0 ,0);
					_ghost[ghost]->_sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					ghost++;
				}
			}
		}
	}
}

void Pacman::LoadContent()
{
	do
	{
		LoadLevel("levels/1.txt");
	} while (false);

	// Load Pacman
	_pacman->_Texture = new Texture2D();
	_pacman->_Texture->Load("Textures/Pacman.png", false);
	_pacman->_Position = new Vector2(350.0f, 350.0f);
	_pacman->_sourceRect = new Rect(0, 0, 28, 28);
	_pacman->_centerPosition = new Vector2(0,0);

	_eat1->Load("Sounds/eat1.wav");
	_eat2->Load("Sounds/eat2.wav");
	
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
	if (keyboardState->IsKeyDown(Input::Keys::SPACE) && gameState == MainMenu) {
		gameState = Active;
	}	
	else
	{
		if (gameState != Paused && gameState != MainMenu)
		{
			Input(elapsedTime, keyboardState, mouseState);
			UpdatePacman(elapsedTime);

			for (int i = 0; i < ghost; i++) {
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

	for (int i = 0; i < wall; i++)
		SpriteBatch::Draw(_wall[i]->_Texture, _wall[i]->_Position, _wall[i]->_SourceRect);

	for (int i = 0; i < ghost; i++) {
		SpriteBatch::Draw(_ghost[i]->_Texture, _ghost[i]->_Position, _ghost[i]->_sourceRect);
	}

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

bool Pacman::Collision(float actorX, float actorY, Vector2* Target, Rect* TargetRect) {
	if (actorX > Target->X
		&& actorY > Target->Y
		&& actorX < Target->X + TargetRect->Width
		&& actorY < Target->Y + TargetRect->Height)
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

void Pacman::CheckViewportCollision(Player* _pacman)
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

void Pacman::CheckViewportCollision(MovingEnemy* _pacman)
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
		_pacman->_centerPosition->X = _pacman->_Position->X + _pacman->_sourceRect->Width / 2;
		_pacman->_centerPosition->Y = _pacman->_Position->Y + _pacman->_sourceRect->Height / 2;

		CheckViewportCollision(_pacman);
		
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
				_pacman->_Score += 10;
				if (_pacman->_soundState == 0) {
					Audio::Play(_eat1);
					_pacman->_soundState = 1;
				}
				else {
					Audio::Play(_eat2);
					_pacman->_soundState = 0;
				}
			}
		}

		/*for (int i = 0; i < wall; i++) {
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
			}
		}*/

		if (Collision(_pacman->_Position, _pacman->_sourceRect, _cherry->_Position, _cherry->_Rect)) {
			delete _cherry->_Rect;
			delete _cherry->_Position;
			_pacman->_Score += 250;
		}

		for (int i = 0; i < ghost; i++) {
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

float Pacman::distanceToPacman(float input, float input2) {
	return sqrt(pow(input - _pacman->_centerPosition->X, 2) + pow(input2 - _pacman->_centerPosition->Y, 2));
}

void Pacman::gridCheck(int levelArray[24][32], MovingEnemy* ghost, int Offset, int elapsedTime)
{
	int posX = ghost->_centerPosition->X / gridX;
	int posY = ghost->_centerPosition->Y / gridY;

	float choice[4];
	choice[0] = distanceToPacman(ghost->_centerPosition->X + Offset * 32, ghost->_centerPosition->Y);
	choice[1] = distanceToPacman(ghost->_centerPosition->X - Offset * 32, ghost->_centerPosition->Y);
	choice[2] = distanceToPacman(ghost->_centerPosition->X, ghost->_centerPosition->Y + Offset * 24);
	choice[3] = distanceToPacman(ghost->_centerPosition->X, ghost->_centerPosition->Y - Offset * 24);
		
	float choiceClone[4] = { choice[0], choice[1], choice[2], choice[3] };

	if (levelArray[posY][posX + Offset] == 1 || ghost->_Direction == 1)
		choice[0] = 9999;
	if (levelArray[posY][posX - Offset] == 1 || ghost->_Direction == 0)
		choice[1] = 9999;
	if (levelArray[posY + Offset][posX] == 1 || ghost->_Direction == 3)
		choice[2] = 9999;
	if (levelArray[posY - Offset][posX] == 1 || ghost->_Direction == 2)
		choice[3] = 9999;

	for (int i = 0; i < 4; i++) {
		if (choice[0] > choice[i]) {
			choice[0] = choice[i];
		}
	}

	for (int i = 0; i < 4; i++) {
		if (choice[0] == choiceClone[i]) {
			ghost->_Direction = i;
		}
	}

	switch (ghost->_Direction)
	{
	case 0:
		ghost->_Position->X += ghost->_Speed * elapsedTime;
		break;
	case 1:
		ghost->_Position->X -= ghost->_Speed * elapsedTime;
		break;
	case 2:
		ghost->_Position->Y += ghost->_Speed * elapsedTime;
		break;
	case 3:		
		ghost->_Position->Y -= ghost->_Speed * elapsedTime;
		break;
	default:
		break;
	}
}

void Pacman::UpdateGhost(MovingEnemy* ghosts, int elapsedtime) {

	ghosts->_centerPosition->X = ghosts->_Position->X + ghosts->_sourceRect->Width / 2;
	ghosts->_centerPosition->Y = ghosts->_Position->Y + ghosts->_sourceRect->Height / 2;

	float distance = distanceToPacman(ghosts->_centerPosition->X, ghosts->_centerPosition->Y);

	for (int i = 0; i < ghost; i++) {
		gridCheck(masterGrid, _ghost[i], 1, elapsedtime);
		CheckViewportCollision(_ghost[i]);
	}
}