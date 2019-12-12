#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include "Pacman.h"
#include <time.h>
#include <thread>


Pacman::Pacman(int argc, char* argv[], int munchieCount, int powerPellet, int ghostCount) : Game(argc, argv)
{
	srand(time(NULL));
	//munchie count is currently 303
	munchieCount = munchieCount;
	_munchie = new Consumable* [munchieCount];

	ghostCount = ghostCount;
	_ghosts = new MovingEnemy* [ghostCount];

	powerPellet = powerPellet;
	_powerPellet = new Consumable* [powerPellet];



	for (int i = 0; i < munchieCount; i++)
	{
		_munchie[i] = new Consumable();
		_munchie[i]->frameCount = rand() % 1;
		_munchie[i]->currentFrameTime = 0;
		_munchie[i]->_frameTime = rand() % 500 + 50;
	}

	for (int i = 0; i < powerPellet; i++)
	{
		_powerPellet[i] = new Consumable();
		_powerPellet[i]->frameCount = 1;
		_powerPellet[i]->currentFrameTime = 0;
		_powerPellet[i]->_frameTime = 400;
	}

	_pacman = new Player();
	_level = new level();

	_pacman->dead = false;
	//Remaining initialiation
	_paused = false;
	_pKeyDown = false;
	_qKeyDown = false;
	_eKeyDown = false;
	_GameStarted = false;
	_pacman->direction = 4;
	_pacman->currentFrameTime = 100;
	_pacman->frame = 0;
	_pacman->speed = 0.2f;
	_pacman->frameTime = 100;
	_pacman->collider = new Rect;
	_pacman->lives = 3;
	//_pacman->inputUp = new Rect;
	//_pacman->inputDown = new Rect;
	//_pacman->inputLeft = new Rect;
	//_pacman->inputRight = new Rect;
	_pacman->Pacman32PosX = 0;
	_pacman->Pacman32PosY = 0;
	_pacman->nextDir = 4;
	

	//nodes[mapHeight][mapWidth] = new sNode;
	//nodeStart[mapHeight][mapWidth] = new sNode;
	//nodeEnd[mapHeight][mapWidth] = new sNode;
	//nodes[mapHeight][mapWidth]->vecNeighbours.push_back(&nodes[mapHeight][mapWidth])
	//
	//Wall Creation
	for (int x = 0; x < 28; x++)
	{
		for (int y = 0; y < 30; y++)
		{
			_map[x][y] = new Wall;
		}
	}



	//initialise ghost character
	for (int i = 0; i < ghostCount; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.15f;
		_ghosts[i]->collider = new Rect;
		_ghosts[i]->spooked = false;
	}

	//Initialise audio
	_pop = new SoundEffect();
	_powerup = new SoundEffect();
	_powerupEnd = new SoundEffect();
	_lifeLost = new SoundEffect();
	_gameOverSound = new SoundEffect();
	_winSound = new SoundEffect();
	_backgroundSong = new SoundEffect();
	_ghostDed = new SoundEffect();


	//timer settings
	_timer = new timer;
	_timer->storedTime = 5;

	//score
	_score = new score;
	_score->_scoreInt = 0;
	_score->munchiesEaten = 0;
	_gameOver = false;
	_win = false;
	_selectedTileset = 0;
	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 896, 960, false, 25, 25, "Pacman", 60);
	Input::Initialise();
	recentChanged = false;
	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();

	checkTileSet();

	//Initialise member variables

}

Pacman::~Pacman()
{
	//clean up pointers within the Pacman Structure
	delete _pacman->position;
	delete _pacman->sourceRect;
	delete _pacman->texture;
	delete _pacman;

	for (int i = 0; i < munchieCount; i++)
	{
		delete _munchie[i]->texture;
		delete _munchie[i]->rect;
		delete _munchie[i];
	}
	delete[] _munchie;

	for (int i = 0; i < ghostCount; i++)
	{
		delete _ghosts[i]->texture;
		delete	_ghosts[i]->sourceRect;
		delete _ghosts[i]->position;
		delete _ghosts[i];
	}
	delete[] _ghosts;

	for (int i = 0; i < powerPellet; i++)
	{
		delete _powerPellet[i]->texture;
		delete _powerPellet[i]->rect;
		delete _powerPellet[i];
	}
	delete[] _powerPellet;

	delete _pop ;
	delete _powerup ;
	delete _powerupEnd ;
	delete _lifeLost ;
	delete _gameOverSound ;
	delete _winSound ;
	delete _backgroundSong ;
	delete _ghostDed ;

	delete _score;

}


