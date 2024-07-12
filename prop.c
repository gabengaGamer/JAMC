//=============================================================================
//
//  Asura prop converter.
//
//=============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "prop.h"

//=============================================================================
// CONVERTING
//=============================================================================
int jamc_prop_convertation(int argc, char *argv[])	
{
    unsigned i;
    unsigned vert_cnt;
    unsigned index_cnt;

    struct MYPROPVERTEX *verts;
    unsigned short *indices;

    int voffs = 1;
    unsigned short *pi;
    unsigned short v1, v2, v3;

    fprintf(stderr, "Converting...\n", argv[0]);

	fseek(f_in, vert_cnt_offset, SEEK_SET); //Setting vertex count.
	fread(&vert_cnt, 1, sizeof(vert_cnt), f_in);
	
	fseek(f_in, vert_offset, SEEK_SET); //Setting vertex reading position.
    verts = (struct MYPROPVERTEX *)malloc(sizeof(struct MYPROPVERTEX) * vert_cnt);
	fread(verts, 1, sizeof(struct MYPROPVERTEX)*vert_cnt, f_in);
	
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
    free(verts);
	free(indices);
	
    return jamc_finalization(argc, argv);
}