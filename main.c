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
#include "main.h"
#include "prop.h"
#include "level.h"
#include "ragdoll.h"

//=============================================================================
// STRUCTURIZING
//=============================================================================

    unsigned i;
	
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
	unsigned char vertex_offset_bones_check_bytes[32];
	
	const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    #ifdef _DEBUG
    int total_null_count = 0;
	#endif
//=============================================================================
// PREPARATION
//=============================================================================
int jamc_preparation(int argc, char *argv[])	
{
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
            #ifdef _DEBUG
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
    #ifdef _DEBUG
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
		    #ifdef _DEBUG
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
		    #ifdef _DEBUG
            printf("Your vertex offset HEX value is: 0x%lX\n", vert_offset);
			#endif
    } else {
            fprintf(stderr, "Error finding vertex bytes. Are you using the right file?\n"); //I think you just write a bad code. Sad.
            return 1;
    }
	
//Finding of the number of vertices.	
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
	
	#ifdef _DEBUG
    printf("Vertex count value has been found at position: 0x%lX\n", vert_cnt_offset);
	#endif
//========================Checking the model for bones=========================

    #ifdef _DEBUG
	total_null_count = vert_offset + 32; // 20 - совокупность байтов координат,нормалей и uv координат.
    printf("Counter start: 0x%lX\n", total_null_count);
	#endif
	
    fseek(f_in, vert_offset + 32, SEEK_SET); // 32 - a set of bytes of coordinates, normals and uv coordinates.
    fread(vertex_offset_bones_check_bytes, 1, 32, f_in); //In theory, 32 bytes should be checked here but not 20, I have problems with this.

    for (i = 0; i < 32; i++) {
        if (vertex_offset_bones_check_bytes[i] == 0x00) { //Counting nulls.
            vertex_offset_bones_null_count++;
			printf("CC0: 0x%lX\n", vertex_offset_bones_null_count);
        }
    }
	if (vertex_offset_bones_null_count >= 16) {
		    printf("CC2: 0x%lX\n", vertex_offset_bones_null_count);
		    #ifdef _DEBUG	
			printf("Ragdoll has been detected\n");
			#endif
			return jamc_ragdoll_convertation(argc, argv);
    } else {
			return jamc_prop_convertation(argc, argv);
    }
	fprintf(stderr, "Error counting zeros. Are you using the right file?\n");
	return 1;
}

//=============================================================================
// FINALIZING
//=============================================================================
int jamc_finalization(int argc, char *argv[])	
{
	fclose(f_in);
	fclose(f_out);

	fprintf(stderr, "Conversion complete.\n", argv[0]); //Sexy.
	
	return 0;	
}

//=============================================================================
// FILE SYSTEM
//=============================================================================
int main(int argc, char *argv[])	
{
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
	return jamc_preparation(argc, argv);
}