void Pacman::LoadContent()
{
	//load Audio
	_pop->Load("Sounds/pop.wav");
	_powerup->Load("Sounds/powerup.wav");
	_powerupEnd->Load("Sounds/powerdown.wav");
	_lifeLost->Load("Sounds/badSound.wav");
	_gameOverSound->Load("Sounds/gameOver.wav");
	_winSound->Load("Sounds/win.wav");
	_backgroundSong->Load("Sounds/pacman.wav");
	_ghostDed->Load("Sounds/ghostded.wav");
	_backgroundSong->SetLooping(true);
	

	//Load Wall and level
	Texture2D* wallTex = new Texture2D();
	wallTex->Load("Textured/wall.png", true);
	Audio::Play(_backgroundSong);
	// Load Pacman
	_pacman->texture = new Texture2D();
	switch (_selectedTileset)
	{
	case 0:
		_pacman->texture->Load("Textures/TileSet1/Pacman.tga", false);
		break;
	case 1:
		_pacman->texture->Load("Textures/TileSet2/LeggySpriteSheet.png", false);
		break;
	case 2:
		_pacman->texture->Load("Textures/TileSet3/rollMan.png", false);
		break;
	}
	_pacman->position = new Vector2(446.0f, 385.0f);
	_pacman->centrePos = new Vector2;
	//_pacman->leftPos = new Vector2;
	//_pacman->rightPos = new Vector2;
	//_pacman->topPos = new Vector2;
	//_pacman->bottomPos = new Vector2;

	_pacman->centrePos->X = (_pacman->position->X + 16);
	_pacman->centrePos->Y = (_pacman->position->Y + 16);
	//_pacman->leftPos->X = (_pacman->centrePos->X - 8);
	//_pacman->leftPos->Y = (_pacman->centrePos->Y);
	//_pacman->rightPos->X = (_pacman->centrePos->X + 8);
	//_pacman->rightPos->Y = (_pacman->centrePos->Y);
	//_pacman->bottomPos->X = (_pacman->centrePos->X);
	//_pacman->bottomPos->Y = (_pacman->centrePos->Y + 8);
	//_pacman->topPos->X = (_pacman->centrePos->X);
	//_pacman->topPos->Y = (_pacman->centrePos->Y - 8);

	_pacman->Pacman32PosX = (_pacman->centrePos->X / 32);
	_pacman->Pacman32PosY = (_pacman->centrePos->Y / 32);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/Tileset1/MunchieSpriteSheet.png", true);

	Texture2D* powerPelletTex = new Texture2D();
	powerPelletTex->Load("Textures/Tileset1/powerPelletSprite", true);
	/*for (int i = 0; i < munchieCount; i++)
	{
		_munchie[i]->texture = munchieTex;
		_munchie[i]->rect = new Rect(0.0f, 0.0f, 12, 12);
		_munchie[i]->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
	}
*/
	//Loading the level
	Level();

	//// Load Cherry
	//_cherry->texture = new Texture2D();
	//_cherry->texture->Load("Textures/Tileset1/CheerySpritesheet.png", true);
	//_cherry->position = new Vector2(rand() % Graphics::GetViewportWidth(), rand() % Graphics::GetViewportHeight());
	//_cherry->rect = new Rect(0.0f, 0.0f, 12, 12);
	//_cherry->_frameTime = 500;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Tileset1/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//set Game Over Parameters
		//Set Game Over Parameters
	_gameOverBackground = new Texture2D();
	_gameOverBackground->Load("Textures/TileSet1/Transparency.png", false);
	_gameOverRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_gameOverStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

		//win
	_winScreen = new Texture2D();
	_winScreen->Load("Textures/TileSet1/Transparency.png", false);
	_winText = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_winPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	//initialise ghosty boy
	/*for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture = new Texture2D();
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
		switch (i)
		{
		case 1:
		{
			_ghosts[i]->texture->Load("Textures/Tileset1/GhostBlueSpriteSheet.png", false);
			break;
		}
		case 2:
		{
			_ghosts[i]->texture->Load("Textures/Tileset1/redGhostSprite.png", false);
			break;
		}
		case 3:
		{
			_ghosts[i]->texture->Load("Textures/Tileset1/pingGhostSpriteSheet.png", false);
			break;
		}
		case 4:
		{
			_ghosts[i]->texture->Load("Textures/Tileset1/orangeGhostSpriteSheet.png", false);
			break;
		}
		}
	}*/

	//loading score
	_scorePosition = new Vector2(700.0f, 25.0f);
	_score->_scoreInt = 0;
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	//Runs the check paused method
	CheckPaused(keyboardState);
	

	
	if (!_paused && _GameStarted)
	{
		//Runs the method to check for inputs
		CheckWallCollision(elapsedTime);
		Input(elapsedTime, keyboardState);
		//Runs the update pacman method
		UpdatePacman(elapsedTime);
		for (int i = 0; i < ghostCount; i++)
		{
			CheckGhostWall(_ghosts[i], elapsedTime);
		}
		CheckGhostCollisions();
		for (int i = 0; i < 4; i++)
		{
			UpdateGhost(_ghosts[i], elapsedTime);
			/*CheckGhostWall(_ghosts[i], elapsedTime);*/
		}

		CheckMunchieCollisions();
		//Runs the method to update the munchie sprite
		for (int i = 0; i < munchieCount; i++)
		{
			UpdateMunchie(_munchie[i], elapsedTime);
		}
		for (int i = 0; i < powerPellet; i++)
		{
			UpdatePellet(_powerPellet[i], elapsedTime);
			CheckPelletCollision();
		}
		CheckWin();
		if (_pacman->pellet)
		{
			
			switch (_selectedTileset)
			{
			case 0:
				_pacman->texture->Load("Textures/TileSet1/pacmanPowerup.png", false);
				break;
			case 1:
				_pacman->texture->Load("Textures/TileSet2/LeggySpriteSheetNo.png", false);
				break;
			case 2:
				_pacman->texture->Load("Textures/TileSet3/invertRoll.png", false);
				break;
			}
			clock_t start = clock();
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				switch (i)
				{
					case 0:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/blueScared.png", true);
						break;
					}
					case 1:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/redScared.png", true);
						break;
					}
					case 2:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/pinkScared.png", true);
						break;
					}
					case 3:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/orangeScared.png", true);
						break;
					}
				}
				_ghosts[i]->spooked = true;
			}
			clock_t end = clock();
			double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
			CheckTimer(elapsed_secs);
			if (_timer->storedTime == 0)
			{
				Audio::Play(_powerupEnd);
				_pacman->pellet = false;
				switch (_selectedTileset)
				{
				case 0:
					_pacman->texture->Load("Textures/TileSet1/Pacman.tga", false);
					break;
				case 1:
					_pacman->texture->Load("Textures/TileSet2/LeggySpriteSheet.png", false);
					break;
				case 2:
					_pacman->texture->Load("Textures/TileSet3/rollMan.png", false);
					break;
				}
				for (int i = 0; i < GHOSTCOUNT; i++)
				{
					_ghosts[i]->spooked = false;
					RespawnGhosts();
				}
			}
		}
			/*UpdateCherry(elapsedTime);*/
		//Runs the collision method to check player position
		CheckViewportCollision();

	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	//stream << "Pacman X: " << _pacman->Pacman32PosX << " Y: " << _pacman->Pacman32PosY << " Pacman POSX: " << _pacman->centrePos->X << " Tileset: " << _selectedTileset << " MUNCH: " << _score->munchiesEaten;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (!_pacman->dead)
	{	// Draws Pacman
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); 
	}

	for (int y = 0; y < mapHeight; y++)
	{
		for (int x = 0; x < mapWidth; x++)
		{
			
			SpriteBatch::Draw(_map[x][y]->texture, _map[x][y]->position, _map[x][y]->sourceRect);
		}
	}

	for (int i = 0; i < munchieCount; i++)
	{
		SpriteBatch::Draw(_munchie[i]->texture, _munchie[i]->position, _munchie[i]->rect);
	}

	for (int i = 0; i < powerPellet; i++)
	{
		SpriteBatch::Draw(_powerPellet[i]->texture, _powerPellet[i]->position, _powerPellet[i]->rect);
	}

	//draw da ghost
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}

	//SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->rect);
	/*if (_munchieFrameCount == 0)
	{
		// Draws Red Munchie
		SpriteBatch::Draw(_munchieInvertedTexture, _munchieRect, nullptr, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE);
		
	}
	else
	{
		// Draw Blue Munchie
		
		
		

		if (_munchieFrameCount >= 60)
			_munchieFrameCount = 0;
	} */
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);

	}
	if (!_GameStarted)
	{
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO START!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	if (_pacman->dead)
	{
		std::stringstream gameOverStream;
		gameOverStream << "GAME OVER!";

		SpriteBatch::Draw(_gameOverBackground, _gameOverRectangle, nullptr);
		SpriteBatch::DrawString(gameOverStream.str().c_str(), _gameOverStringPosition, Color::Red);
	}
	if (_win)
	{
		std::stringstream winScreen;
		winScreen << "You Win!";

		SpriteBatch::Draw(_winScreen, _winText, nullptr);
		SpriteBatch::DrawString(winScreen.str().c_str(), _winPosition, Color::Green);
		
	}
	stringstream scoreText;
	_score->_scoreString = to_string(_score->_scoreInt);
	scoreText << "Score: " << _score->_scoreString;
	SpriteBatch::DrawString(scoreText.str().c_str(), _scorePosition, Color::Red);

	SpriteBatch::EndDraw(); 
	// Ends Drawing
}



