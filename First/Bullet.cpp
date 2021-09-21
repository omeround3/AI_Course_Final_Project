#include <math.h>
#include "Bullet.h"
#include "GLUT.H"
#include "Definitions.h"

// Constructors
Bullet::Bullet()
{
}

Bullet::Bullet(double x, double y)
{
	double len;
	this->x_ = x;
	this->y_ = y;
	this->dir_angle_ = (rand() % 360) * 3.14 / 180;// in radians
	setDirection(this->dir_angle_);
	isFired_ = false;
}

Bullet::Bullet(double x, double y, double angle)
{
	double len;
	this->x_ = x;
	this->y_ = y;
	this->dir_angle_ = angle;
	setDirection(this->dir_angle_);
	isFired_ = false;
}

// Desctrutor
Bullet::~Bullet()
{
}

// Getters
double Bullet::getX()
{
	return x_;
}

double Bullet::getY()
{
	return y_;
}

bool Bullet::IsMoving()
{
	return isFired_;
}

// Setters
void Bullet::setIsMoving(bool move)
{
	isFired_ = move;
}


void Bullet::setDirection(double angle)
{
	dirx_ = cos(angle);
	diry_ = sin(angle);
}

// Class functions
void Bullet::Move(Cell maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y_ + 1) / 2;
	j = MSZ * (x_ + 1) / 2;

	if (isFired_ && maze[i][j].getIdentity() == SPACE)
	{
		x_ += 0.001 * dirx_;
		y_ += 0.001 * diry_;
	}
}

void Bullet::ShowMe()
{
	glColor3d(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x_ - 0.01, y_);
	glVertex2d(x_, y_ + 0.01);
	glVertex2d(x_ + 0.01, y_);
	glVertex2d(x_, y_ - 0.01);
	glEnd();
}


void Bullet::SimulateFire(Cell maze[MSZ][MSZ], double security_map[MSZ][MSZ], double hurt)
{
	int row, col;
	row = (int)(MSZ * (y_ + 1) / 2);
	col = (int)(MSZ * (x_ + 1) / 2);

	while (maze[row][col].getIdentity() == SPACE)
	{
		security_map[row][col] += hurt;
		x_ += SPEED * dirx_;
		y_ += SPEED * diry_;
		row = MSZ * (y_ + 1) / 2;
		col = MSZ * (x_ + 1) / 2;
	}
	
}
