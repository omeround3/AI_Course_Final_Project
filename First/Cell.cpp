#include <math.h>
#include "Cell.h"
#include "Definitions.h"

// Constructors
Cell::Cell()
{
	setIdentity(SPACE);
	setSearchIdentity(SPACE);
	setParent(nullptr);
	this->g_ = 0;
	setRoomIndex(-1);
}

Cell::Cell(Point2D& point, Point2D* target, int v, double g, Cell* parent) {
	this->point_ = point;
	this->target_point_ = target;
	setIdentity(v);
	setParent(parent);
	this->g_ = g;
	this->h_ = computeH();
	this->g_val_ = 0;
}

Cell::Cell(Cell* other)
{
	this->point_ = other->getPoint();
	if (other->getParent() == nullptr)
		setParent(nullptr);
	else
		setParent(other->getParent());
	setIdentity(other->getIdentity());
	this->h_ = other->h_;
	this->g_ = other->g_;
	this->f_val_ = other->f_val_;
	this->g_val_ = other->g_val_;
	setSearchIdentity(other->search_identity_);
}

// Desctructor
Cell::~Cell()
{
}


int Cell::getRoomIndex()
{
	return this->room_index_;
}

int Cell::getIdentity()
{
	return cell_identity_;
}

int Cell::getSearchIdentity()
{
	return this->search_identity_;
}

double Cell::getFVal()
{
	return this->f_val_;
}

double Cell::getG()
{
	return g_;
}

double Cell::getGVal()
{
	return this->g_val_;
}

double Cell::computeH()
{
	return sqrt(pow(point_.getRow() - target_point_->getRow(), 2) +
		pow(point_.getCol() - target_point_->getCol(), 2));
}

double Cell::getF()
{
	return g_ + h_;
}

double Cell::getDanger()
{
	return this->danger_;
}

Point2D Cell::getPoint()
{
	return point_;
}

Point2D* Cell::getTarget()
{
	return target_point_;
}

Cell* Cell::getParent()
{
	return parent_;
}


// Setters
void Cell::setRoomIndex(int index)
{
	this->room_index_ = index;
}

void Cell::setIdentity(int identity)
{
	this->cell_identity_ = identity;
}


void Cell::setFVal(double val)
{
	this->f_val_ = val;
}

void Cell::setGVal(double val)
{
	this->g_val_ = val;
}

void Cell::setDanger(double danger)
{
	this->danger_ = danger;
}


void Cell::setSearchIdentity(int identity)
{
	this->search_identity_ = identity;
}

void Cell::setPoint(int x, int y)
{
	this->point_ = Point2D(x, y);
}


void Cell::setParent(Cell* parent)
{
	this->parent_ = parent;
}


// Class functions
bool Cell::Equals(Cell* other)
{
	return this->point_.getRow() == other->point_.getRow() && this->point_.getCol() == other->point_.getCol();
}

bool Cell::Walkable()
{
	return this->cell_identity_ != WALL && this->cell_identity_ != TEAM_1 && this->cell_identity_ != TEAM_2;
}


