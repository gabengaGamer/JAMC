//=============================================================================
//
//  Asura level converter.
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "level.h"

//=============================================================================
// STRUCTURIZING
//=============================================================================

    int index_end_pos;

    int face_subsequence = 0; 
    
//=============================================================================
// LEVEL PROCESSING
//=============================================================================

int LevelBatchProcess(int argc, char *argv[])
{
	fseek(f_in, 0, SEEK_SET);

    for (int i = 0; i < amount_index_clusters; i++) {
		#ifdef _DEBUG
        printf("Info: Mesh cluster: %d\n", i + 1);
		#endif
        GetLVIndexOffset(argc, argv);
    }
    return FinishProcessing(argc, argv);
}

void GetLVIndexOffset(int argc, char *argv[])
{
    index_offset = ftell(f_in);

    int index_offset_pattern_found;
    index_offset_pattern_found = 0; //Reset the value ​​after each run.
    unsigned char index_offset_pattern[8];
    
    while (fread(index_offset_pattern, 1, 8, f_in) == 8) {
        if (index_offset_pattern[0] == 0x00 &&
            index_offset_pattern[1] == 0x00 &&
            index_offset_pattern[2] == 0x01 &&
            index_offset_pattern[3] == 0x00 &&
            index_offset_pattern[4] >  0x00 &&
            index_offset_pattern[5] == 0x00 &&
            index_offset_pattern[6] >  0x00 &&
            index_offset_pattern[7] == 0x00) {
            index_offset_pattern_found = 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR);
        index_offset++;
    }
    
    if (index_offset_pattern_found) {
            #ifdef _DEBUG
            printf("Debug: Index offset starts: 0x%lX\n", index_offset);
            #endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset start!\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
    
    int index_start_pos;

    index_start_pos = index_offset;
    
    int index_offset_end_pattern_found;
    index_offset_end_pattern_found = 0; //Reset the value ​​after each run.
    unsigned char index_offset_end_pattern[8];
    
    while (fread(index_offset_end_pattern, 1, 8, f_in) == 8) { //I'm actually not sure about this pattern.
        if (index_offset_end_pattern[0] >= 0x01 &&             //There are differences in some places that create problems.  
            index_offset_end_pattern[1] >= 0x00 &&             //Also, changing the pattern can cause false positives.
            index_offset_end_pattern[2] == 0x00 &&
            index_offset_end_pattern[3] == 0x00 &&
            index_offset_end_pattern[4] >= 0x00 && //0x00 or 0x01 ?
            index_offset_end_pattern[5] >= 0x00 &&
            index_offset_end_pattern[6] == 0x00 &&
            index_offset_end_pattern[7] == 0x00) {
            index_offset_end_pattern_found = 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR);
    }
    
    if (index_offset_end_pattern_found) {
            index_end_pos = ftell(f_in); //Applying current position.
            
            index_end_pos -= 9; //Correcting

            #ifdef _DEBUG
            printf("Debug: Index offset ends: 0x%lX\n", index_end_pos);
            #endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset end. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            exit(1);
    }

    //Everything seems fine. Let's continue working with indexes.
    index_length = (index_end_pos - index_start_pos + 1) / 2; // Calculate the length.

    #ifdef _DEBUG
    printf("Debug: Index offset length: 0x%lX\n", index_length);
    #endif
    
    GetLVIndexCount(argc, argv);
}

//=============================================================================

void GetLVIndexCount(int argc, char *argv[])
{
    //Let's convert all this crap into a HEX-readable format.
    
    unsigned char inverted_index_cnt[4];
    
    inverted_index_cnt[0] =  index_length & 0xFF;
    inverted_index_cnt[1] = (index_length >> 8) & 0xFF;
    inverted_index_cnt[2] = (index_length >> 16) & 0xFF;
    inverted_index_cnt[3] = (index_length >> 24) & 0xFF;
    
    #ifdef _DEBUG
    printf("Debug: Structured index offset count value: %02X %02X %02X %02X\n",inverted_index_cnt[0], inverted_index_cnt[1], inverted_index_cnt[2], inverted_index_cnt[3]);
    #endif
    
    //Search for the original index count in the file.

    unsigned char index_cnt_offset_pattern[4];
    int index_cnt_offset_found = 0;
    
    index_cnt_offset = index_end_pos;
    
    while (fread(index_cnt_offset_pattern, 1, 4, f_in) == 4) {
        fseek(f_in, index_cnt_offset, SEEK_SET);
        if (index_cnt_offset_pattern[0] == inverted_index_cnt[0] &&
            index_cnt_offset_pattern[1] == inverted_index_cnt[1] &&
            index_cnt_offset_pattern[2] == inverted_index_cnt[2] &&
            index_cnt_offset_pattern[3] == inverted_index_cnt[3]) {
            index_cnt_offset_found = 1;
            break;
        }
        fseek(f_in, -1, SEEK_CUR); //For a more accurate check.
        index_cnt_offset--;
    }
    
    if (index_cnt_offset_found) {
            #ifdef _DEBUG
            printf("Debug: Index offset count value starts: 0x%lX\n", index_cnt_offset);
            #endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset count value!\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
    
    GetLVVertexOffset(argc, argv);
}

//=============================================================================

void GetLVVertexOffset(int argc, char *argv[])
{
    //Finding of the offset of vertices.    
    vert_offset = index_cnt_offset + 4; //Vertexes offset are always behind index cnt. That's all the magic

    #ifdef _DEBUG
    printf("Vertex offset starts in: 0x%lX\n", vert_offset);
    #endif
    
    GetLVVertexCount(argc, argv);
}

//=============================================================================

void GetLVVertexCount(int argc, char *argv[])    
{
    //Finding of the number of vertices.    
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
    
    #ifdef _DEBUG
    printf("Debug: Vertex offset count value starts: 0x%lX\n", vert_cnt_offset);
    #endif
    
    jamc_level_convertation(argc, argv);
}

//=============================================================================
// CONVERTING
//=============================================================================
int jamc_level_convertation(int argc, char *argv[])
{
    unsigned i;
    unsigned vert_cnt;
    unsigned index_cnt;

    struct MYLEVELVERTEX *verts;
    unsigned short *indices;

    int voffs = 1;
    unsigned short *pi;
    unsigned short v1, v2, v3;
    
    static int mesh_cnt = 0;

    fseek(f_in, vert_cnt_offset, SEEK_SET); //Setting vertex count.
    fread(&vert_cnt, 1, sizeof(vert_cnt), f_in);
    
    fseek(f_in, vert_offset, SEEK_SET); //Setting vertex reading position.
    verts = (struct MYLEVELVERTEX *)malloc(sizeof(struct MYLEVELVERTEX) * vert_cnt);
    fread(verts, 1, sizeof(struct MYLEVELVERTEX)*vert_cnt, f_in);
    
    fprintf(f_out, "g mesh_%d\n", mesh_cnt); //Separating geometry.
    fprintf(f_out, "o mesh_%d\n", mesh_cnt);
    mesh_cnt++;
    
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

    fseek(f_in, index_cnt_offset, SEEK_SET); //Setting faces count.
    fread(&index_cnt, 1, sizeof(index_cnt), f_in);
    
    fseek(f_in, index_offset, SEEK_SET); //Setting faces reading position.
    indices = (unsigned short *)malloc(index_cnt * sizeof(unsigned short));
    fread(indices, 1, sizeof(unsigned short)*index_cnt, f_in);
    
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
    if(v1 == v2 || v1 == v3 || v2 == v3)
       goto next_face;

//Flip every second face.
    if((i - 2) % 2)
        fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v3+voffs+face_subsequence, v3+voffs+face_subsequence, v3+voffs+face_subsequence, 
        v2+voffs+face_subsequence, v2+voffs+face_subsequence, v2+voffs+face_subsequence, 
        v1+voffs+face_subsequence, v1+voffs+face_subsequence, v1+voffs+face_subsequence
        );
    else
        fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v1+voffs+face_subsequence, v1+voffs+face_subsequence, v1+voffs+face_subsequence, 
        v2+voffs+face_subsequence, v2+voffs+face_subsequence, v2+voffs+face_subsequence, 
        v3+voffs+face_subsequence, v3+voffs+face_subsequence, v3+voffs+face_subsequence
        );

    next_face:
    v1 = v2;
    v2 = v3;
}
    face_subsequence += vert_cnt;
    free(verts);
    free(indices);
    return 0;
}    


