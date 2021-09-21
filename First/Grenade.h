#pragma once
#include "Bullet.h"
#include "Cell.h"

const int NUM_BULLETS = 20;

class Grenade
{
public:
	// Constructors
	Grenade();
	Grenade(double x, double y);

	// Destructors
	~Grenade();

	// Class Functions
	void Explode();
	void ShowMe();
	void Exploding(Cell maze[MSZ][MSZ]);
	void SimulateExplosion(Cell maze[MSZ][MSZ], double security_map[MSZ][MSZ], double hurt);

private:
	double x_, y_;
	Bullet* bullets_[NUM_BULLETS];

};

