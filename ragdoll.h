#ifndef RAGDOLL_H
#define RAGDOLL_H

//=============================================================================
// PROTOTYPING
//=============================================================================

int jamc_ragdoll_convertation();

//=============================================================================
// EXTERNING
//=============================================================================

    extern const char *input_file;
    extern const char *output_file;

    extern FILE *f_in;
    extern FILE *f_out;

    extern int index_offset;
    extern int index_cnt_offset;
    extern int vert_offset;
    extern int vert_cnt_offset;
    
//=============================================================================
// STRUCTURIZING
//=============================================================================

struct MYRAGDOLLVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
    float bone[8]; //Using bones is not possible, because I'm only working with OBJ for now. But I can at least convert the mesh.
};

#endif // RAGDOLL_H