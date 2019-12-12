#include "Pacman.h"

//Entry Point for Application
int main(int argc, char* argv[]) {
	int Count = 303;
	int Pellet = 40;
	int Ghost = 4;
	Pacman* game = new Pacman(argc, argv, Count, Pellet, Ghost);

}