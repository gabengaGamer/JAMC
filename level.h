#ifndef LEVEL_H
#define LEVEL_H

int jamc_level_convertation(int argc, char *argv[]);

struct MYLEVELVERTEX
{
    float pos[3];
    float normal[3];
    //float tc[3];
	float model_pos[3]; //Most likely, the coordinates of the model’s location in the world are stored here.
};

#endif // LEVEL_H