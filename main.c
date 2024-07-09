//=============================================================================
//
//  Program:        JusticeAsuraModelConvertor | J.A.M.C.
//
//  Credits:        Modera
//
//  Author:         GameSpy
//
//  Date:           Started 05.07.2024
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//=============================================================================
// STRUCTURIZING
//=============================================================================

struct MYVERTEX
{
    float pos[3];
    float normal[3];
    float tc[2];
	//float bones[8]; //Because I'm only working with OBJ for now. Using bones is not possible. But I can at least convert the mesh.
};

int main(int argc, char *argv[])	
{
    unsigned i;
    unsigned vert_cnt;
    unsigned index_cnt;

    struct MYVERTEX *verts;
    unsigned short *indices;

    int voffs = 1;
    unsigned short *pi;
    unsigned short v1, v2, v3;
	
    int index_offset_bytes_found = 0;
    int index_cnt_offset_bytes_found = 0;
    int vert_offset_bytes_found = 0;
    size_t index_offset = 0;
    size_t index_cnt_offset = 0;
    size_t vert_offset = 0;                                  //Fuck!!! There are too many identifiers here. It will be necessary to reduce their number in the future
    size_t vert_cnt_offset = 0;
    unsigned char index_offset_bytes[8];
    unsigned char index_cnt_offset_bytes[4];
    unsigned char index_cnt_offset_hex_value[4];
    unsigned char vert_offset_bytes[8];
	
    size_t index_offset_end_pos = 0;
    size_t index_offset_start_pos = 0;
    size_t index_offset_length = 0;
    size_t divided_index_offset_length = 0;
	
    int index_offset_end_pos_null_counter = 0;
    int index_offset_end_pos_null_counter_vault = 0;
    size_t index_offset_end_null_pos = 0;
	
	int vertex_offset_bones_null_count = 0;
	unsigned char vertex_offset_bones_check_bytes[20];
	
//=============================================================================
// FILE SYSTEM
//=============================================================================

    const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    input_file = argv[1];
    output_file = "output.obj";

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    f_in = fopen(input_file, "rb");
    if (!f_in) {
        fprintf(stderr, "Error loading input file!\n", argv[0]);
        return 1;
    }

    f_out = fopen(output_file, "w");
    if (!f_out) {
        fprintf(stderr, "Error loading output file!\n", argv[0]);
        fclose(f_in);
        return 1;
    }
	
//=============================================================================
// PREPARATION
//=============================================================================

	fprintf(stderr, "Processing...\n", argv[0]);
	
	while (fread(index_offset_bytes, 1, 8, f_in) == 8) { //Finding beginning of faces by pattern.
        if (index_offset_bytes[0] == 0x00 &&
            index_offset_bytes[1] == 0x00 &&
            index_offset_bytes[2] == 0x01 &&
            index_offset_bytes[3] == 0x00 &&
            index_offset_bytes[4] >  0x00 &&
            index_offset_bytes[5] == 0x00 &&
            index_offset_bytes[6] >  0x00 &&
            index_offset_bytes[7] == 0x00) {
            index_offset_bytes_found = 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //Back to the beginning.
        index_offset++;
    }

    index_offset_start_pos = index_offset + 20; //Hacky hack! I dont know how it works, but, it actually works!
	
    fseek(f_in, 0, SEEK_END);
    index_offset_end_pos = ftell(f_in);
	
//Checking for the extra zeros in the indexes.
    index_offset_end_null_pos = index_offset_end_pos - 1; //Set the starting search position
	
    while (index_offset_end_null_pos >= 0){
        fseek(f_in, index_offset_end_null_pos, SEEK_SET); //Some models use a lot of extra zeros at the end of indexes. So let's see if there are any extra zeros
        index_offset_end_pos_null_counter_vault = fgetc(f_in);
        if (index_offset_end_pos_null_counter_vault == 0x00) {
            index_offset_end_pos_null_counter++;
            index_offset_end_null_pos--;
        }   else {
            break;
        }
    }

    if (index_offset_end_pos_null_counter > 1) {
        index_offset_end_pos -= (index_offset_end_pos_null_counter - 1); //Rewrite extra zeros
    }
	
//Everything seems fine. Let's continue working with indexes.
    index_offset_length = index_offset_end_pos - index_offset_start_pos + 20; // Why 20 ? I don't know how it happened.

    divided_index_offset_length = index_offset_length / 2; //Divide our value to obtain information.

    if (index_offset_bytes_found) {
            #ifdef DEBUG
            printf("Your index offset HEX value is: 0x%lX\n", divided_index_offset_length);
			#endif
    } else {
            fprintf(stderr, "Error finding index offset information. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//Let's convert all this crap into a script-readable format.
    index_cnt_offset_hex_value[0] = divided_index_offset_length & 0xFF;
    index_cnt_offset_hex_value[1] = (divided_index_offset_length >> 8) & 0xFF;
    index_cnt_offset_hex_value[2] = (divided_index_offset_length >> 16) & 0xFF;
    index_cnt_offset_hex_value[3] = (divided_index_offset_length >> 24) & 0xFF;
    #ifdef DEBUG
    printf("Structured index offset HEX value: %02X %02X %02X %02X\n",index_cnt_offset_hex_value[0], index_cnt_offset_hex_value[1], index_cnt_offset_hex_value[2], index_cnt_offset_hex_value[3]);
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
        fseek(f_in, -3, SEEK_CUR); //Back to the beginning.
		index_cnt_offset++;
    }
	
    if (index_cnt_offset_bytes_found) {
		    #ifdef DEBUG
            printf("Index count value has been found at position: 0x%lX\n", index_cnt_offset);
			#endif
    } else {
            fprintf(stderr, "Error finding index information. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//========================Now let's prepare the vertexes========================

    fseek(f_in, 0, SEEK_SET);

    while (fread(vert_offset_bytes, 1, 8, f_in) == 8) { //Finding beginning of vertices by pattern.
        if (vert_offset_bytes[0] == 0x00 &&
            vert_offset_bytes[1] > 0x00 &&
            vert_offset_bytes[2] > 0x00 &&
			vert_offset_bytes[3] > 0x00 &&
            vert_offset_bytes[4] > 0x00 &&
			vert_offset_bytes[5] > 0x00 &&
            vert_offset_bytes[6] > 0x00 &&
            vert_offset_bytes[7] > 0x00) {
            vert_offset_bytes_found = 1;
            vert_offset += 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //Back to the beginning.
        vert_offset++;
    }
	
    if (vert_offset_bytes_found) {
		    #ifdef DEBUG
            printf("Your vertex offset HEX value is: 0x%lX\n", vert_offset);
			#endif
    } else {
            fprintf(stderr, "Error finding vertex bytes. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//Finding of the number of vertices.	
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
	
	#ifdef DEBUG
    printf("Vertex count value has been found at position: 0x%lX\n", vert_cnt_offset);
	#endif
//========================Checking the model for bones=========================

    fseek(f_in, vert_offset + 20, SEEK_SET);
    fread(vertex_offset_bones_check_bytes, 1, 20, f_in); //Move +20 bytes to the expected location of the bones.

    for (i = 0; i < 20; i++) {
        if (vertex_offset_bones_check_bytes[i] == 0x00) { //Counting nulls
            vertex_offset_bones_null_count++;
        }
        if (vertex_offset_bones_null_count >= 4) { //More than 4? Great, it's a ragdoll.
			#define bones
		    #ifdef DEBUG
			printf("Ragdoll has been detected\n");
			#endif
			break;
        }
    }

//=============================================================================
// CONVERTING
//=============================================================================

    fprintf(stderr, "Converting...\n", argv[0]);

	fseek(f_in, vert_cnt_offset, SEEK_SET); //Setting vertex count.
	fread(&vert_cnt, 1, sizeof(vert_cnt), f_in);
	
	fseek(f_in, vert_offset, SEEK_SET); //Setting vertex reading position.
    verts = malloc(sizeof(struct MYVERTEX)*vert_cnt);
	fread(verts, 1, sizeof(struct MYVERTEX)*vert_cnt, f_in);
	
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
	indices = malloc(index_cnt * sizeof(unsigned short));
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

//Skip degenerate faces.
    if(v1 == v2 || v1 == v3 || v2 == v3)
       goto next_face;

//Flip every second face.
    if((i - 2) % 2)
	    fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v3+voffs, v3+voffs, v3+voffs, 
		v2+voffs, v2+voffs, v2+voffs, 
		v1+voffs, v1+voffs, v1+voffs
	    );
    else
	    fprintf(f_out, "f %u/%u/%u %u/%u/%u %u/%u/%u\n",
        v1+voffs, v1+voffs, v1+voffs, 
		v2+voffs, v2+voffs, v2+voffs, 
		v3+voffs, v3+voffs, v3+voffs
	    );

    next_face:
    v1 = v2;
    v2 = v3;
}

//=============================================================================
// FINALIZING
//=============================================================================

	free(verts);
	free(indices);

	fclose(f_in);
	fclose(f_out);

	fprintf(stderr, "Conversion complete\n", argv[0]); //Sexy.
	
	return 0;	
}