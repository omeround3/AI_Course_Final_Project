#pragma once
#include "Cell.h"
#include "Definitions.h"


class Bullet
{
public:
	// Constructors
	Bullet();
	Bullet(double x, double y);
	Bullet(double x, double y, double angle);

	// Destructor 
	~Bullet();

	// Getters
	double getY();
	double getX();
	bool IsMoving();

	// Setters
	void setIsMoving(bool move);
	void setDirection(double angle);

	// Class functions
	void Move(Cell maze[MSZ][MSZ]);
	void ShowMe();
	void SimulateFire(Cell maze[MSZ][MSZ], double security_map[MSZ][MSZ], double hurt);

private:
	double x_, y_;
	double dirx_, diry_;
	double dir_angle_;
	bool isFired_;

};

