#include <vector>
#include <iostream>
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>
#include "Shader.hpp"

Shader::Shader() : loaded(0)
{

}

Shader::~Shader()
{

}

bool Shader::loadShader(std::string file_, int type_)
{
   #pragma warning(disable:4996)
   FILE* fp = fopen(file_.c_str(), "rt"); 
   #pragma warning(default:4996)
   if(!fp)
   {
      std::cout << "shader source file did not open!: " << file_ << "\n";
      return false; 
   }

   // Get all lines from a file
   std::vector<std::string> lines; 
   char line[255]; 
   while(fgets(line, 255, fp))
   {
      lines.push_back(line); 
   }
   fclose(fp); 

   const char** program = new const char*[(int)lines.size()]; 
   for(int i = 0; i < (int)lines.size(); ++i)
   {
#pragma warning(suppress: 6386)
      program[i] = lines[i].c_str(); 
   }

   shaderID = glCreateShader(type_);

   glShaderSource(shaderID, (int)lines.size(), program, NULL); 
   glCompileShader(shaderID); 

   delete[] program; 

   int iCompilationStatus; 
   glGetShaderiv(shaderID, GL_COMPILE_STATUS, &iCompilationStatus); 

   if(iCompilationStatus == GL_FALSE)
   {
      std::cout << "shader compile failed: " << iCompilationStatus << "\n";
      
      GLint blen = 0;	
      GLsizei slen = 0;
      glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH , &blen);       
      if (blen > 1)
      {
       GLchar* compiler_log = (GLchar*)malloc(blen);
       glGetShaderInfoLog(shaderID, blen, &slen, compiler_log);
       std::cout << "compiler_log: " << file_ << "\n" << compiler_log;
       free (compiler_log);
      }

      return false;
   }

   type = type_; 
   loaded = true;
   std::cout << "shader: " << file_ << " compiled succesfully\n";
   return true;
}

void Shader::deleteShader()
{
   if(!isLoaded())
   {
      return;
   }
	loaded = false;
	glDeleteShader(shaderID);
}

bool Shader::isLoaded()
{
   return loaded;
}

GLuint Shader::getShaderID()
{
   return shaderID;
}