#ifndef PROP_H
#define PROP_H

int jamc_prop_convertation(int argc, char *argv[]);

struct MYPROPVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
};

#endif // PROP_H