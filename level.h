#ifndef LEVEL_H
#define LEVEL_H

int jamc_level_preparation(int argc, char *argv[]);

int jamc_level_convertation(int argc, char *argv[]);

struct MYLEVELVERTEX
{
    float pos[3];
    float normal[3];
	float unknown[1]; //I dont know what is it.
    float tc[2];
};

#endif // LEVEL_H