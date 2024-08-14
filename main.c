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
//#include "level.h"
#include "ragdoll.h"

//=============================================================================
// STRUCTURIZING
//=============================================================================

    unsigned i;
	
	const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    int index_offset = 0;
    int index_cnt_offset = 0;
    int vert_offset = 0;                           
    int vert_cnt_offset = 0;
	
	unsigned char index_cnt_offset_hex_value[4];

//=============================================================================
// ASSET LOADING
//=============================================================================

void GetAssetType(int argc, char *argv[]) 
{
	//Trying to determine the type of asset.
	
	unsigned char index_offset_pattern[8];
	int amount_index_clusters = 0; 
	
	while (fread(index_offset_pattern, 1, 8, f_in) == 8) { //Count the number of indexes, thus determining whether the model is a level.
        if (index_offset_pattern[0] == 0x00 &&
            index_offset_pattern[1] == 0x00 &&
            index_offset_pattern[2] == 0x01 &&
            index_offset_pattern[3] == 0x00 &&
            index_offset_pattern[4] >  0x00 &&
            index_offset_pattern[5] == 0x00 &&
            index_offset_pattern[6] >  0x00 &&
            index_offset_pattern[7] == 0x00) {
            amount_index_clusters++;	
        }
		fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
    }
	
	#ifdef _DEBUG
    printf("Debug: Index clusters count: 0x%lX\n", amount_index_clusters);
	#endif

	if (amount_index_clusters > 1) {
		    #ifdef _DEBUG
			printf("Debug: Level has been detected\n");
			#endif
			//jamc_level_preparation(argc, argv);
			exit(1);
    } else {
		    #ifdef _DEBUG
		    printf("Debug: Level not detected\n");
			#endif
			GetIndexOffset(argc, argv);
    }
}

//=============================================================================

