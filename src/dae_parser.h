#ifndef DAEPARSER_H
#define DAEPARSER_H
#include "tools.h"

typedef struct MeshData{
    vec3* positions; 
    vec3* normals; 
    vec2* tex_coords; 
    i32* joint_ids; 
    vec3* weights; 
    u32 size;

    i32* indices;
    u32 indices_size;
}MeshData;

static MeshData read_collada(String filepath)
{
   MeshData data = {0}; 

   //read the goddamn data

   return data;
}



#endif
