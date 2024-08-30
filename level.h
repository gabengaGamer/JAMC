#ifndef LEVEL_H
#define LEVEL_H

#define CLUSTER_TIMEOUT 10 //Time (in seconds) allocated to process each mesh cluster.

//=============================================================================
// PROTOTYPING
//=============================================================================

int LevelBatchProcess();

//=============================================================================

void GetLVIndexOffset();

//=============================================================================

void GetLVIndexCount();

//=============================================================================

void GetLVVertexOffset();

//=============================================================================

void GetLVVertexCount();

//=============================================================================

int jamc_level_convertation();

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
    
    extern int index_length;
    
    extern int amount_index_clusters;
    
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