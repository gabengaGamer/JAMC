//=============================================================================
//
//  Program:        JusticeAsuraModelConverter | J.A.M.C.
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
#include <windows.h>
#include "main.h"
#include "prop.h"
#include "level.h"
#include "ragdoll.h"

//=============================================================================
// STRUCTURIZING
//=============================================================================

    unsigned i;
    
    const char *input_file;
    const char *output_file;

    FILE *f_in;
    FILE *f_out;

    int index_offset;
    int index_cnt_offset;
    int vert_offset;                           
    int vert_cnt_offset;
    
    int amount_index_clusters; 
    
    int index_length;

//=============================================================================
// ASSET LOADING
//=============================================================================

void GetAssetType() 
{
    //Trying to determine the type of asset.
    
    unsigned char index_offset_pattern[8];
    
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
            //GetLVIndexOffset();
            LevelBatchProcess();
    } else {
            #ifdef _DEBUG
            printf("Debug: Level not detected\n");
            #endif
            GetIndexOffset();
    }
}

//=============================================================================

void GetIndexOffset()    
{            
    fseek(f_in, 0, SEEK_SET);
    
    index_offset = ftell(f_in);
    
    //Finding beginning of faces by pattern.
    
    int index_offset_pattern_found = 0;
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
        fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
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
    
    //Finding ending of faces.
    
    int index_end_pos;
    
    fseek(f_in, 0, SEEK_END);
    index_end_pos = ftell(f_in);
    
    #ifdef _DEBUG
    printf("Debug: Index offset ends: 0x%lX\n", index_end_pos);
    #endif
    
    //Checking for the extra zeros in the indexes.
    //Some models use a lot of extra zeros at the end of indexes. So let's see if there are any extra zeros.
    
    int index_end_pos_corrected = 0;
    int index_end_pos_null_counter = 0;
    int index_end_pos_null_count = 0;
    
    index_end_pos_corrected = index_end_pos - 1; //Set the starting search position
    
    while (index_end_pos_corrected >= 0){ //Processing
        fseek(f_in, index_end_pos_corrected, SEEK_SET);
        index_end_pos_null_counter = fgetc(f_in);
        if (index_end_pos_null_counter == 0x00) {
            index_end_pos_null_count++;
            index_end_pos_corrected--;
        }   else {
            break;
        }
    }

    if (index_end_pos_null_count > 1) {
        index_end_pos -= (index_end_pos_null_count - 1); //Rewrite extra zeros
    }

    //Everything seems fine. Let's continue working with indexes.
    
    index_length = (index_end_pos - index_start_pos) / 2; // Calculate the length.

    #ifdef _DEBUG
    printf("Debug: Index offset length: 0x%lX\n", index_length);
    #endif
    
    GetIndexCount();
}

//=============================================================================

void GetIndexCount()    
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

    fseek(f_in, 0, SEEK_SET);
    
    index_cnt_offset = ftell(f_in);
    
    while (fread(index_cnt_offset_pattern, 1, 4, f_in) == 4) {
        if (index_cnt_offset_pattern[0] == inverted_index_cnt[0] &&
            index_cnt_offset_pattern[1] == inverted_index_cnt[1] &&
            index_cnt_offset_pattern[2] == inverted_index_cnt[2] &&
            index_cnt_offset_pattern[3] == inverted_index_cnt[3]) {
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
            fprintf(stderr, "Alert: Error finding index offset count value!\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
    GetVertexOffset();
}

//=============================================================================

void GetVertexOffset()    
{
    int vert_offset_found = 0;
    unsigned char vert_offset_pattern[8];
    
    fseek(f_in, 0, SEEK_SET);
    
    vert_offset = ftell(f_in);

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
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //For a more accurate check.
        vert_offset++;
    }
    
    vert_offset += 1; //корректировка крч
    
    if (vert_offset_found) {
            #ifdef _DEBUG
            printf("Debug: Vertex offset starts: 0x%lX\n", vert_offset);
            #endif
    } else {
            fprintf(stderr, "Alert: Error finding vertex offset start!\n"); //I think you just write a bad code. Sad.
            exit(1);
    }
    GetVertexCount();
}

//=============================================================================

void GetVertexCount()    
{
    //Finding of the number of vertices.    
    vert_cnt_offset = index_cnt_offset - 4; //Vertexes are always behind indexes. That's all the magic
    
    #ifdef _DEBUG
    printf("Debug: Vertex offset count value starts: 0x%lX\n", vert_cnt_offset);
    #endif
    
    GetModelType();
}

//=============================================================================

void GetModelType() 
{
    //Trying to determine the type of model.
    
    int bones_null_offset = 0;
    int bones_null_count = 0;
    unsigned char bones_null_counter[32];

    bones_null_offset = vert_offset + 32; // 32 - a set of bytes of coordinates, normals and uv coordinates.
    
    #ifdef _DEBUG
    printf("Debug: Ragdoll counter offset starts: 0x%lX\n", bones_null_offset);
    #endif
    
    fseek(f_in, bones_null_offset, SEEK_SET);
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
            jamc_ragdoll_convertation();
    } else {
            #ifdef _DEBUG
            printf("Debug: Ragdoll not detected\n");
            #endif
            jamc_prop_convertation();
    }
}

//=============================================================================
// FINALIZING
//=============================================================================

int FinishProcessing()    
{
    fclose(f_in);
    fclose(f_out);

    printf("Conversion complete.\n"); //Sexy.
    
    return 0;    
}

//=============================================================================
// READING ANIMATION
//=============================================================================

void ReadingAnimation() {
    for (int i = 0; i < 3; i++) {
        printf("\rReading");
        for (int j = 0; j <= i; j++) {
            printf(".");
        }
        Sleep(512);
    }
    printf("\rReading...\n");
}

//=============================================================================
// FILE SYSTEM
//=============================================================================

int main(int argc, char *argv[])   
{
    input_file = argv[1];
    output_file = "output.obj";

    if (argc < 2) {
        printf("Info: Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    f_in = fopen(input_file, "rb");
    if (!f_in) {
        fprintf(stderr, "Alert: Error loading <%s!\n", argv[1]);
        return 1;
    }

    f_out = fopen(output_file, "w");
    if (!f_out) {
        fprintf(stderr, "Alert: Error loading output file!\n");
        fclose(f_in);
        return 1;
    }
    
	ReadingAnimation(); //Funne stuff xD.
    //printf("Reading...\n");
    GetAssetType();
}