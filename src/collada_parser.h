#ifndef COLLADA_PARSER_H
#define COLLADA_PARSER_H
#include "tools.h"



static b32 
vert_equals(vertex l, vertex r)
{
    i32 res = ( equalf(l.position.x, r.position.x, 0.001f) && equalf(l.position.y, r.position.y, 0.001f) && equalf(l.position.z, r.position.z, 0.001f)  && equalf(l.normal.x, r.normal.x, 0.001f) && equalf(l.normal.y, r.normal.y, 0.001f) && equalf(l.normal.z, r.normal.z, 0.001f) && equalf(l.tex_coord.x, r.tex_coord.x, 0.001f) && equalf(l.tex_coord.y, r.tex_coord.y, 0.001f));
    return res;
}

static MeshData 
read_collada(String filepath)
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
        //we find the <library_geometries> subsection of the collada file
        if (strcmp(line, "<library_geometries>") == 0)
        {
            break;
        }
   }

   //first we read the mesh positions array
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
            data.positions = (vec3*)arena_alloc(&global_platform.permanent_storage, position_count*sizeof(f32)); //position count is number of floats?? (yes)
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
            fscanf(file, "%s %s",garbage,  count);
            //count the triangles
            data.vertex_count = get_num_from_string(count) * 3;
            //make the vertex array triangle_count triangles big
            verts = (vertex*)arena_alloc(&global_platform.permanent_storage, sizeof(vertex) * data.vertex_count);
            data.verts = (vertex*)arena_alloc(&global_platform.permanent_storage, sizeof(vertex) * data.vertex_count);
            break;
        }
    }
    ivec3 * triangles = (ivec3*)arena_alloc(&global_platform.permanent_storage, sizeof(i32)*3 * data.vertex_count);
    i32 * skinning_index = (i32*)arena_alloc(&global_platform.permanent_storage, sizeof(i32) *data.vertex_count);

    //now read the vertex data
   while(true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)
            return data; //we reached the end of the file

        //then the uv coordinates
        if (strcmp(line, "<p>") == 0)
        {
            i32 vec[3];
            for (i32 i = 0; i < data.vertex_count; ++i)
            {
                fscanf(file, "%i %i %i", &vec[0], &vec[1], &vec[2]);
                triangles[i] = {vec[0], vec[1], vec[2]}; 
            }
            break;
        }
   }

    for (u32 i = 0; i < data.vertex_count; ++i)
    {
        data.verts[i] = {(vec3)data.positions[triangles[i].x], (vec3)data.normals[triangles[i].y], (vec2)data.tex_coords[triangles[i].z]};
        skinning_index[i] = triangles[i].x;
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
            joints_count = get_num_from_string(count);
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
        if (strstr(line, "bind_poses") != NULL)
        {
            fscanf(file, "%s %s %s", garbage,garbage, count);
            //count the number of tex_coords
            i32 floats_count = get_num_from_string(count);
            data.inv_bind_poses_count = floats_count;
            //make the tex_coords array of size normal count
            transforms = (mat4*)arena_alloc(&global_platform.permanent_storage, floats_count * sizeof(f32)); 
            //read the tex_coords and put them in the array
            mat4 mat;
            //for (i32 i = 0; i < floats_count/16; ++i)
            for (i32 i = 0; i < floats_count/16; ++i)
            {
                fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &mat.raw[0],&mat.raw[1],&mat.raw[2],&mat.raw[3],&mat.raw[4],&mat.raw[5],&mat.raw[6],&mat.raw[7],&mat.raw[8],&mat.raw[9],&mat.raw[10],&mat.raw[11],&mat.raw[12],&mat.raw[13],&mat.raw[14],&mat.raw[15]);

                //NOTE: we transpose because the matrices are given in row major order!!
                mat = transpose_mat4(mat);
                //transforms[i] = mul_mat4(mat, quat_to_mat4(quat_from_angle(v3(1,0,0), 90.f)));
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
                //weights = (f32*)arena_alloc(&global_platform.frame_storage, weights_count * sizeof(f32));
                weights = (f32*)arena_alloc(&global_platform.permanent_storage, weights_count * sizeof(f32));
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
   //the total number of weights for all vertices(?)
   i32 vertex_weights_count;
   while(!fin)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)return data;

        //we are inside the vertex weights source
        if (strcmp(line, "<vertex_weights") == 0)
        {
           fscanf(file, "%s", count); 
           vertex_weights_count = get_num_from_string(count);
           //vcount = (i32*)arena_alloc(&global_platform.frame_storage, sizeof(i32) * vertex_count);
           vcount = (i32*)arena_alloc(&global_platform.permanent_storage, sizeof(i32) * vertex_weights_count);
           while(!fin)
           {
               fscanf(file, "%s", line);
               //now we start reading the vertex number of weights
               if (strcmp(line,"<vcount>") == 0)
               {
                    for(i32 i = 0; i < vertex_weights_count; ++i)
                    {
                        fscanf(file, "%i", &vcount[i]);
                    }
                    fin = 1;
               }
           }
        }
   }
    

   //ivec3* vertex_joint_ids = (ivec3*)arena_alloc(&global_platform.permanent_storage, sizeof(ivec3) * vertex_count);
   ivec3* vertex_joint_ids = (ivec3*)arena_alloc(&global_platform.frame_storage, sizeof(ivec3) * data.vertex_count);
   //vec3* vertex_weights = (vec3*)arena_alloc(&global_platform.permanent_storage, sizeof(vec3) * vertex_count);
   vec3* vertex_weights = (vec3*)arena_alloc(&global_platform.frame_storage, sizeof(vec3) * data.vertex_count);
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
                    if (rat < 0.01f)continue;
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
   data.vertices = (AnimatedVertex*)arena_alloc(&global_platform.permanent_storage, sizeof(AnimatedVertex) * data.vertex_count);
   for (u32 i = 0; i < data.vertex_count; ++i)
   {
       //the skinning_index table is used as the 'index' to the vertex joint and vertex_weight arrays which are supposed to be
       //indexed by the index of the position which can be found on the <vertices> group 
       data.vertices[i] = {data.verts[i].position, data.verts[i].normal, data.verts[i].tex_coord, vertex_joint_ids[skinning_index[i]], vertex_weights[skinning_index[i]]};
   }

   //we find the root joint(s) (multiple root joints are not currently supported)
   //by reading the first <skeleton> type string
   String root;
   while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)
            return data;// we reached the end of the file
        
        if (strcmp(line, "<skeleton>") == 0)
        {
            fscanf(file, "%s", line); //name of the root joint

            root = str(&global_platform.frame_storage, line); 
            break;
        }
   }
   rewind(file);

   //now we know the root we should read the original transforms of every joint
   //we do this by reading the <library_visual_scenes> <node>s
   //if the node is of type "JOINT" it means its a child node..
   //if its of type "NODE" its a new root (<skeleton> thing), which we would like right now..

   //as for the hierarchy of joints.. each <node> we read is a child of the previous node that has
   //not been closed (by </node>)
  //Joint *root = arena_alloc(&global_platform.permanent_storage, sizeof(Joint));
  while (true)
   {
        i32 res = fscanf(file, "%s", line);
        if (res == EOF)
            return data;// we reached the end of the file
        
        if (strcmp(line, "<library_visual_scenes>") == 0)
        {
            //NOTE:read the "NODE" node
            fscanf(file, "%s %s %s", garbage, garbage, garbage);
            //read the only node info (we need to read joints_count data(??))
            //static String str(Arena* arena, char* characters)
            //" <node "
            fscanf(file, "%s", garbage);
            //" id = "ABC" "
            fscanf(file, "%s", line);
            line[str_size(line)-1] = '\0';
            String id = str(&global_platform.permanent_storage, (char*)(line + 4*sizeof(char))); 
            //" name= "ABC" "
            fscanf(file, "%s", line);
            line[str_size(line)-1] = '\0';
            String name = str(&global_platform.permanent_storage, (char*)(line + 6*sizeof(char))); 
            u32 joint_index = 0;
            //for (u32 i = 0; i < joints_count;++i)
            //{
            //    if (strcmp(id.data, joint_names[i].data) == 0)
            //        joint_index = i;
            //}
            fscanf(file, "%s", garbage); //THIS IS WETHER ITS JOINT OR NODE
            //now read the transform
            mat4 mat;
            fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &mat.raw[0],&mat.raw[1],&mat.raw[2],&mat.raw[3],&mat.raw[4],&mat.raw[5],&mat.raw[6],&mat.raw[7],&mat.raw[8],&mat.raw[9],&mat.raw[10],&mat.raw[11],&mat.raw[12],&mat.raw[13],&mat.raw[14],&mat.raw[15]);

            fscanf(file, "%s", garbage); // <\matrix>
            
            //NOTE:read the root joint
            fscanf(file, "%s", garbage);
            fscanf(file, "%s %s",garbage, garbage);
            //read the only node info (we need to read joints_count data(??))
            //static String str(Arena* arena, char* characters)
            //" <node "
            fscanf(file, "%s", garbage);
            //" id ="ABC" "
            fscanf(file, "%s", line);
            line[str_size(line)-1] = '\0';
            id = str(&global_platform.permanent_storage, (char*)(line + 4*sizeof(char))); 
            //" name="ABC" "
            fscanf(file, "%s", line);
            line[str_size(line)-1] = '\0';
            name = str(&global_platform.permanent_storage, (char*)(line + 6*sizeof(char))); 
            //" sid="ABC" "
            fscanf(file, "%s", line);
            line[str_size(line)-1] = '\0';
            String sid = str(&global_platform.permanent_storage, (char*)(line + 5*sizeof(char))); 

            joint_index = 0;
            for (u32 i = 0; i < joints_count;++i)
            {
                if (strcmp(id.data, joint_names[i].data) == 0)
                    joint_index = i;
            }
            fscanf(file, "%s", garbage); //THIS IS WETHER ITS JOINT OR NODE
            if (garbage[6] == 'N')memcpy(infoLog,"Multiple root bones not suppored for collada files.. sorry o_o", 64);
            //now read the transform
            fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &mat.raw[0],&mat.raw[1],&mat.raw[2],&mat.raw[3],&mat.raw[4],&mat.raw[5],&mat.raw[6],&mat.raw[7],&mat.raw[8],&mat.raw[9],&mat.raw[10],&mat.raw[11],&mat.raw[12],&mat.raw[13],&mat.raw[14],&mat.raw[15]);
            //Joint joint = joint(joint_index, name, mat);
            Joint root = joint(joint_index, name, mat);
            root.parent = &root;
            
            i32 open_nodes_count = 1; //we have read the "NODE" node
            Joint *current = &root;
            fscanf(file, "%s", garbage); // <\matrix>

            //NOTE: read the rest of the joints
            while (open_nodes_count > 0)
            {
                i32 res = fscanf(file, "%s", garbage);
                if (res == EOF)break;
                else if (strcmp(garbage, "</node>") == 0)
                {
                    open_nodes_count--;
                    current = current->parent;
                    continue;
                }else if (strcmp(garbage, "<node") ==0)open_nodes_count++;
                else continue;

                fscanf(file, "%s %s",garbage, garbage);
                //read the only node info (we need to read joints_count data(??))
                //static String str(Arena* arena, char* characters)
                //" <node "
                fscanf(file, "%s", garbage);
                //" id ="ABC" "
                fscanf(file, "%s", line);
                line[str_size(line)-1] = '\0';
                id = str(&global_platform.permanent_storage, (char*)(line + 4*sizeof(char))); 
                //" name="ABC" "
                fscanf(file, "%s", line);
                line[str_size(line)-1] = '\0';
                name = str(&global_platform.permanent_storage, (char*)(line + 6*sizeof(char))); 
                //" sid="ABC" "
                fscanf(file, "%s", line);
                line[str_size(line)-1] = '\0';
                sid = str(&global_platform.permanent_storage, (char*)(line + 5*sizeof(char))); 

                u32 joint_index = 0;
                for (u32 i = 0; i < joints_count;++i)
                {
                    if (strcmp(id.data, joint_names[i].data) == 0)
                        joint_index = i;
                }
                fscanf(file, "%s", garbage); //THIS IS WETHER ITS JOINT OR NODE
                //if (garbage[6] == 'N')memcpy(infoLog,"Multiple root bones not suppored for collada files.. sorry o_o", 64);
                //now read the transform
                fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &mat.raw[0],&mat.raw[1],&mat.raw[2],&mat.raw[3],&mat.raw[4],&mat.raw[5],&mat.raw[6],&mat.raw[7],&mat.raw[8],&mat.raw[9],&mat.raw[10],&mat.raw[11],&mat.raw[12],&mat.raw[13],&mat.raw[14],&mat.raw[15]);
                Joint j = joint(joint_index, name, mat);
                current->children.push_back(j);

                fscanf(file, "%s", garbage); // <\matrix>

            }










           break; 
        }
   }

   return data;
}

#endif
