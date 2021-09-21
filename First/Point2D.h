#pragma once
#include <string>

class Point2D
{
public:
	// Constructor
	Point2D();
	Point2D(int row, int col);
	Point2D(Point2D* other);
	
	// Destructor
	~Point2D();

	// Getters
	int getRow();
	int getCol();

	// Setters
	void setRow(int r);
	void setCol(int c);

	/// Operators Overloading
	bool operator == (const Point2D &other)
	{
		return other.row_ == row_ && other.col_ == col_;
	}

	// Class Functions
	bool equals(Point2D* other);
	std::string toString();

private:
	int row_, col_;
};

