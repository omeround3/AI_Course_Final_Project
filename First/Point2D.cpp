#include <string>
#include "Point2D.h"

// Constructors
Point2D::Point2D()
{
}

Point2D::Point2D(Point2D* other)
{
	this->col_ = other->col_;
	this->row_ = other->row_;
}

Point2D::Point2D(int row, int col)
{
	this->col_ = row;
	this->row_ = col;
}


Point2D::~Point2D()
{
}

// Getters
int Point2D::getRow()
{
	return row_;
}

int Point2D::getCol() 
{
	return col_;
}

// Setters
void Point2D::setRow(int row) {
	row_ = row;
}

void Point2D::setCol(int col)
{
	col_ = col;
}

std::string Point2D::toString()
{
	std::string str;
	str.append("(");
	str.append(std::to_string(this->col_));
	str.append(";");
	str.append(std::to_string(this->row_));
	str.append(")");
	return str;
}


bool Point2D::equals(Point2D* p)
{
	return this->row_ == p->row_ && this->col_ == p->col_;
}
