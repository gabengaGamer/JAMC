//=============================================================================
//
//  Asura ENV converter.
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "level.h"

//=============================================================================
// STRUCTURIZING
//=============================================================================

    int mesh_begin;

    unsigned vert_cnt;
    unsigned index_cnt;

    struct MYLEVELVERTEX *verts;
    unsigned short *indices;
    
//=============================================================================
// LEVEL PROCESSING
//=============================================================================

void GetLevelTable()
{    
    fseek(f_in, 4, SEEK_SET);
    
    int level_table;
    fread(&level_table, sizeof(int), 1, f_in);
    #ifdef _DEBUG
    printf("Debug: Level table: 0x%lX\n", level_table);
    #endif        
    
    //mesh_begin = level_table * 12; //At the moment, it is not known exactly what this formula should look like, 
                                     //so this part of the code is useless for now.
    
    //fseek(f_in, mesh_begin + 16, SEEK_CUR); //16 is an adjustment, "junk info" ALWAYS starts at 0x18 so we just adjust pointer to this position.
    
    //"junk info" is a useless set of 4 byte values, these values ​​do not affect anything, 
    //however their number is important for constructing the offset.
    
    //Important! 
    //The only correct way to process the Asura Engine level is to open it in HEX and find the end of "junk info" and specify it "fseek" as below.
    
    //fseek(f_in, 0x3930, SEEK_SET);   //ENV 914KB TEST
    //fseek(f_in, 0x93E4, SEEK_SET);   //ENV 9422KB TEST
    //fseek(f_in, 0xC7BC, SEEK_SET);   //ENV 6720KB TEST

    switch (level_table) {
        case SP_00:
            fseek(f_in, 0x1158, SEEK_SET);
            break;
        case SP_01:
            fseek(f_in, 0xE5C8, SEEK_SET);
            break;
        case SP_02:
            fseek(f_in, 0xC7BC, SEEK_SET);
            break;
        case SP_02End:
            fseek(f_in, 0xF9C, SEEK_SET);
            break;
        case SP_03:
            fseek(f_in, 0x93E4, SEEK_SET);
            break;
        case SP_04:
            fseek(f_in, 0xB4D4, SEEK_SET);
            break;
        case SP_05:
            fseek(f_in, 0x6C68, SEEK_SET);
            break;
        case SP_06:
            fseek(f_in, 0xC624, SEEK_SET);
            break;
        case SP_07:
            fseek(f_in, 0x925C, SEEK_SET);
            break;
        case SP_08:
            fseek(f_in, 0x4730, SEEK_SET);
            break;
        case SP_09:
            fseek(f_in, 0xC84C, SEEK_SET);
            break;
        case SP_10:
            fseek(f_in, 0x6AB0, SEEK_SET);
            break;
        case SP_11:
            fseek(f_in, 0x174C, SEEK_SET);
            break;
        case Epilogue:
            fseek(f_in, 0x59A0, SEEK_SET);
            break;
        case SP_MENU:
            fseek(f_in, 0xC60, SEEK_SET);
            break;
        case Basement:
            fseek(f_in, 0x1DD0, SEEK_SET);
            break;
        case DM_01:
            fseek(f_in, 0xBD0, SEEK_SET);
            break;
        case DM_Block:
            fseek(f_in, 0x3028, SEEK_SET);
            break;
        case DM_Ects:
            fseek(f_in, 0x2700, SEEK_SET);
            break;
        case DM_Guns:
            fseek(f_in, 0xEF8, SEEK_SET);
            break;
        case DM_Resyk:
            fseek(f_in, 0x1E2C, SEEK_SET);
            break;
        case DM_Riot:
            fseek(f_in, 0x5308, SEEK_SET);
            break;
        case DM_Sky:
            fseek(f_in, 0x109C, SEEK_SET);
            break;
        case DM_Train:
            fseek(f_in, 0x33BC, SEEK_SET);
            break;
        case DM_Umpty:
            fseek(f_in, 0x1408, SEEK_SET);
            break;
        case DM_Wall:
            fseek(f_in, 0x3930, SEEK_SET);
            break;
        default:
            printf("Warning: Unknown ENV!\n");
            exit(1);
    }    
    LevelBatchProcess();    
}

//=============================================================================

int LevelBatchProcess()
{
    for (int i = 0; i < mesh_count; i++) {             
        #ifdef _DEBUG
        printf("Info: Mesh cluster: %d\n", i+1);
        #endif
        GetLVVertexCount();        
    }
    return FinishProcessing();
}

//=============================================================================

void GetLVVertexCount()    
{   
    vert_cnt_offset = ftell(f_in);
    
    #ifdef _DEBUG
    printf("Debug: Vertex count value starts in: 0x%lX\n", vert_cnt_offset);
    #endif
    
    fread(&vert_cnt, 1, sizeof(vert_cnt), f_in);

    GetLVIndexCount();
}

