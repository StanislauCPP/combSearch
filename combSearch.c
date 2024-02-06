#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Subset.h"

int main()
{
	int shape, parScanf;

	parScanf = scanf("%d", &shape);
	if (parScanf != 1)
		abort();

	getCombinationsByShape(shape);

	return 0;
}
