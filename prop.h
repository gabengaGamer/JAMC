#ifndef PROP_H
#define PROP_H

//=============================================================================
// PROTOTYPING
//=============================================================================

int jamc_prop_convertation();

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

struct MYPROPVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
};

#endif // PROP_H