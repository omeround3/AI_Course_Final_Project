#pragma once
#include "Cell.h"


/* A Cell comparator: compares by F value */
class CompareCellsF
{
public:
	CompareCellsF();
	~CompareCellsF();

	bool operator () (Cell* pn1, Cell* pn2)
	{
		return pn1->getFVal() > pn2->getFVal();
	}
};

