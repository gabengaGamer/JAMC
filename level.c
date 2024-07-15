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

    size_t index_end_offset = 0;
	size_t index_start_offset = 0;
	
	//int index_level_offset_start_bytes_found = 0;
    //unsigned char index_level_offset_start_bytes[16];

    int index_offset_end_bytes_found = 0;
    unsigned char index_offset_end_bytes[8];
	
//=============================================================================
// LEVEL PROCESSING
//=============================================================================
int jamc_level_preparation(int argc, char *argv[])	
{
	fseek(f_in, 0, SEEK_SET);
	
	//fprintf(stderr, "Level preparation may take a long time, stay on the line...\n", argv[0]);
	
	while (1) {
	
    //Reset the values ​​after each run.	
	index_offset_start_bytes_found = 0;
    index_offset_end_bytes_found = 0;
	index_cnt_offset = 0;
	memset(index_offset_start_bytes, 0, sizeof(index_offset_start_bytes));
    memset(index_offset_end_bytes, 0, sizeof(index_offset_end_bytes));

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
	
	if (index_offset_start_bytes_found) {
		    index_start_offset = ftell(f_in); //Applying current position.

            index_offset_start_pos = index_start_offset - 8; //End of the line.
			
			index_offset = index_offset_start_pos; //Start of indexes.
		
            #ifdef _DEBUG
            printf("Index offset starts in: 0x%lX\n", index_offset_start_pos);
			#endif
    } else {
            fprintf(stderr, "Error finding index offset start. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
	while (fread(index_offset_end_bytes, 1, 8, f_in) == 8) { //Finding end of faces by pattern.
        if (index_offset_end_bytes[0] >= 0x01 &&
            index_offset_end_bytes[1] >= 0x00 &&
            index_offset_end_bytes[2] == 0x00 &&
            index_offset_end_bytes[3] == 0x00 &&
            index_offset_end_bytes[4] >= 0x01 &&
            index_offset_end_bytes[5] >= 0x00 &&
            index_offset_end_bytes[6] == 0x00 &&
            index_offset_end_bytes[7] == 0x00) {
            index_offset_end_bytes_found = 1;
            break;
        }
		fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
    }
	
	if (index_offset_start_bytes_found) {
		    index_end_offset = ftell(f_in); //Applying current position.

            index_offset_end_pos = index_end_offset - 9; //End of the line.
		
            #ifdef _DEBUG
            printf("Index offset ends in: 0x%lX\n", index_offset_end_pos);
			#endif
    } else {
            fprintf(stderr, "Error finding index offset end. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }

//Everything seems fine. Let's continue working with indexes.
    index_offset_length = index_offset_end_pos - index_offset_start_pos + 1;

    divided_index_offset_length = index_offset_length / 2; //Divide our value to obtain information.

    if (index_offset_start_bytes_found) {
            #ifdef _DEBUG
            printf("Index offset count value is: 0x%lX\n", divided_index_offset_length);
			#endif
    } else {
            fprintf(stderr, "Error finding index offset count value. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//Let's convert all this crap into a script-readable format.
    index_cnt_offset_hex_value[0] = divided_index_offset_length & 0xFF;
    index_cnt_offset_hex_value[1] = (divided_index_offset_length >> 8) & 0xFF;
    index_cnt_offset_hex_value[2] = (divided_index_offset_length >> 16) & 0xFF;
    index_cnt_offset_hex_value[3] = (divided_index_offset_length >> 24) & 0xFF;
	
    #ifdef _DEBUG
    printf("Structured index offset count value in HEX is: %02X %02X %02X %02X\n",index_cnt_offset_hex_value[0], index_cnt_offset_hex_value[1], index_cnt_offset_hex_value[2], index_cnt_offset_hex_value[3]);
	#endif
	
//Search for the hex value in the file.
    fseek(f_in, 0, SEEK_SET);
	
    while (fread(index_cnt_offset_bytes, 1, 4, f_in) == 4) {
        if (index_cnt_offset_bytes[0] == index_cnt_offset_hex_value[0] &&
            index_cnt_offset_bytes[1] == index_cnt_offset_hex_value[1] &&
            index_cnt_offset_bytes[2] == index_cnt_offset_hex_value[2] &&
            index_cnt_offset_bytes[3] == index_cnt_offset_hex_value[3]) {
            index_cnt_offset_bytes_found = 1;
            break;
        }
        fseek(f_in, -3, SEEK_CUR); //For a more accurate check.
		index_cnt_offset++;
    }
	
    if (index_cnt_offset_bytes_found) {
		    #ifdef _DEBUG
            printf("Index offset count value has been found at position: 0x%lX\n", index_cnt_offset);
			#endif
    } else {
            fprintf(stderr, "Error finding index offset count value. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//========================Now let's prepare the vertexes========================

    //Finding of the number of vertices.	
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
	
	#ifdef _DEBUG
    printf("Vertex offset count value has been found at position: 0x%lX\n", vert_cnt_offset);
    #endif
	
	//Finding of the offset of vertices.	
	vert_offset = index_cnt_offset + 4; //Vertexes offset are always behind index cnt. That's all the magic

	#ifdef _DEBUG
    printf("Vertex offset starts in: 0x%lX\n", vert_offset);
    #endif
	
    if (jamc_level_convertation(argc, argv) != 0) {
        return 1;
    }
        fseek(f_in, index_end_offset, SEEK_SET);
    }

    return jamc_finalization(argc, argv);
}
//=============================================================================
// STRUCTURIZING
//=============================================================================

    size_t face_subsequence = 0; 

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
	
	int mesh_cnt = 0;

    fprintf(stderr, "Level conversion can take from 1 to 5 minutes, stay on the line...\n", argv[0]);

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
	/*
	for(i = 0; i < vert_cnt; i++) {
		verts[i].tc[1] = 1.0f -verts[i].tc[1]; //Inverting UV to sync with texture maps.
		fprintf(f_out, "vt %f %f\n", verts[i].tc[0], verts[i].tc[1]); 
	}
	*/

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