#pragma once
#include "Cell.h"

/* A Cell comparator: compares by F value */
class CompareCells
{
public:
	// Constructor & Desctuctor
	CompareCells();
	~CompareCells();

	bool operator () (Cell* pn1, Cell* pn2)
	{
		return pn1->getF() > pn2->getF();
	}
};