void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState)
{
	_pacman->centrePos->X = (_pacman->position->X + 16);
	_pacman->centrePos->Y = (_pacman->position->Y + 16);
	//_pacman->leftPos->X = (_pacman->centrePos->X - 16);
	//_pacman->leftPos->Y = (_pacman->centrePos->Y);
	//_pacman->rightPos->X = (_pacman->centrePos->X + 16);
	//_pacman->rightPos->Y = (_pacman->centrePos->Y);
	//_pacman->bottomPos->X = (_pacman->centrePos->X);
	//_pacman->bottomPos->Y = (_pacman->centrePos->Y + 16);
	//_pacman->topPos->X = (_pacman->centrePos->X);
	//_pacman->topPos->Y = (_pacman->centrePos->Y - 16);

	_pacman->Pacman32PosX = (_pacman->centrePos->X / 32);
	_pacman->Pacman32PosY = (_pacman->centrePos->Y / 32);

	floorY = floor((_pacman->position->Y + 16) / 32);
	ceilY = ceil((_pacman->position->Y-16) / 32);
	floorX = floor((_pacman->position->X + 16) / 32);
	ceilX = ceil((_pacman->position->X -16) / 32);

	// Checks if each key is pressed and sets Pacmans direction accordingly
	if (keyboardState->IsKeyDown(Input::Keys::D))
		_pacman->direction = 0;
	else if (keyboardState->IsKeyDown(Input::Keys::A))
		_pacman->direction = 2;
	else if (keyboardState->IsKeyDown(Input::Keys::W))
		_pacman->direction = 3;
	else if (keyboardState->IsKeyDown(Input::Keys::S))
		_pacman->direction = 1;


	//switch (_pacman->nextDir)
	//{
	//	case 0:
	//	{

	//		if (_pacman->direction == 2)
	//		{
	//			_pacman->direction = 0;
	//		}
	//		else if (!_map[floorY][ceilX + 1]->solid && _pacman->direction == 1)
	//		{
	//			_pacman->direction = 0;
	//		}
	//		else if (!_map[ceilY][ceilX + 1]->solid && _pacman->direction == 3)
	//		{
	//			_pacman->direction = 0;
	//		}

	//		break;
	//	}
	//	case 1:
	//	{
	//		if (_pacman->direction == 3)
	//		{
	//			_pacman->direction = 1;
	//		}
	//		else if (!_map[floorY - 1][floorX]->solid && _pacman->direction == 2)
	//		{
	//			_pacman->direction = 1;
	//		}
	//		else if (!_map[floorY - 1][ceilX]->solid && _pacman->direction == 0)
	//		{
	//			_pacman->direction = 1;
	//		}
	//		break;
	//	}
	//	case 2:
	//	{
	//		if (_pacman->direction == 0)
	//		{
	//			_pacman->direction = 2;
	//		}
	//		else if (!_map[floorY][floorX + 1]->solid && _pacman->direction == 1)
	//		{
	//			_pacman->direction = 2;
	//		}
	//		else if (!_map[ceilY][floorX + 1]->solid && _pacman->direction == 3)
	//		{
	//			_pacman->direction = 2;
	//		}
	//		break;
	//	}

	//	case 3:
	//	{
	//		if (_pacman->direction == 1)
	//		{
	//			_pacman->direction = 3;
	//		}
	//		else if (!_map[floorY - 1][ceilX]->solid && _pacman->direction == 2)
	//		{
	//			_pacman->direction = 3;
	//		}
	//		else if (!_map[floorY - 1][floorX]->solid && _pacman->direction == 0)
	//		{
	//			_pacman->direction = 3;
	//		}
	//		break;
	//	}
	//	default:
	//		break;
	//}
	
//THIS IS ALL A FAILURE OF STORING INPUTS


	////Checking Above Pacman
	//_pacman->inputUp->X = (_pacman->position->X +4.0f);
	//_pacman->inputUp->Y = (_pacman->position->Y - 1.0f);
	//_pacman->inputUp->Width = 32;
	//_pacman->inputUp->Height = 6;
	////Checking Below Pacman
	//_pacman->inputDown->X = (_pacman->position->X + 4.0f);
	//_pacman->inputDown->Y = (_pacman->position->Y + 32.0f);
	//_pacman->inputDown->Width = 32;
	//_pacman->inputDown->Height = 6;
	////Check Right of Pacman
	//_pacman->inputRight->X = (_pacman->position->X + 32.0f);
	//_pacman->inputRight->Y = (_pacman->position->Y +4.0f);
	//_pacman->inputRight->Width = 6;
	//_pacman->inputRight->Height = 32;
	////Check Left of Pacman
	//_pacman->inputLeft->X = (_pacman->position->X - 1.0f);
	//_pacman->inputLeft->Y = (_pacman->position->Y +4.0f);
	//_pacman->inputLeft->Width = 6;
	//_pacman->inputLeft->Height = 32;

	////Checking for Collisions of the inputs


	//	switch (_pacman->nextDir)
	//	{
	//		cout << _pacman->nextDir << endl;
	//		cout << _pacman->direction << endl;
	//	case 0:
	//	{
	//			_pacman->Pacman32PosX = (_pacman->leftPos->X / 32);
	//			_pacman->Pacman32PosY = (_pacman->leftPos->Y / 32);

	//			//cout << "RIGHT : " << _pacman->direction << endl;
	//			if (!_map[_pacman->Pacman32PosX + 1][_pacman->Pacman32PosY]->solid && _pacman->direction != 0)
	//			{
	//				for (int y = 0; y < mapHeight; y++)
	//				{
	//					for (int x = 0; x < mapWidth; x++)
	//					{
	//						if (_map[x][y]->solid)
	//						{
	//							float wallRightX = _map[x][y]->position->X;
	//							float wallRightY = _map[x][y]->position->Y;
	//							int wallRightWidth = _map[x][y]->sourceRect->Width;
	//							int wallRightHeight = _map[x][y]->sourceRect->Height;;
	//							Rect wallRectRight = Rect(wallRightX, wallRightY, wallRightWidth, wallRightHeight);

	//							if (!_pacman->inputRight->Intersects(Rect(wallRectRight)) && _pacman->direction != 0)
	//							{
	//								_pacman->direction = 0;
	//								_pacman->nextDir = 4;
	//								break;
	//							}
	//							else if (_pacman->inputRight->Intersects(Rect(wallRectRight)))
	//							{
	//								cout << "Something in the way right" << endl;
	//								/*cout << _map[_pacman->Pacman32PosX][_pacman->Pacman32PosY]->solid << endl;*/
	//							}
	//						}
	//					}
	//				}


	//			}
	//		
	//		break;
	//	}
	//	case 1:
	//	{
	//			_pacman->Pacman32PosX = (_pacman->topPos->X / 32);
	//			_pacman->Pacman32PosY = (_pacman->topPos->Y / 32);

	//			//cout << "DOWN : " << _pacman->direction << endl;
	//			if (!_map[_pacman->Pacman32PosX][_pacman->Pacman32PosY + 1]->solid && _pacman->direction != 1)
	//			{
	//				for (int y = 0; y < mapHeight; y++)
	//				{
	//					for (int x = 0; x < mapWidth; x++)
	//					{
	//						float wallDownX = _map[x][y]->position->X;
	//						float wallDownY = _map[x][y]->position->Y;
	//						int wallDownWidth = _map[x][y]->sourceRect->Width;
	//						int wallDownHeight = _map[x][y]->sourceRect->Height;;
	//						Rect wallRectDown = Rect(wallDownX, wallDownY, wallDownWidth, wallDownHeight);

	//						if (!_pacman->inputDown->Intersects(Rect(wallRectDown)) && _pacman->direction != 1)
	//						{
	//							_pacman->direction = 1;
	//							_pacman->nextDir = 4;
	//							break;
	//						}
	//						else if (_pacman->inputRight->Intersects(Rect(wallRectDown)))
	//						{
	//							cout << "Something in the way down" << endl;
	//						}
	//					}
	//				}


	//				break;
	//			}

	//		
	//		break;
	//	}
	//	case 2:
	//	{
	//			_pacman->Pacman32PosX = (_pacman->rightPos->X / 32);
	//			_pacman->Pacman32PosY = (_pacman->rightPos->Y / 32);

	//			//cout << "LEFT : " << _pacman->direction << endl;
	//			if (!_map[_pacman->Pacman32PosX - 1][_pacman->Pacman32PosY]->solid && _pacman->direction != 2)
	//			{
	//				for (int y = 0; y < mapHeight; y++)
	//				{
	//					for (int x = 0; x < mapWidth; x++)
	//					{
	//						if (_map[x][y]->solid)
	//						{
	//							float wallLeftX = _map[x][y]->position->X;
	//							float wallLeftY = _map[x][y]->position->Y;
	//							int wallLeftWidth = _map[x][y]->sourceRect->Width;
	//							int wallLeftHeight = _map[x][y]->sourceRect->Height;;
	//							Rect wallRectLeft = Rect(wallLeftX, wallLeftY, wallLeftWidth, wallLeftHeight);

	//							if (!_pacman->inputLeft->Intersects(Rect(wallRectLeft)) && _pacman->direction != 2)
	//							{
	//								_pacman->direction = 2;
	//								_pacman->nextDir = 4;
	//								break;
	//							}
	//							else if (_pacman->inputRight->Intersects(Rect(wallRectLeft)))
	//							{
	//								cout << "Something in the way left" << endl;
	//							}
	//						}
	//					}
	//				}
	//				break;
	//			}
	//			
	//		
	//		break;
	//	}
	//	case 3:
	//	{
	//		/*if (_map[_pacman->Pacman32PosX][_pacman->Pacman32PosY - 1]->solid == true)
	//		{*/
	//			_pacman->Pacman32PosX = (_pacman->bottomPos->X / 32);
	//			_pacman->Pacman32PosY = (_pacman->bottomPos->Y / 32);

	//			if (!_map[_pacman->Pacman32PosX][_pacman->Pacman32PosY - 1]->solid && _pacman->direction != 1)
	//			{
	//				for (int y = 0; y < mapHeight; y++)
	//				{
	//					for (int x = 0; x < mapWidth; x++)
	//					{
	//						if (_map[x][y]->solid)
	//						{
	//							float wallUpX = _map[x][y]->position->X;
	//							float wallUpY = _map[x][y]->position->Y;
	//							int wallUpWidth = _map[x][y]->sourceRect->Width;
	//							int wallUpHeight = _map[x][y]->sourceRect->Height;;
	//							Rect wallRectUp = Rect(wallUpX, wallUpY, wallUpWidth, wallUpHeight);
	//							if (!_pacman->inputUp->Intersects(Rect(wallRectUp)) && _pacman->direction != 3)
	//							{
	//								_pacman->direction = 3;
	//								_pacman->nextDir = 4;
	//								break;
	//							}
	//							else if (_pacman->inputRight->Intersects(Rect(wallRectUp)))
	//							{
	//								cout << "Something in the way up" << endl;
	//							}
	//						}
	//					}
	//				}

	//				break;
	//			}
	//			
	//		//}
	//		break;
	//	}
	//	default:
	//		break;
	//	}
	//Checks the direction pacman is facing and continues moving in that direction
	if (_pacman->direction == 0)
	{
		//Moves Pacman across X axis right
		_pacman->position->X += _pacman->speed * elapsedTime; 
	}
	else if (_pacman->direction == 2)
	{
		//Moves Pacman across X axis left
		_pacman->position->X -= _pacman->speed * elapsedTime; 
	}
	else if (_pacman->direction == 3)
	{
		//Moves Pacman across Y axis up
		_pacman->position->Y -= _pacman->speed * elapsedTime; 
	}
	else if (_pacman->direction == 1)
	{
		//Moves Pacman across Y axis down
		_pacman->position->Y += _pacman->speed * elapsedTime; 
	}
}

