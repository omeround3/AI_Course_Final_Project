#include <iostream>
#include "Grenade.h"
#include "Definitions.h"
using namespace std;

const double PI = 3.14;

// Constructors
Grenade::Grenade()
{
}

Grenade::Grenade(double x, double y)
{
	int i;
	double alpha, teta = 2 * PI / NUM_BULLETS;
	this->x_ = x;
	this->y_ = y;
	for (alpha = 0, i = 0; i < NUM_BULLETS && alpha < 2 * 3.14; alpha += teta, i++)
	{
		bullets_[i] = new Bullet(x, y, alpha);
	}

}

// Descturctor
Grenade::~Grenade()
{
}

// Class functions
void Grenade::Explode()
{
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (this != NULL) {
			bullets_[i]->setIsMoving(true);
		}
		else {
			cout << "There is no bullet!\n";
			return;
		}
	}
}

void Grenade::ShowMe()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets_[i]->ShowMe();
}

void Grenade::Exploding(Cell maze[MSZ][MSZ])
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets_[i]->Move(maze);

}

void Grenade::SimulateExplosion(Cell maze[MSZ][MSZ], double security_map[MSZ][MSZ], double hurt)
{
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		bullets_[i]->setIsMoving(true);
		bullets_[i]->SimulateFire(maze, security_map, hurt);
	}

}