/* Graveyard aka CODE VAULT
                                INDEX END PATTERN SEARCHER (OLD)

    while (fread(index_offset_end_pattern, 1, 12, f_in) == 12) { //Finding end of faces by pattern.
        if (index_offset_end_pattern[0] >= 0x01 &&
            index_offset_end_pattern[1] >= 0x00 &&
            index_offset_end_pattern[2] == 0x00 &&
            index_offset_end_pattern[3] == 0x00 &&
            index_offset_end_pattern[4] >= 0x01 &&
            index_offset_end_pattern[5] >= 0x00 &&
            index_offset_end_pattern[6] == 0x00 &&
            index_offset_end_pattern[7] >= 0x00 &&
            index_offset_end_pattern[8] >  0x00 &&
            index_offset_end_pattern[9] >  0x00 &&
            index_offset_end_pattern[10] > 0x00 &&
            index_offset_end_pattern[11] > 0x00){ // Очень спорно, шо пиздец.
            index_offset_end_pattern_found = 1;
            break;
        }
        fseek(f_in, -11, SEEK_CUR);
    }

                                INDEX START PATTERN SEARCHER (OLD)

	while (fread(index_offset_start_bytes, 1, 8, f_in) == 8) { //Finding start of faces by pattern.
        if (index_offset_start_bytes[0] == 0x00 &&
            index_offset_start_bytes[1] == 0x00 &&
            index_offset_start_bytes[2] == 0x01 &&
            index_offset_start_bytes[3] == 0x00 &&
            index_offset_start_bytes[4] >= 0x01 &&
            index_offset_start_bytes[5] == 0x00 &&
            index_offset_start_bytes[6] >= 0x01 &&
            index_offset_start_bytes[7] == 0x00) {
            index_offset_start_bytes_found = 1;
            break;
        }
		fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
    }
*/
