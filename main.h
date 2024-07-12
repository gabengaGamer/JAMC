#ifndef MAIN_H
#define MAIN_H

int jamc_preparation(int argc, char *argv[]);

int jamc_finalization(int argc, char *argv[]);

//=============================================================================
// STRUCTURIZING
//=============================================================================

    extern int index_offset_bytes_found;
    extern int index_cnt_offset_bytes_found;
    extern int vert_offset_bytes_found;
    extern size_t index_offset;
    extern size_t index_cnt_offset;
    extern size_t vert_offset;
    extern size_t vert_cnt_offset;

    extern unsigned char index_offset_bytes[8];
    extern unsigned char index_cnt_offset_bytes[4];
    extern unsigned char index_cnt_offset_hex_value[4];
    extern unsigned char vert_offset_bytes[8];

    extern size_t index_offset_end_pos;
    extern size_t index_offset_start_pos;
    extern size_t index_offset_length;
    extern size_t divided_index_offset_length;

    extern int index_offset_end_pos_null_counter;
    extern int index_offset_end_pos_null_counter_vault;
    extern size_t index_offset_end_null_pos;

    extern int vertex_offset_bones_null_count;
    extern unsigned char vertex_offset_bones_check_bytes[32];

    extern const char *input_file;
    extern const char *output_file;

    extern FILE *f_in;
    extern FILE *f_out;

    extern int total_null_count;

#endif // MAIN_H