void Pacman::CheckPaused(Input::KeyboardState* keyboardState)
{
	
	if (!_GameStarted)
	{
		if (keyboardState->IsKeyDown(Input::Keys::SPACE))
			_GameStarted = true;
	}
	//Gets inputs to check if the game will be paused
	if (!_GameStarted || _paused || _win)
	{
		if (keyboardState->IsKeyUp(Input::Keys::Q))
		{
			_qKeyDown = false;
		}
		if (keyboardState->IsKeyDown(Input::Keys::Q))
		{
			_qKeyDown = true;
			if (_selectedTileset == 0)
				_selectedTileset = 2;
			else
				_selectedTileset--;
			recentChanged = true;
			checkTileSet();
			this_thread::sleep_for(1s);
		}

		if (keyboardState->IsKeyUp(Input::Keys::E))
		{
			_eKeyDown = false;
		}
		if (keyboardState->IsKeyDown(Input::Keys::E))
		{
			_eKeyDown = true;
			if (_selectedTileset == 2)
				_selectedTileset = 0;
			else
				_selectedTileset++;
			recentChanged = true;
			checkTileSet();
			this_thread::sleep_for(1s);

		}
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;
	}
	//Makes sure that the pause doesn't flicker on and off
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}
	//Checks if the game has started yet

}

