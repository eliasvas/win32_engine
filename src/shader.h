#pragma once
#include "help.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#pragma once

struct shader
{
    GLuint ID;   
    const char* vertex_str;
    const char* fragment_str;
};

void use_shader(shader* shader)
{
    glUseProgram(shader->ID);
}

static GLuint load_shader_from_strings (const char * vertex_str, const char * fragment_str)
{ 
    GLuint ID;
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
       
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_str, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 1;
    };
      
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_str, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 666;
    };
      
    
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 666;
    }
      
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return ID;
}
static void shader_load (shader* s, const char * vertex_path, const char * fragment_path)
{
    s->vertex_str = vertex_path;
    s->fragment_str = fragment_path;
    const char* vs = read_file(vertex_path);
    const char* fs = read_file(fragment_path); 
    s->ID = load_shader_from_strings(vs,fs);

}

static void reload_shader_from_files( GLuint* program, const char* vertex_shader_filename, const char* fragment_shader_filename ) {
  assert( program && vertex_shader_filename && fragment_shader_filename );
  shader new_shader;
  shader_load(&new_shader,vertex_shader_filename, fragment_shader_filename );
  if ( new_shader.ID ) {
    glDeleteProgram( *program );
    *program = new_shader.ID;
  }
}
static void 
setBool(shader* shader, const std::string &name, bool value)
{         
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), (int)value); 
}
static void 
setInt(shader* shader, const std::string &name, i32 value)
{ 
    glUniform1i(glGetUniformLocation(shader->ID, name.c_str()), value); 
}
static void 
setFloat(shader * shader, const std::string &name, f32 value)
{ 
    glUniform1f(glGetUniformLocation(shader->ID, name.c_str()), value); 
} 
static void 
setMat4fv(shader * shader, const std::string &name, f32* value)
{ 
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name.c_str()),1,GL_FALSE, value);  //NOTE(ilias): check
}
