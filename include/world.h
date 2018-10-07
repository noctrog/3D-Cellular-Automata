#pragma once

using namespace std;

#include <iostream>
#include <fstream>

class World
{
    public:
	World();
	virtual ~World ();

	size_t	      num_beings ();

	virtual void  read_from (ifstream& ifs) = 0;
	virtual void  evolve (size_t maxgen) = 0;

    protected:
	size_t nbeings;
};


