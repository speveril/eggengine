#include "gen_util.h"

int nextPower(int num)
{
	int r = 1;

	while(r < num)
	{
		r *= 2;
	}

	return r;
}