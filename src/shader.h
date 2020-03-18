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
};

void use_shader(shader* shader)
{
    glUseProgram(shader->ID);
}

char * read_file(const char *filename){
    FILE *f = fopen("textfile.txt", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = (char*)malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0; 

    return (char*)string;
}

//TODO(ilias): redo in C99
i32 shader_load (shader* shader, const char * vertex_path, const char * fragment_path)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // open files
    vShaderFile.open(vertex_path);
    fShaderFile.open(fragment_path);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();		
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode   = vShaderStream.str();
    fragmentCode = fShaderStream.str();		



    //NOTE(ilias): fucking make them the C way!!
    const char* vShaderCode = vertexCode.c_str();//read_file(vertex_path);
    const char* fShaderCode = fragmentCode.c_str();//read_file(fragment_path); 
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
       
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
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
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 1;
    };
      
    
    // shader Program
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertex);
    glAttachShader(shader->ID, fragment);
    glLinkProgram(shader->ID);
    // print linking errors if any
    glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 1;
    }
      
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return 0;
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
