#pragma once
#include "Point2D.h"

class Room
{
public:
	// Constructors
	Room();
	Room(int row, int col, int width, int height);

	// Destructor
	~Room();

	// Getters
	Point2D getLeftTop();
	Point2D getRightBottom();
	Point2D getCenter();

	bool IsOverlap(Room* other);
	void toString();

private:
	Point2D left_top_, right_bottom_, center_;
	int width_, height_;
};

