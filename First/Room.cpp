#include <math.h>
#include <iostream>
#include "Room.h"

using namespace std;

/* Constructors */
Room::Room()
{
}

Room::Room(int row, int col, int width, int height) {
	center_.setRow(row);
	center_.setCol(col);
	left_top_.setRow(row - height / 2);
	left_top_.setCol(col - width / 2);
	right_bottom_.setRow(row + height / 2);
	right_bottom_.setCol(col + width / 2);
	width_ = width;
	height_ = height;

}

/* Destructors */
Room::~Room()
{
}

/* Getters */
Point2D Room::getLeftTop()
{
	return left_top_;
}
Point2D Room::getRightBottom() {
	return right_bottom_;
}

Point2D Room::getCenter()
{
	return center_;
}

/* Class Functions */
bool Room::IsOverlap(Room* other)
{
	int horiz_dist, vert_dist, vsz, hsz;
	horiz_dist = abs(center_.getCol() - other->center_.getCol());
	vert_dist = abs(center_.getRow() - other->center_.getRow());
	vsz = height_ / 2 + other->height_ / 2;
	hsz = width_ / 2 + other->width_ / 2;

	return horiz_dist <= hsz + 2 && vert_dist <= vsz + 2;
}


void Room::toString()
{
	cout << "CHECK ROOM" << "CENTER-> (" << center_.getRow() << "," << center_.getCol()
		<< "), WIDTH-> " << width_ << ", HEIGHT->" << height_ << endl;
}




