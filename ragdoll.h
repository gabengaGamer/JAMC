#ifndef RAGDOLL_H
#define RAGDOLL_H

int jamc_ragdoll_convertation(int argc, char *argv[]);

struct MYRAGDOLLVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
	float bone[8]; //Using bones is not possible, because I'm only working with OBJ for now. But I can at least convert the mesh.
};

#endif // RAGDOLL_H