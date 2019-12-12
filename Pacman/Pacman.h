#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define MUNCHIECOUNT 100
#define GHOSTCOUNT 4
#define mapHeight 30
#define mapWidth 28

// Just need to include main header file
#include "S2D/S2D.h"

#include <vector>
#include <stdlib.h>

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//structure for the player
struct Player
{
	bool dead;
	float speed;
	int currentFrameTime;
	int direction;
	int frame;
	Rect* sourceRect;
	Texture2D* texture;
	Vector2* position;
	int frameTime;
	Rect* collider;
	int lives;
	bool pellet;
	int nextDir;
	//Rect* inputUp;
	//Rect* inputDown;
	//Rect* inputLeft;
	//Rect* inputRight;
	int Pacman32PosX;
	int Pacman32PosY;
	Vector2* centrePos;
	//Vector2* leftPos;
	//Vector2* topPos;
	//Vector2* bottomPos;
	//Vector2* rightPos;
};

//giving munchies rights, putting them in a structure to be called
struct Consumable
{
	int frameCount;
	Rect* rect;
	Texture2D* texture;
	Vector2* position;
	int currentFrameTime;
	int _frameTime;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
	Rect* collider;
	bool spooked;
	int currentFrameTime;
	int _frameTime;
	int frameCount;
	Vector2* spawn;
};

struct Wall
{
	Texture2D* texture; 
	Rect* sourceRect;
	Vector2* position;
	bool solid;
};

struct level
{
	int _score;
	int _timeRemaining;
	bool _allMunchiesEaten;
	Player* _player;
};

struct score
{
	//score
	string _scoreString;
	int _scoreInt;
	int munchiesEaten;
};

struct timer
{
	int storedTime;
};

//struct sNode
//{
//	bool bObstacle = false;			// Is the node an obstruction?
//	bool bVisited = false;			// Have we searched this node before?
//	float fGlobalGoal;				// Distance to goal so far
//	float fLocalGoal;				// Distance to goal if we took the alternative route
//	int x;							// Nodes position in 2D space
//	int y;
//	int vecNeighbours1[mapHeight][mapWidth];	// Connections to neighbours
//	int vecNeighbours2[mapHeight][mapWidth];
//	int vecNeighbours3[mapHeight][mapWidth];
//	int vecNeighbours4[mapHeight][mapWidth];
//	sNode* parent;					// Node connecting to this node that offers shortest parent
//};


// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	// Position for String
	Vector2* _stringPosition;
	Vector2* _scorePosition;

	//int _munchieCurrentFrameTime; 
	Player* _pacman;
	Consumable** _munchie;

	//ints to keep track of
	int munchieCount;
	int powerPellet;
	int directionToMove;
	int ghostCount;
	bool gameOver;
	int floorX;
	int ceilX;
	int floorY;
	int ceilY;
	bool recentChanged;

	MovingEnemy** _ghosts;
	level* _level;
	score* _score;
	Consumable** _powerPellet;
	timer* _timer;

	//wall stuff
	Wall* _map[mapWidth][mapHeight];

	//initialising things
	//sNode* nodes[mapWidth][mapHeight];
	//sNode* nodeStart[mapWidth][mapHeight];
	//sNode* nodeEnd[mapWidth][mapHeight];

	//spawning stuff
	Vector2 _start;
	Vector2 _exit;

	//Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
	bool _qKeyDown;
	bool _eKeyDown;
	bool _GameStarted;

	//Game Over Screen
	Texture2D* _gameOverBackground;
	Rect* _gameOverRectangle;
	Vector2* _gameOverStringPosition;
	bool _gameOver;

	//Win screen
	Texture2D* _winScreen;
	Rect* _winText;
	Vector2* _winPosition;
	bool _win;

	//Main Menu
	Texture2D* _menuScreen;
	Rect* _menuText;
	Vector2* _menuPositon;
	bool _menu;
	int _selectedTileset;

	//Audio
	SoundEffect* _pop;
	SoundEffect* _powerup;
	SoundEffect* _powerupEnd;
	SoundEffect* _lifeLost;
	SoundEffect* _gameOverSound;
	SoundEffect* _winSound;
	SoundEffect* _backgroundSong;
	SoundEffect* _ghostDed;
	

	//Methods to be used in update()
	void Input(int elapsedTime, Input::KeyboardState* state);
	void pausedInput(int elapsedTime, Input::KeyboardState* state);
	void CheckPaused(Input::KeyboardState* state);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Consumable* munchie, int elapsedTime);
	void UpdateCherry(int elapsedTime);
	void CheckGhostCollisions();
	void CheckMunchieCollisions();
	void UpdateGhost(MovingEnemy*, int elapsedTime);
	void CheckWallCollision(int elapsedTime);
	void Level();
	void CheckGhostWall(MovingEnemy* ghost, int elapsedTime);
	void UpdatePellet(Consumable* pellet, int elapsedTime);
	void CheckPelletCollision();
	void ResetTimer();
	void CheckTimer(double time);
	void CheckWin();
	void RespawnGhosts();
	void checkTileSet();
	



public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[], int munchieCount, int powerPellet, int ghostCount);



	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);

};


