#ifndef LEVEL_H
#define LEVEL_H

//=============================================================================
// PROTOTYPING
//=============================================================================

int jamc_level_preparation(int argc, char *argv[]);

//=============================================================================

int jamc_level_convertation(int argc, char *argv[]);

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

struct MYLEVELVERTEX
{
    float pos[3];
    float normal[3];
	float unknown[1]; //I dont know what is it.
    float tc[2];
};

#endif // LEVEL_H