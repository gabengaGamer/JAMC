#ifndef LEVEL_H
#define LEVEL_H

int jamc_level_convertation(int argc, char *argv[]);

struct MYLEVELVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
};

#endif // LEVEL_H