void GetIndexOffset(int argc, char *argv[])	
{			
	fseek(f_in, 0, SEEK_SET);
	
	//Finding beginning of faces by pattern.
	
	int index_offset_start_bytes_found = 0;
    unsigned char index_offset_start_bytes[8];
	
	while (fread(index_offset_start_bytes, 1, 8, f_in) == 8) {
        if (index_offset_start_bytes[0] == 0x00 &&
            index_offset_start_bytes[1] == 0x00 &&
            index_offset_start_bytes[2] == 0x01 &&
            index_offset_start_bytes[3] == 0x00 &&
            index_offset_start_bytes[4] >  0x00 &&
            index_offset_start_bytes[5] == 0x00 &&
            index_offset_start_bytes[6] >  0x00 &&
            index_offset_start_bytes[7] == 0x00) {
            index_offset_start_bytes_found = 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
        index_offset++;
    }
	
	if (index_offset_start_bytes_found) {
            #ifdef _DEBUG
            printf("Debug: Index offset starts: 0x%lX\n", index_offset);
			#endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset start.\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
	
	int index_offset_start_pos = 0;

    index_offset_start_pos = index_offset + 20; //Hacky hack! I dont know how it works, but, it actually works!
	
	//Finding ending of faces.
	
	int index_offset_end_pos = 0;
	int index_offset_end_pos_null_counter = 0;
    int index_offset_end_pos_null_counter_vault = 0;
    int index_offset_end_null_pos = 0;
	
    fseek(f_in, 0, SEEK_END);
    index_offset_end_pos = ftell(f_in);
	
    #ifdef _DEBUG
    printf("Debug: Index offset ends: 0x%lX\n", index_offset_end_pos);
	#endif
	
    //Checking for the extra zeros in the indexes.
	//Some models use a lot of extra zeros at the end of indexes. So let's see if there are any extra zeros.
	
    index_offset_end_null_pos = index_offset_end_pos - 1; //Set the starting search position
	
    while (index_offset_end_null_pos >= 0){ //Processing
        fseek(f_in, index_offset_end_null_pos, SEEK_SET);
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
	
	int index_offset_length = 0;
    int divided_index_offset_length = 0;
	
    index_offset_length = index_offset_end_pos - index_offset_start_pos + 20; // Why 20 ? I don't know how it happened.

    divided_index_offset_length = index_offset_length / 2; //Divide our value to obtain information.

    if (index_offset_start_bytes_found) {
            #ifdef _DEBUG
            printf("Debug: Index offset length: 0x%lX\n", divided_index_offset_length);
			#endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset count value.\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
	
    //Let's convert all this crap into a script-readable format.
	
    index_cnt_offset_hex_value[0] = divided_index_offset_length & 0xFF;
    index_cnt_offset_hex_value[1] = (divided_index_offset_length >> 8) & 0xFF;
    index_cnt_offset_hex_value[2] = (divided_index_offset_length >> 16) & 0xFF;
    index_cnt_offset_hex_value[3] = (divided_index_offset_length >> 24) & 0xFF;
	
    #ifdef _DEBUG
    printf("Debug: Structured index offset count value: %02X %02X %02X %02X\n",index_cnt_offset_hex_value[0], index_cnt_offset_hex_value[1], index_cnt_offset_hex_value[2], index_cnt_offset_hex_value[3]);
	#endif
	
	GetIndexCount(argc, argv);
}

//=============================================================================

void GetIndexCount(int argc, char *argv[])	
{
//Search for the hex value in the file.

    unsigned char index_cnt_offset_pattern[4];
    int index_cnt_offset_found = 0;

    fseek(f_in, 0, SEEK_SET);
	
    while (fread(index_cnt_offset_pattern, 1, 4, f_in) == 4) {
        if (index_cnt_offset_pattern[0] == index_cnt_offset_hex_value[0] &&
            index_cnt_offset_pattern[1] == index_cnt_offset_hex_value[1] &&
            index_cnt_offset_pattern[2] == index_cnt_offset_hex_value[2] &&
            index_cnt_offset_pattern[3] == index_cnt_offset_hex_value[3]) {
            index_cnt_offset_found = 1;
            break;
        }
        fseek(f_in, -3, SEEK_CUR); //For a more accurate check.
		index_cnt_offset++;
    }
	
    if (index_cnt_offset_found) {
		    #ifdef _DEBUG
            printf("Debug: Index offset count value starts: 0x%lX\n", index_cnt_offset);
			#endif
    } else {
            fprintf(stderr, "Alert: Error finding index offset count value.\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
	GetVertexOffset(argc, argv);
}

//=============================================================================

void GetVertexOffset(int argc, char *argv[])	
{
	int vert_offset_found = 0;
	unsigned char vert_offset_pattern[8];
	
    fseek(f_in, 0, SEEK_SET);

    while (fread(vert_offset_pattern, 1, 8, f_in) == 8) { //Finding beginning of vertices by pattern.
        if (vert_offset_pattern[0] == 0x00 &&
            vert_offset_pattern[1] > 0x00 &&
            vert_offset_pattern[2] > 0x00 &&
			vert_offset_pattern[3] > 0x00 &&
            vert_offset_pattern[4] > 0x00 &&
			vert_offset_pattern[5] > 0x00 &&
            vert_offset_pattern[6] > 0x00 &&
            vert_offset_pattern[7] > 0x00) {
            vert_offset_found = 1;
            vert_offset += 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
        vert_offset++;
    }
	
    if (vert_offset_found) {
		    #ifdef _DEBUG
            printf("Debug: Vertex offset starts: 0x%lX\n", vert_offset);
			#endif
    } else {
            fprintf(stderr, "Alert: Error finding offset start.\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
	GetVertexCount(argc, argv);
}

//=============================================================================

void GetVertexCount(int argc, char *argv[])	
{
    //Finding of the number of vertices.	
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
	
	#ifdef _DEBUG
    printf("Debug: Vertex offset count value starts: 0x%lX\n", vert_cnt_offset);
	#endif
	
	GetModelType(argc, argv);
}

//=============================================================================

void GetModelType(int argc, char *argv[]) 
{
	//Trying to determine the type of model.
	
	#ifdef _DEBUG
    int total_null_count = 0;
	#endif
	int bones_null_count = 0;
	unsigned char bones_null_counter[32];

    #ifdef _DEBUG
	total_null_count = vert_offset + 32; // 32 - a set of bytes of coordinates, normals and uv coordinates.
    printf("Debug: Ragdoll counter offset starts: 0x%lX\n", total_null_count);
	#endif
	
    fseek(f_in, vert_offset + 32, SEEK_SET); // 32 - a set of bytes of coordinates, normals and uv coordinates.
    fread(bones_null_counter, 1, 32, f_in); //Next 32 bytes should be checked here.

    for (i = 0; i < 32; i++) {
        if (bones_null_counter[i] == 0x00) { //Counting nulls.
            bones_null_count++;
			#ifdef _DEBUG	
			printf("Debug: Ragdoll counted zero: 0x%lX\n", bones_null_count);
			#endif
        }
    }
	
	if (bones_null_count >= 16) {
		    #ifdef _DEBUG	
			printf("Debug: Ragdoll total zeros: 0x%lX\n", bones_null_count);
			printf("Debug: Ragdoll has been detected\n");
			#endif
			jamc_ragdoll_convertation(argc, argv);
    } else {
		    #ifdef _DEBUG
		    printf("Debug: Ragdoll not detected\n");
			#endif
			jamc_prop_convertation(argc, argv);
    }
}

//=============================================================================
// FINALIZING
//=============================================================================
int FinishProcessing(int argc, char *argv[])	
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
        fprintf(stderr, "Alert: Error loading input file!\n", argv[0]);
        return 1;
    }

    f_out = fopen(output_file, "w");
    if (!f_out) {
        fprintf(stderr, "Alert: Error loading output file!\n", argv[0]);
        fclose(f_in);
        return 1;
    }
	
	fprintf(stderr, "Reading...\n", argv[0]);
	GetAssetType(argc, argv);
}