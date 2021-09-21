#pragma once
#include <vector>
#include "Point2D.h"


class Cell
{
public:
	// Constructors
	Cell();
	Cell(Point2D& point, Point2D* target, int identity, double g, Cell* parent);
	Cell(Cell* other);

	// Destructor
	~Cell();

	// Getters
	int getRoomIndex();
	int getIdentity();
	int getSearchIdentity();
	double getG();
	double getGVal();
	double getFVal();
	double getF();
	double getDanger();
	double computeH();
	Point2D getPoint();
	Point2D* getTarget();
	Cell* getParent();

	// Setters
	void setRoomIndex(int index);
	void setIdentity(int identity);
	void setFVal(double val);
	void setGVal(double val);
	void setDanger(double danger);
	void setSearchIdentity(int identity);
	void setPoint(int x, int y);
	void setParent(Cell* parent);

	// Operator Overloading
	bool operator == (const Cell& other) {
		return point_ == other.point_;
	}

	// Class functions
	bool Equals(Cell* other);
	bool Walkable();

private:
	int cell_identity_, room_index_, search_identity_;
	double h_, g_;
	double danger_, f_val_, g_val_;
	Cell* parent_;
	Point2D point_;
	Point2D* target_point_;
};

