#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector> //TODO(ilias): drop vector dependency
#include "stdlib.h"
#include "stdio.h"
#include "tools.h"
#include "texture.h"

using std::vector;

static u32 count_vertices(char* filename)
{
    u32 count = 0;
    FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error Opening File!\n"); //send to infoLog
		return 0;
	}
	
    char line[128];
	while(true)
	{
		i32 res = fscanf(file, "%s", line);
		if (res == EOF)break;
	    if (strcmp(line, "f") == 0)count+=3;	
    }
	return count;	
}

//TODO(ilias): investigate, for some reason some normals are {0,0,0}
static b32 load_obj(std::vector<vec3>& vertices_arr,std::vector<vec3>& normals_arr, std::vector<vec2>& uvs_arr,const char * filename)
{
	vector<vec3> temp_vertices;
	vector<vec3> temp_normals;
	vector<vec2> temp_uvs;
	
	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error Opening File!\n"); //send to infoLog
		return 0;
	}
	
	while(true)
	{
		char line[128];
		i32 res = fscanf(file, "%s", line);
		if (res == EOF)break;
		
		if (strcmp(line, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}else if (strcmp(line, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}else if (strcmp(line, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}else if (strcmp(line, "f") == 0) //NOTE(ilias): maybe ditch NEWLINE?? <<
		{
			//%d/%d/%d %d/%d/%d %d/%d/%d = {v1,v2,v3}, vx = {vertex, normal, uv}
			u32 vertices[3],normals[3], uvs[3];
			i32 matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertices[0], &uvs[0], &normals[0], &vertices[1],
																		&uvs[1], &normals[1], &vertices[2],
																		&uvs[2], &normals[2]);										
			if (matches != 9)//NOTE(ilias): meshes should be triangulated.. GL_QUADS is not supported
			{
				printf("obj format not supported.. sorry man!!\n");
				return 0;
			}
			vertices_arr.push_back(temp_vertices[vertices[0]-1]);
			vertices_arr.push_back(temp_vertices[vertices[1]-1]);
			vertices_arr.push_back(temp_vertices[vertices[2]-1]);
			uvs_arr.push_back(temp_uvs[uvs[0]-1]);
			uvs_arr.push_back(temp_uvs[uvs[1]-1]);
			uvs_arr.push_back(temp_uvs[uvs[2]-1]);
			normals_arr.push_back(temp_normals[normals[0]-1]);
			normals_arr.push_back(temp_normals[normals[1]-1]);
			normals_arr.push_back(temp_normals[normals[2]-1]);
		}
	}
    return 1;	
}
typedef struct ModelRaw{
    GLuint vao;
    //Shader s;
    vertex* vertices;
    u32 vertices_count;
    //Material m;
    Texture* diff;
    Texture* spec;
    String diff_name;
    String spec_name;
}ModelRaw;




//maybe supply a version with pointers to pos/normal/tex_coord arrays so as to not lose so much space allocating
//NOTE: the mesh MUST be triangulated
static ModelRaw 
load_obj(char * filename){
    ModelRaw res;
    res.vertices_count = count_vertices(filename); 
    vertex *vertices = (vertex*)arena_alloc(&global_platform.permanent_storage, sizeof(vertex) * res.vertices_count);
    u32 vertex_index = 0;

    //at most they will have vertices_count vec3's 
    vec3* positions = (vec3*)arena_alloc(&global_platform.frame_storage, sizeof(vec3) * res.vertices_count);
    u32 positions_count = 0;
    vec3* normals = (vec3*)arena_alloc(&global_platform.frame_storage, sizeof(vec3) * res.vertices_count);
    u32 normals_count = 0;
    vec2* tex_coords= (vec2*)arena_alloc(&global_platform.frame_storage, sizeof(vec2) * res.vertices_count);
    u32  tex_coords_count = 0;

	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Error Opening File!\n"); //send to infoLog
		return {};
	}
	
	while(true)
	{
		char line[128];
		i32 res = fscanf(file, "%s", line);
		if (res == EOF)break;
		
		if (strcmp(line, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            positions[positions_count++] = vertex;
		}else if (strcmp(line, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
            tex_coords[tex_coords_count++] = uv;
		}else if (strcmp(line, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            normals[normals_count++] = normal;
		}else if (strcmp(line, "f") == 0) //NOTE(ilias): maybe ditch NEWLINE?? <<
		{
            i32 positions_index[3],normals_index[3], uvs_index[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &positions[0], &uvs_index[0], &normals_index[0], &positions_index[1],
																		&uvs_index[1], &normals_index[1], &positions_index[2],
																		&uvs_index[2], &normals_index[2]);	

            vertex to_add = vert(positions[positions_index[0]], normals[normals_index[0]], tex_coords[uvs_index[0]]);
            vertices[vertex_index++] = to_add;
            to_add = {positions[positions_index[1]], normals[normals_index[1]], tex_coords[uvs_index[1]]};
            vertices[vertex_index++] = to_add;
            to_add = {positions[positions_index[2]], normals[normals_index[2]], tex_coords[uvs_index[2]]};
            vertices[vertex_index++] = to_add;
        }
    }
    res.vertices = vertices;

    return res;
}







#endif
