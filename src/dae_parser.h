#ifndef DAEPARSER_H
#define DAEPARSER_H
#include "tools.h"

typedef struct AnimatedVertex
{
    vec3 position;
    vec3 normal;
    vec2 tex_coord;
    i32 joint_ids[3];
    vec3 weights;
}AnimatedVertex;

typedef struct MeshData{
    vec3* positions; 
    vec3* normals; 
    vec2* tex_coords; 
    vertex* verts;
    i32 vertex_count;
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
   FILE* file = fopen(filepath.data, "r");
   if (file == NULL)
   {
       printf("Error Opening .dae file!!\n");
       return {0};
   }
   char line[256];
   char garbage[256];
   char count[256];


   while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file
            //first we read the mesh positions array
        if (strcmp(line, "<float_array") == 0)
        {
            fscanf(file, "%s  %s", garbage, count);
            //count now has the count of the positions to come in the form 'count="2424"'
            i32 position_count = get_num_from_string(count);
            //make the position array of size position_count
            data.positions = (vec3*)arena_alloc(&global_platform.permanent_storage, position_count*sizeof(f32)); //position count is of floats??
            //now read the position data and put them on the array
            vec3 vec;
            for (i32 i = 0; i < position_count/3; ++i)
            {
                fscanf(file, "%f %f %f", &vec.x, &vec.y, &vec.z);
                data.positions[i] = vec;
            }
            break;
        }
   }
   while(true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file

        //then the normals
        if (strcmp(line, "<float_array") == 0)
        {
            fscanf(file, "%s %s", garbage, count);
            //count has the number of normals     
            i32 normal_count = get_num_from_string(count);
            //make the normal array of size normal_count
            data.normals = (vec3*)arena_alloc(&global_platform.permanent_storage, normal_count*sizeof(f32));
            //read the count data and put the on the array
            vec3 vec;
            for (i32 i = 0; i < normal_count/3; ++i)
            {
                fscanf(file, "%f %f %f", &vec.x, &vec.y, &vec.z);
                data.normals[i] = vec;
            }
            break;
        }
   }
   while(true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file

        //then the uv coordinates
        if (strcmp(line, "<float_array") == 0)
        {
            fscanf(file, "%s %s", garbage, count);
            //count the number of tex_coords
            i32 tex_count = get_num_from_string(count);
            //make the tex_coords array of size normal count
            data.tex_coords = (vec2*)arena_alloc(&global_platform.permanent_storage, tex_count * sizeof(f32)); 
            //read the tex_coords and put them in the array
            vec2 vec;
            for (i32 i = 0; i < tex_count/2; ++i)
            {
                fscanf(file, "%f %f", &vec.x, &vec.y);
                data.tex_coords[i] = vec;
            }
            break;
        }
   }
   //now read the vertex indices
    while(true)
    {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file

        if (strcmp(line, "<triangles") == 0)
        {
            fscanf(file, "%s", count);
            //count the triangles
            data.vertex_count = get_num_from_string(count) * 3;
            //make the vertex array triangle_count triangles big
            data.verts = (vertex*)arena_alloc(&global_platform.permanent_storage, sizeof(vertex) * data.vertex_count);
            break;
        }
    }

    //now read the vertex data
   while(true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file

        //then the uv coordinates
        if (strcmp(line, "<p>") == 0)
        {
            i32 vec[3];
            for (i32 i = 0; i < data.vertex_count; ++i)
            {
                fscanf(file, "%i %i %i", &vec[0], &vec[1], &vec[2]);
                data.verts[i] = {data.positions[vec[0]], data.normals[vec[1]], data.tex_coords[vec[2]]};
            }
            break;
        }
   }

   return data;
}

#endif