void Pacman::CheckViewportCollision()
{
	//checks if pacman is off the screen
	if (_pacman->position->X > Graphics::GetViewportWidth())
	{
		//pacman hits wall and will loop around
		_pacman->position->X = 0;
	}
	if (_pacman->position->X + _pacman->sourceRect->Width < (0 + _pacman->sourceRect->Width))
	{
		//pacman hits wall and will loop around
		_pacman->position->X = 1024 - _pacman->sourceRect->Width;
	}
	if (_pacman->position->Y > Graphics::GetViewportHeight())
	{
		//pacman hits wall and will loop around
		_pacman->position->Y = 0 - _pacman->sourceRect->Width;
	}
	if (_pacman->position->Y + _pacman->sourceRect->Height < (0 + _pacman->sourceRect->Height))
	{
		//pacman hits wall and will loop around
		_pacman->position->Y = 768 - _pacman->sourceRect->Width;
	}
}


void Pacman::UpdatePacman(int elapsedTime)
{
	//Creates the frame time and sets it to the current time elapsed
	_pacman->currentFrameTime += elapsedTime;

	//Checks pacmans current frame time against the assigned frame time
	if (_pacman->currentFrameTime > _pacman->frameTime)
	{
		//increments the pacman frame time
		_pacman->frame++;

		//Checks if the frame is 2 or more to update the animation
		switch (_selectedTileset)
		{
		case 0:
		{
			if (_pacman->frame >= 2)
				_pacman->frame = 0;
			_pacman->currentFrameTime = 0;
			break;
		}
		case 1:
		{
			if (_pacman->frame >= 4)
				_pacman->frame = 0;
			_pacman->currentFrameTime = 0;
			break;
		}
		case 2:
		{
			if (_pacman->frame >= 8)
				_pacman->frame = 0;
			_pacman->currentFrameTime = 0;
			break;
		}
		}

	}
	//Updates the sprite depending on the assigned pacman direction and frame
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
}


void Pacman::UpdateMunchie(Consumable* _munchie, int elapsedTime)
{
		//Updates the frame time as time goes on
		_munchie->currentFrameTime += elapsedTime;
		//Checks if the frame time exceeds the current assigned munchie frame time
		if (_munchie->currentFrameTime >_munchie->_frameTime)
		{
			//updates the munchie Frame Count
			_munchie->frameCount++;
			//Checks if the frame count is 2 or above
			if (_munchie->frameCount >= 2)
				_munchie->frameCount = 0;
			//resets the frame count to 0
			_munchie->currentFrameTime = 0;
		}
		//Updates the munchie sprite by moving the source rectangle in the image
		_munchie->rect->Y = _munchie->rect->Height * _munchie->frameCount;
}
//
//void Pacman::UpdateCherry(int elapsedTime)
//{
//	_cherry->currentFrameTime += elapsedTime;
//
//	if (_cherry->currentFrameTime > _cherry->_frameTime)
//	{
//		_cherry->frameCount++;
//
//		if (_cherry->frameCount >= 2)
//			_cherry->frameCount = 0;
//
//		_cherry->currentFrameTime = 0;
//	}
//	_cherry->rect->Y = _cherry->rect->Height * _cherry->frameCount;
//}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	//Creates the frame time and sets it to the current time elapsed
	ghost->_frameTime += elapsedTime;

	//Checks pacmans current frame time against the assigned frame time
	if (ghost->currentFrameTime > ghost->_frameTime)
	{
		//increments the ghost frame time
		ghost->frameCount++;

		//Checks if the frame is 2 or more to update the animation
		if (ghost->frameCount >= 2)
			ghost->frameCount = 0;
		ghost->currentFrameTime = 0;


	}
	//Updates the sprite depending on the assigned pacman direction and frame
	ghost->sourceRect->Y = ghost->sourceRect->Height * ghost->direction;
	ghost->sourceRect->X = ghost->sourceRect->Width * ghost->frameCount;
	if (!ghost->spooked)
	{
		if (ghost->direction == 0)//moves right
		{
			ghost->position->X += ghost->speed * elapsedTime;
		}
		else if (ghost->direction == 1)//moves left
		{
			ghost->position->X -= ghost->speed * elapsedTime;
		}
		else if ((ghost->direction == 2))//moves left
		{
			ghost->position->Y -= ghost->speed * elapsedTime;
		}
		else if ((ghost->direction == 3))//moves left
		{
			ghost->position->Y += ghost->speed * elapsedTime;
		}
	}
	//if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth()) // hit the right edge
	//{
	//	ghost->direction = 1; //change direction
	//}
	//else if (ghost->position->X <= 0) //hits left edge
	//{
	//	ghost->direction = 0; //change direction
	//}

}

