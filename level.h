#ifndef LEVEL_H
#define LEVEL_H

//=============================================================================
// PROTOTYPING
//=============================================================================

int LevelBatchProcess();

//=============================================================================

void GetLevelTable();

//=============================================================================

void GetLVIndexOffset();

//=============================================================================

void GetLVIndexCount();

//=============================================================================

void GetLVVertexOffset();

//=============================================================================

void GetLVVertexCount();

//=============================================================================

int ProcessLVMesh();

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
    
    extern int mesh_count;
    
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

//=============================================================================
// ENV DATABASE
//=============================================================================

/////////////////////////////JUDGE DREDD VS DEATH//////////////////////////////

//------------Campagin------------//

#define SP_00      0x000000D2 // Training.
#define SP_01      0x000009C9 // Chapter 1 Halls of Justice.
#define SP_02      0x000008BB // Chapter 2 Nixon Penitentiary.
#define SP_02End   0x000000AE // Chapter 2 Nixon Penitentiary - End.
#define SP_03      0x000006AB // Chapter 3 The Docks.
#define SP_04      0x00000871 // Chapter 4 Ryder Mega-Mall.
#define SP_05      0x0000051D // Chapter 5 Icarus Labs.
#define SP_06      0x00000846 // Chapter 6 Mean Streets.
#define SP_07      0x000006A4 // Chapter 7 Clooney Hospital.
#define SP_08      0x00000325 // Chapter 8 Smokatorium.
#define SP_09      0x00000933 // Chapter 9 Resyk.
#define SP_10      0x000004B6 // Chapter 10 Undercity.
#define SP_11      0x000000F3 // Chapter 11 Deadworld.
#define Epilogue   0x00000396 // Chapter 11 Deadworld - End.
#define SP_MENU    0x0000008A // Menu.

//------------Multiplayer------------//

#define Basement   0x00000147
#define DM_01      0x0//0x0000008A
#define DM_Block   0x00000230
#define DM_Ects    0x000001B9
#define DM_Guns    0x000000B9
#define DM_Resyk   0x00000169
#define DM_Riot    0x000003B9
#define DM_Sky     0x000000BC
#define DM_Train   0x0000025F
#define DM_Umpty   0x000000F2
#define DM_Wall    0x00000252

//------------Arcades------------//

#define AR_01   0x00
#define AR_02   0x01
#define AR_03   0x02
#define AR_04   0x03
#define AR_05   0x04
#define AR_06   0x05
#define AR_07   0x06
#define AR_08   0x07
#define AR_09   0x08
#define AR_10   0x09
#define AR_11   0x10
#define AR_12   0x11

#endif // LEVEL_H