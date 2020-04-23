#pragma once
#include <vector> //TODO(ilias): drop vector dependency
#include "stdlib.h"
#include "stdio.h"
#include "tb.h"

using std::vector;

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
