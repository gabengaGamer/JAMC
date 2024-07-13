#ifndef LEVEL_H
#define LEVEL_H

int jamc_level_convertation(int argc, char *argv[]);

struct MYLEVELVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
	float zxc[1]; //Something is stored here.
};

#endif // LEVEL_H