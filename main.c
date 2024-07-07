//==============================================================================
//
//  Program:        JusticeAsuraModelConvertor | J.A.M.C.
//
//  Credits:        Modera
//
//  Author:         GameSpy
//
//  Date:           Started 05.07.2024
//
//==============================================================================

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
	
    int found = 0;
    size_t vert_offset = 0;
	size_t index_offset = 0;
    unsigned char vert_bytes[4];
	unsigned char index_bytes[8];

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
		fprintf(stderr, "Error loading input file\n", argv[0]);
        return 1;
    }

    f_out = fopen(output_file, "w");
    if (!f_out) {
		fprintf(stderr, "Error loading output file\n", argv[0]);
        fclose(f_in);
        return 1;
    }

//=============================================================================
// CONVERTING
//=============================================================================

	fprintf(stderr, "Processing\n", argv[0]);
	
    while (fread(vert_bytes, 1, 4, f_in) == 4) { //Finding beginning of vertices by pattern
        if (vert_bytes[0] == 0x00 &&
            vert_bytes[1] > 0x00 &&
            vert_bytes[2] > 0x00 &&
            vert_bytes[3] > 0x00) {
            found = 1;
            vert_offset += 1;
            break;
        }
        fseek(f_in, -3, SEEK_CUR); //Back to the beginning
        vert_offset++;
    }

	fseek(f_in, 0x10, SEEK_SET); //Temp solution aka hacki the hack
	fread(&vert_cnt, 1, sizeof(vert_cnt), f_in);
	
	//fseek(f_in, 0x2c, SEEK_SET); //Temp solution aka hacki the hack
	fseek(f_in, vert_offset, SEEK_SET); //Setting vertex reading position
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

	fseek(f_in, 0x14, SEEK_SET); //Temp solution aka hacki the hack
	fread(&index_cnt, 1, sizeof(index_cnt), f_in);
	
    while (fread(index_bytes, 1, 8, f_in) == 8) { //Finding beginning of faces by pattern
        if (index_bytes[0] == 0x00 &&
            index_bytes[1] == 0x00 &&
            index_bytes[2] == 0x01 &&
			index_bytes[3] == 0x00 &&
			index_bytes[4] == 0x02 &&
			index_bytes[5] == 0x00 &&
			index_bytes[6] == 0x02 &&
            index_bytes[7] == 0x00) {
            found = 1;
            break;
        }
        fseek(f_in, -7, SEEK_CUR); //Back to the beginning
        index_offset++;
    }
	
	//fseek(f_in, 0x6368, SEEK_SET); //Temp solution aka hacki the hack
	fseek(f_in, index_offset, SEEK_SET); //Setting faces reading position
	indices = malloc(index_cnt * sizeof(unsigned short));
	fread(indices, 1, sizeof(unsigned short)*index_cnt, f_in);
	
//=============================================================================
// MESH FIX UP
//=============================================================================

// export triangle strips
	pi = indices;
	v1 = pi[0]; 
	v2 = pi[1];
	
    for(i = 2; i < index_cnt; i++) {
        v3 = pi[i];

// skip degenerate faces
    if(v1 == v2 || v1 == v3 || v2 == v3)
       goto next_face;

// flip every second faces
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

	fprintf(stderr, "Conversion complete\n", argv[0]);
	
	return 0;	
}