void Pacman::CheckGhostCollisions()
{
	// Local Variables
	int i = 0;
	int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->position->X;
	int left2 = 0;
	int right1 = _pacman->position->X + _pacman->sourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 =_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 =_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;
		if (!_pacman->pellet) 
		{
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2)
				&& (left1 < right2))
			{
				Audio::Play(_lifeLost);
				_pacman->lives -= 1;
				_pacman->position->X = 446.0f;
				_pacman->position->Y = 353.0f;

				if (_pacman->lives == 0)
				{
					Audio::Play(_gameOverSound);
					_pacman->dead = true;

				}
			}
		}
		else if (_pacman->pellet)
		{
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_ghosts[i]->position->X = 1000.0f;
				_score->_scoreInt += 100;
				Audio::Play(_ghostDed);
			}
		}
	}
}

void Pacman::Level()
{
	_level->_score = 0;
	_level->_allMunchiesEaten = false;
	_level->_player = nullptr;
	_level->_timeRemaining = 60000;

	char ch;

	int width;
	fstream stream("Levels/map.txt", fstream::in);

	char mapInput[mapWidth][mapHeight];
	for (int y = 0; y < mapHeight; y++)
	{
		for (int x = 0; x < mapWidth+1; x++)
		{
			char ch = stream.get();
			//if (ch == '\n')
			//{
			//	stream.ignore();
			//}
			if(ch != '\n')
			{
				mapInput[x][y] = ch;
			}
			
		}
	}


	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 28; x++)
		{
			_map[x][y]->texture = new Texture2D;
			switch (mapInput[x][y])
			{
				case '-':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallHoriz.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '.':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/empty.png", true);
					_map[x][y]->solid = false;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					_munchie[munchieCount]->texture = new Texture2D();
					_munchie[munchieCount]->texture->Load("Textures/Tileset1/MunchieSpriteSheett.png", true);
					_munchie[munchieCount]->position = new Vector2(x * 32, y * 32);
					_munchie[munchieCount]->rect = new Rect(0.0f, 0.0f, 32, 32);
					munchieCount++;
					_map[x][y]->solid = false;
					break;
				}
				case 'e':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/empty.png", true);
					_map[x][y]->solid = false;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case 'f':
					_map[x][y]->texture->Load("Textures/Tileset1/empty.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				case'G':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/empty.png", true);
					_map[x][y]->solid = false;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					_ghosts[ghostCount]->texture = new Texture2D();
					_ghosts[ghostCount]->position = new Vector2(((x * 32.0f) + 6), ((y * 32.0f) + 6));
					_ghosts[ghostCount]->spawn = new Vector2(x * 32.0f, y * 32.0f);
					_ghosts[ghostCount]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
					switch (ghostCount)
					{
					case 0:
					{
						_ghosts[ghostCount]->texture->Load("Textures/Tileset1/GhostBlueSpriteSheet.png", true);
						break;
					}
					case 1:
					{
						_ghosts[ghostCount]->texture->Load("Textures/Tileset1/redGhostSprite.png", true);
						break;
					}
					case 2:
					{
						_ghosts[ghostCount]->texture->Load("Textures/Tileset1/pinkGhostSpriteSheet.png", true);
						break;
					}
					case 3:
					{
						_ghosts[ghostCount]->texture->Load("Textures/Tileset1/orangeGhostSpriteSheet.png", true);
						break;
					}
					}
					ghostCount++;
				}
				case '8':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/empty.png", true);
					_map[x][y]->solid = false;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					_powerPellet[powerPellet]->texture = new Texture2D();
					_powerPellet[powerPellet]->texture->Load("Textures/Tileset1/powerPelletSprite.png", true);
					_powerPellet[powerPellet]->position = new Vector2(x * 32, y * 32);
					_powerPellet[powerPellet]->rect = new Rect(0.0f, 0.0f, 32, 32);
					powerPellet++;
					break;
				}
				case 'l':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallUp.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '<':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallBL.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '>':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallBR.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '6':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallTRight.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case 'r':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallTR.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '^':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallTL.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '7':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallTLeft.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case '[':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallHorLeft.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case ']':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallHorRight.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case 'n':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallUpEnd.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case 'u':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallDownEnd.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
				case 'T':
				{
					_map[x][y]->texture->Load("Textures/Tileset1/wallTDown.png", true);
					_map[x][y]->solid = true;
					_map[x][y]->position = new Vector2(x * 32.0f, y * 32.0f);
					_map[x][y]->sourceRect = new Rect(0.0f, 0.0f, 32, 32);
					break;
				}
			}
		}
	}
}

