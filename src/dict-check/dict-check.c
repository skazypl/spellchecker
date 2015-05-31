#include <stdio.h>
#include <stdlib.h>
#include <dictionary.h>
#include <string.h>


int main(int argc, char const *argv[])
{
	bool ddebug = false;
	bool parseInput = true;
	FILE* dictLocation;
	switch (argc)
	{
		case 1:
			dictLocation = fopen(argv[0], "r");
			if (dictLocation == NULL)
			{
				printf("ERR: nie udalo sie otworzyc slownika\n");
				parseInput = false;
			}
			break;
		case 2:
			if(strcmp("-v", argv[0]))
				ddebug = true;
			else
			{
				parseInput = false;
				break;
			}
			dictLocation = fopen(argv[1], "r");
			if (dictLocation == NULL)
			{
				printf("ERR: nie udalo sie otworzyc slownika\n");
				parseInput = false;
			}
			break;

		default:
			break;
	}

	if (parseInput)
	{

	}

	return 0;
}