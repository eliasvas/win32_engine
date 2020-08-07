#ifndef DAEPARSER_H
#define DAEPARSER_H
#include "tools.h"

typedef struct AnimatedVertex
{
    vec3 position;
    vec3 normal;
    vec2 tex_coord;
    ivec3 joint_ids;
    vec3 weights;
}AnimatedVertex;

typedef struct MeshData{
    vec3* positions; 
    vec3* normals; 
    vec2* tex_coords; 
    vertex* verts; //just for rendering
    i32 vertex_count;
    i32* joint_ids; 
    vec3* weights; 
    u32 size;

    mat4* inv_bind_poses;
    i32 inv_bind_poses_count;

    AnimatedVertex* vertices;

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
   vertex* verts;
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
            verts = (vertex*)arena_alloc(&global_platform.permanent_storage, sizeof(vertex) * data.vertex_count);
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
                verts[i] = {data.positions[vec[0]], data.normals[vec[1]], data.tex_coords[vec[2]]};
                data.verts[i] = {data.positions[vec[0]], data.normals[vec[1]], data.tex_coords[vec[2]]};
            }
            break;
        }
   }

   //read the joint names array
   i32 joints_count;
   String* joint_names;
   while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data;// we reached the end of the file

        if (strcmp(line, "<Name_array") == 0)
        {
            fscanf(file, "%s", garbage);
            fscanf(file, "%s", count);
            i32 joints_count = get_num_from_string(count);
            joint_names = (String*)arena_alloc(&global_platform.frame_storage, sizeof(String) * joints_count);
            for (i32 i = 0; i< joints_count; ++i)
            {
                fscanf(file, "%s", line);
                joint_names[i] = str(&global_platform.frame_storage, line);
            }
            break;
        }
   }


   //now lets read the inv bind transforms
   mat4* transforms;
   while(true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data; //we reached the end of the file

        //then the uv coordinates
        if (strstr(line, "bind") != NULL)
        {
            fscanf(file, "%s %s %s", garbage,garbage, count);
            //count the number of tex_coords
            i32 floats_count = get_num_from_string(count);
            data.inv_bind_poses_count = floats_count;
            //make the tex_coords array of size normal count
            transforms = (mat4*)arena_alloc(&global_platform.permanent_storage, floats_count * sizeof(f32)); 
            //read the tex_coords and put them in the array
            mat4 mat;
            for (i32 i = 0; i < floats_count/16; ++i)
            {
                fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &mat.raw[0],&mat.raw[1],&mat.raw[2],&mat.raw[3],&mat.raw[4],&mat.raw[5],&mat.raw[6],&mat.raw[7],&mat.raw[8],&mat.raw[9],&mat.raw[10],&mat.raw[11],&mat.raw[12],&mat.raw[13],&mat.raw[14],&mat.raw[15]);

                //NOTE: we transpose because the matrices are given in row major order!!
                mat = transpose_mat4(mat);
                transforms[i] = mat;
            }
            break;
        }
   }
   data.inv_bind_poses = transforms;


   //now lets read the weights
   f32* weights = NULL;
   i32 weights_count;
   while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data;// we reached the end of the file

        if (strcmp(line, "<float_array") == 0)
        {
            fscanf(file, "%s", line);
            //if we are on the weights array
            if (strstr(line, "weights"))
            {
                //we read the count of weights
                fscanf(file, "%s", count);
                weights_count = get_num_from_string(count);
                weights = (f32*)arena_alloc(&global_platform.frame_storage, weights_count * sizeof(f32));
                for (i32 i = 0; i < weights_count; ++i)
                {
                   fscanf(file, "%f", &weights[i]); 
                }
                break;
            }
        }
   }

   //now lets read the vcount table
   i32* vcount = NULL;
   b32 fin = 0;
   i32 vertex_count;
   while(!fin)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data;

        //we are inside the vertex weights source
        if (strcmp(line, "<vertex_weights") == 0)
        {
           fscanf(file, "%s", count); 
           vertex_count = get_num_from_string(count);
           vcount = (i32*)arena_alloc(&global_platform.frame_storage, sizeof(i32) * vertex_count);
           while(!fin)
           {
               fscanf(file, "%s", line);
               //now we start reading the vertex number of weights
               if (strcmp(line,"<vcount>") == 0)
               {
                    for(i32 i = 0; i < vertex_count; ++i)
                    {
                        fscanf(file, "%i", &vcount[i]);
                    }
                    fin = 1;
               }
           }
        }
   }
    

   ivec3* vertex_joint_ids = (ivec3*)arena_alloc(&global_platform.permanent_storage, sizeof(ivec3) * vertex_count);
   //zero initialize weights???
   vec3* vertex_weights = (vec3*)arena_alloc(&global_platform.permanent_storage, sizeof(vec3) * vertex_count);
   while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data;// we reached the end of the file
        
        //we found the array of Joint/Weights
        if (strcmp(line, "<v>") == 0)
        {
            i32 JW[2];
            for (i32 i = 0; i < weights_count; ++i)
            {
                vertex_weights[i] =v3(0.f,0.f,0.f);
                for(i32 j = 0; j < vcount[i]; ++j)
                {
                    fscanf(file, "%i %i", &JW[0], &JW[1]);
                    f32 weight = weights[JW[1]];
                    //we fill the vertex joint/weight tables with the max weights
                    if (weight > vertex_weights[i].x)
                    {
                       vertex_weights[i].x = weight;
                       vertex_joint_ids[i].x = JW[0];
                    }else if (weight > vertex_weights[i].y)
                    {
                        vertex_weights[i].y = weight;
                        vertex_joint_ids[i].y = JW[0];
                    }else if (weight > vertex_weights[i].z)
                    {
                        vertex_weights[i].z = weight;
                        vertex_joint_ids[i].z = JW[0];
                    }
                    continue;
                    //NOTE: here we normalize in case we had more than 3 weights and they dont add up to 1!!
                    f32 rat = (vertex_weights[i].x + vertex_weights[i].y + vertex_weights[i].z)/1.f;
                    f32 to_add = 1.f - vertex_weights[i].x - vertex_weights[i].y - vertex_weights[i].z;
                    vertex_weights[i].x = (1.f/rat) * vertex_weights[i].x * to_add;
                    vertex_weights[i].y = (1.f/rat) * vertex_weights[i].y * to_add;
                    vertex_weights[i].z = (1.f/rat) * vertex_weights[i].z * to_add;
                }
            }
            break;
        }
   }
   //now lets make the final AnimatedVertex array
   data.vertices = (AnimatedVertex*)arena_alloc(&global_platform.permanent_storage, sizeof(AnimatedVertex) * vertex_count);
   for (u32 i = 0; i < vertex_count*3; ++i)
   {
        data.vertices[i] = {verts[i].position, verts[i].normal, verts[i].tex_coord, vertex_joint_ids[i], vertex_weights[i]};
   }


   return data;
}

#endif