void Pacman::CheckWallCollision(int elapsedTime)
{
	//char mapIn[mapWidth][mapHeight];
	//char ch;

	//fstream mapFile("Levels\map.txt", fstream::in);
	//for (int y = 0; y < mapHeight; y++)
	//{
	//	for (int x = 0; x < mapWidth; x++)
	//	{
	//		char ch = mapFile.get();
	//		if (ch != '\n')
	//			mapIn[x][y] = ch;
	//	}
	//}

	_pacman->collider->X = (_pacman->position->X + 6.0f) ;
	_pacman->collider->Y = (_pacman->position->Y + 6.0f);
	_pacman->collider->Width = (_pacman->sourceRect->Width - 10);
	_pacman->collider->Height = (_pacman->sourceRect->Height - 10);


	for (int y = 0; y < mapHeight; y++)
	{
		for (int x = 0; x < mapWidth; x++)
		{
			float wallNearX = _map[x][y]->position->X;
			float wallNearY = _map[x][y]->position->Y;
			int wallWidth = _map[x][y]->sourceRect->Width;
			int wallHeight = _map[x][y]->sourceRect->Height;
			Rect wallRect = Rect(wallNearX, wallNearY, wallWidth, wallHeight);

			if (_map[x][y]->solid == true)
			{
				if (_pacman->collider->Intersects(Rect(wallRect)))
				{
					if (_pacman->direction == 0)
					{
						_pacman->position->X = _pacman->position->X - _pacman->speed * elapsedTime;
					}
					if (_pacman->direction == 2)
					{
						_pacman->position->X = _pacman->position->X + _pacman->speed * elapsedTime;
					}
					if (_pacman->direction == 3)
					{
						_pacman->position->Y = _pacman->position->Y + _pacman->speed * elapsedTime;
					}
					if (_pacman->direction == 1)
					{
						_pacman->position->Y = _pacman->position->Y - _pacman->speed * elapsedTime;
					}
				}

				
			}
		}
	}
}


void Pacman::CheckMunchieCollisions()
{
	_pacman->collider->X = _pacman->position->X;
	_pacman->collider->Y = _pacman->position->Y;
	_pacman->collider->Width = (_pacman->sourceRect->Width - 6);
	_pacman->collider->Height = (_pacman->sourceRect->Height - 6);

	for (int i = 0; i < munchieCount; i++)
	{
		float munchX = _munchie[i]->position->X;
		float munchY = _munchie[i]->position->Y;
		int munchWid = _munchie[i]->rect->Width;
		int munchHeight = _munchie[i]->rect->Height;
		Rect munchieRect = Rect(munchX, munchY, munchWid, munchHeight);

		if (_pacman->collider->Intersects(Rect(munchieRect)))
		{
			_munchie[i]->position->X = 1000.0f;
			_score->_scoreInt += 10;
			Audio::Play(_pop);
			_score->munchiesEaten++;
		}
	}
}

//bool Pacman::OnUserCreate()
//{
//	// Create a 2D array of nodes - this is for convenience of rendering and construction
//	// and is not required for the algorithm to work - the nodes could be placed anywhere
//	// in any space, in multiple dimensions...
//	nodes[mapWidth][mapHeight] = new sNode;
//	for (int x = 0; x < mapWidth; x++)
//		for (int y = 0; y < mapHeight; y++)
//		{
//			nodes[x][y]->x = x; // ...because we give each node its own coordinates
//			nodes[x][y]->y = y;
//			if (_map[x][y]->solid == true)
//			{
//				nodes[x][y]->bObstacle = true;
//			}
//			else
//			{
//				nodes[x][y]->bObstacle = false;
//			}
//			nodes[x][y]->parent = nullptr;
//			nodes[x][y]->bVisited = false;
//		}
//
//	// Create connections - in this case nodes are on a regular grid
//	//for (int x = 0; x < mapWidth; x++)
//	//	for (int y = 0; y < mapHeight; y++)
//	//	{
//	//		if (y > 0)
//	//			nodes[x][y]->vecNeighbours1[y-1][x];
//	//		if (y < mapHeight - 1)
//	//			nodes[x][y]->vecNeighbours2 = nodes[y + 1][x + 0]);
//	//		if (x > 0)
//	//			nodes[x][y]->vecNeighbour3.push_back(&nodes[y + 0][x - 1]);
//	//		if (x < mapWidth - 1)
//	//			nodes[x][y]->vecNeighbour4.push_back(&nodes[y + 0][x + 1]);
//
//	//	}
//
//	return true;
//}

void Pacman::UpdatePellet(Consumable* pellet, int elapsedTime)
{
	//Updates the frame time as time goes on
	pellet->currentFrameTime += elapsedTime;
	//Checks if the frame time exceeds the current assigned munchie frame time
	if (pellet->currentFrameTime > pellet->_frameTime)
	{
		//updates the munchie Frame Count
		pellet->frameCount++;
		//Checks if the frame count is 2 or above
		if (pellet->frameCount >= 2)
			pellet->frameCount = 0;
		//resets the frame count to 0
		pellet->currentFrameTime = 0;
	}
	//Updates the munchie sprite by moving the source rectangle in the image
	pellet->rect->Y = pellet->rect->Height * pellet->frameCount;
}
void Pacman::CheckGhostWall(MovingEnemy* ghost, int elapsedTime)
{
	//char mapInput[mapHeight][mapWidth];
	//char ch;
	//fstream fin("Map4.txt", fstream::in);
	//for (int y = 0; y < 29; y++) {
	//	for (int x = 0; x < 29; x++) {
	//		char ch = fin.get();
	//		if (ch != '\n') mapInput[y][x] = ch;
	//	}
	//}

	ghost->collider->X = ghost->position->X;
	ghost->collider->Y = ghost->position->Y;
	ghost->collider->Height = ghost->sourceRect->Height;
	ghost->collider->Width = ghost->sourceRect->Width;

	float pacX = _pacman->position->X;
	float pacY = _pacman->position->Y;
	int pacW = _pacman->sourceRect->Width;
	int pacH = _pacman->sourceRect->Height;
	Rect currentPacRect = Rect(pacX, pacY, pacW, pacH);
	for (int y = 0; y < mapHeight; y++)
	{
		for (int x = 0; x < mapWidth ; x++)
		{
			float currentWallX = _map[x][y]->position->X;
			float currentWally = _map[x][y]->position->Y;
			int currentWallw = _map[x][y]->sourceRect->Width;
			int currentWallh = _map[x][y]->sourceRect->Height;
			Rect currentWallRect = Rect(currentWallX, currentWally, (currentWallw), currentWallh);


			if (_map[x][y]->solid)
			{
				if (ghost->collider->Intersects(Rect(currentWallRect)))
				{
					
					if (ghost->direction == 0)
					{
						ghost->position->X -= ghost->speed * elapsedTime;
						ghost->direction = rand() % (3 - 2 + 1) + 2;
						
					}
					else if (ghost->direction == 1)
					{
						ghost->position->X += ghost->speed * elapsedTime;
						ghost->direction = rand() % (3 - 2 + 1) + 2;
						
					}
					else if (ghost->direction == 2) {
						ghost->position->Y += ghost->speed * elapsedTime;
						ghost->direction = rand() % 2;
						
					}
					else if (ghost->direction == 3) {
						ghost->position->Y -= ghost->speed * elapsedTime;
						ghost->direction = rand() % 2;
						
					}
				}
			}
		}
	}
}