//=============================================================================

void GetLVIndexCount()
{   
    index_cnt_offset = ftell(f_in);
    
    #ifdef _DEBUG
    printf("Debug: Index count value starts in: 0x%lX\n", index_cnt_offset);
    #endif
    
    fread(&index_cnt, 1, sizeof(index_cnt), f_in);
    
    GetLVVertexOffset();
}

//=============================================================================

void GetLVVertexOffset()
{    
    vert_offset = ftell(f_in);

    #ifdef _DEBUG
    printf("Debug: Vertex offset starts in: 0x%lX\n", vert_offset);
    #endif
    
    verts = (struct MYLEVELVERTEX *)malloc(sizeof(struct MYLEVELVERTEX) * vert_cnt);
    fread(verts, 1, sizeof(struct MYLEVELVERTEX)*vert_cnt, f_in);
    
    GetLVIndexOffset();
}

//=============================================================================

void GetLVIndexOffset()
{    
    index_offset = ftell(f_in);

    #ifdef _DEBUG
    printf("Debug: Index offset starts in: 0x%lX\n", index_offset);
    #endif    
   
    indices = (unsigned short *)malloc(index_cnt * sizeof(unsigned short));
    fread(indices, 1, sizeof(unsigned short)*index_cnt, f_in);
    
    ProcessLVMesh();
}

//=============================================================================
// CONVERTING
//=============================================================================

int ProcessLVMesh()
{
    unsigned i;

    int voffs = 1;
    unsigned short *pi;
    unsigned short v1, v2, v3;
    
    static int subsequence; 
    static int numbering;
    
    fprintf(f_out, "g mesh_%d\n", numbering); //Separating and numbering each mesh.
    fprintf(f_out, "o mesh_%d\n", numbering);
    numbering++;
    
    for(i = 0; i < vert_cnt; i++) {
        verts[i].pos[0] = -verts[i].pos[0]; //Inverting vertices in X axis.
        verts[i].pos[1] = -verts[i].pos[1]; //Inverting vertices in Z axis.
        fprintf(f_out, "v %f %f %f\n", verts[i].pos[0], verts[i].pos[1], verts[i].pos[2]);
    }
    for(i = 0; i < vert_cnt; i++) {
        verts[i].normal[0] = -verts[i].normal[0]; //Inverting X axis normals to sync changes with vertices.
        verts[i].normal[1] = -verts[i].normal[1]; //Inverting Z axis normals to sync changes with vertices.
        fprintf(f_out, "vn %f %f %f\n", verts[i].normal[0], verts[i].normal[1], verts[i].normal[2]); 
    }
    
    for(i = 0; i < vert_cnt; i++) {
        verts[i].tc[1] = 1.0f -verts[i].tc[1]; //Inverting UV to sync with texture maps.
        fprintf(f_out, "vt %f %f\n", verts[i].tc[0], verts[i].tc[1]); 
    }
    
//=============================================================================
// MESH FIX UP
//=============================================================================

//Export triangle strips.
    pi = indices;
    v1 = pi[0]; 
    v2 = pi[1];
    
    for(i = 2; i < index_cnt; i++) {
        v3 = pi[i];

//Skip degenerated faces.
    if(v1 == v2 || v2 == v3 || v3 == v1)
       goto next_face;

//Flip every second face.
    if((i - 2) % 2)
        fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v3+voffs+subsequence, v3+voffs+subsequence, v3+voffs+subsequence, 
        v2+voffs+subsequence, v2+voffs+subsequence, v2+voffs+subsequence, 
        v1+voffs+subsequence, v1+voffs+subsequence, v1+voffs+subsequence
        );
    else
        fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v1+voffs+subsequence, v1+voffs+subsequence, v1+voffs+subsequence, 
        v2+voffs+subsequence, v2+voffs+subsequence, v2+voffs+subsequence, 
        v3+voffs+subsequence, v3+voffs+subsequence, v3+voffs+subsequence
        );

    next_face:
    v1 = v2;
    v2 = v3;
}
    subsequence += vert_cnt; //Continue the sequence of triangles after processing each mesh. This is very important for writing to obj.
    free(verts);
    free(indices);
    
    return 0;    
}    


/*
for (i = 0; i < index_cnt; i += 3) {
    v1 = indices[i];
    v2 = indices[i + 1];
    v3 = indices[i + 2];
	
	if (v1 == v2 || v1 == v3 || v2 == v3) {
        continue;
    }

    fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v3+voffs+subsequence, v3+voffs+subsequence, v3+voffs+subsequence, 
        v2+voffs+subsequence, v2+voffs+subsequence, v2+voffs+subsequence, 
        v1+voffs+subsequence, v1+voffs+subsequence, v1+voffs+subsequence);
}
    subsequence += vert_cnt; //Continue the sequence of triangles after processing each mesh. This is very important for writing to obj.
*/