void Pacman::CheckPelletCollision()
{
	_pacman->collider->X = _pacman->position->X;
	_pacman->collider->Y = _pacman->position->Y;
	_pacman->collider->Width = _pacman->sourceRect->Width;
	_pacman->collider->Height = _pacman->sourceRect->Height;

	for (int i = 0; i < powerPellet; i++)
	{
		float currentPelletX = _powerPellet[i]->position->X;
		float currentPelletY = _powerPellet[i]->position->Y;
		int currentPelletW = _powerPellet[i]->rect->Width;
		int currentPelletH = _powerPellet[i]->rect->Height;
		Rect currentPelletRect = Rect(currentPelletX, currentPelletY, (currentPelletW), currentPelletH);

		if (_pacman->collider->Intersects(Rect(currentPelletRect)))
		{
			_powerPellet[i]->position->X = 1000.0f;
			Audio::Play(_powerup);
			_pacman->pellet = true;
			ResetTimer();
		}
	}
}

void Pacman::CheckTimer(double time)
{
	_timer->storedTime -= time;
}

void Pacman::ResetTimer()
{
	_timer->storedTime = 500;
}

void Pacman::RespawnGhosts()
{
	for (int i = 0; i < 4; i++)
	{
		_ghosts[i]->position->X = _ghosts[i]->spawn->X;
		_ghosts[i]->position->Y = _ghosts[i]->spawn->Y;
	}
}

void Pacman::CheckWin()
{
	if (_score->munchiesEaten == 303)
	{
		_win = true;
	}
}

void Pacman::checkTileSet()
{
	if (_paused || !_GameStarted)
	{

		
		switch (_selectedTileset)
		{
			case 0:
			{
				_pacman->texture->Load("Textures/TileSet1/Pacman.tga", false);
				for (int i = 0; i < munchieCount; i++)
				{
					_munchie[i]->texture->Load("Textures/TileSet1/MunchieSpriteSheett.png", false);
				}
				for (int i = 0; i < powerPellet; i++)
				{
					_powerPellet[i]->texture->Load("Textures/TileSet1/powerPelletSprite.png", false);
				}
				for (int i = 0; i < ghostCount; i++)
				{
					switch (i)
					{
					case 0:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/GhostBlueSpriteSheet.png", true);
						break;
					}
					case 1:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/redGhostSprite.png", true);
						break;
					}
					case 2:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/pinkGhostSpriteSheet.png", true);
						break;
					}
					case 3:
					{
						_ghosts[i]->texture->Load("Textures/TileSet1/orangeGhostSpriteSheet.png", true);
						break;
					}
					}
				}

				if (recentChanged)
				{
					_pop->Load("Sounds/pop.wav");
					Audio::Stop(_backgroundSong);
					_backgroundSong->Load("Sounds/pacman.wav");
					Audio::Play(_backgroundSong);
					recentChanged = false;
				}
				


				break;

					
			
			}
			case 1:
			{
				_pacman->texture->Load("Textures/TileSet2/LeggySpriteSheet.png", false);
				for (int i = 0; i < munchieCount; i++)
				{
					_munchie[i]->texture->Load("Textures/TileSet2/munchiePill.png", false);
				}
				for (int i = 0; i < powerPellet; i++)
				{
					_powerPellet[i]->texture->Load("Textures/TileSet2/biggerMunch.png", false);
				}
				for (int i = 0; i < ghostCount; i++)
				{
					switch (i)
					{
					case 0:
					{
						_ghosts[i]->texture->Load("Textures/TileSet2/blueFish.png", true);
						break;
					}
					case 1:
					{
						_ghosts[i]->texture->Load("Textures/TileSet2/redFish.png", true);
						break;
					}
					case 2:
					{
						_ghosts[i]->texture->Load("Textures/TileSet2/pinkFish.png", true);
						break;
					}
					case 3:
					{
						_ghosts[i]->texture->Load("Textures/TileSet2/orangeFish.png", true);
						break;
					}
					}
				}
					if (recentChanged)
					{
						_pop->Load("Sounds/nom.wav");
						Audio::Stop(_backgroundSong);
						_backgroundSong->Load("Sounds/prettyWoman.wav");
						Audio::Play(_backgroundSong);
						recentChanged = false;
					}

					//Audio::Stop(_backgroundSong);

					//_backgroundSong->Load("Sounds/prettyWoman.wav");
					//Audio::Play(_backgroundSong);

			
				break;
			}
			case 2:
			{
				_pacman->texture->Load("Textures/TileSet3/rollMan.png", false);
				for (int i = 0; i < munchieCount; i++)
				{
					_munchie[i]->texture->Load("Textures/TileSet3/munchie.png", false);
				}
				for (int i = 0; i < powerPellet; i++)
				{
					_powerPellet[i]->texture->Load("Textures/TileSet3/pellet.png", false);
				}

				//Audio::Stop(_backgroundSong);
				if (recentChanged)
				{
					_pop->Load("Sounds/urp.wav");
					Audio::Stop(_backgroundSong);
					_backgroundSong->Load("Sounds/celebrate.wav");
					Audio::Play(_backgroundSong);
					recentChanged = false;
				}
				//Audio::Play(_backgroundSong);
			
				break;
			}
		}
	}
}