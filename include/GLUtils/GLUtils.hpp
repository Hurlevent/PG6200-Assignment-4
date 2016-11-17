#ifndef _GLUTILS_HPP__
#define _GLUTILS_HPP__

#include <cstdlib>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "GLUtils/Program.hpp"
#include "GLUtils/VBO.hpp"
#include "GLUtils/FBO.hpp"
#include "GameException.h"


//Simple define that allows us to set buffer offsets for VBO rendering
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace GLUtils {

inline void checkGLErrors(const char* file, unsigned int line) {
	GLenum ASSERT_GL_err = glGetError(); 
    if( ASSERT_GL_err != GL_NO_ERROR ) { 
		std::stringstream ASSERT_GL_string; 
		ASSERT_GL_string << file << '@' << line << ": OpenGL error:" 
             << std::hex << ASSERT_GL_err << " " << gluErrorString(ASSERT_GL_err); 
			 THROW_EXCEPTION( ASSERT_GL_string.str() ); 
    } 
}
#define CHECK_GL_ERROR() GLUtils::checkGLErrors(__FILE__, __LINE__)


inline std::string readFile(std::string file) {
	int length;
	std::string buffer;
	std::string contents;

	std::ifstream is;
	is.open(file.c_str());

	if (!is.good()) {
		std::string err = "Could not open ";
		err.append(file);
		THROW_EXCEPTION(err);
	}

	// get length of file:
	is.seekg (0, std::ios::end);
	length = static_cast<int>(is.tellg());
	is.seekg (0, std::ios::beg);

	// reserve memory:
	contents.reserve(length);

	// read data
	while(getline(is,buffer)) {
		contents.append(buffer);
		contents.append("\n");
	}
	is.close();

	return contents;
}

inline std::string mat4toString(glm::mat4 matrix) {
	std::stringstream ss;
	ss << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << "\n"
		<< matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << "\n"
		<< matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << "\n"
		<< matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << "\n";
	return ss.str();
}
}; //Namespace GLUtils

#endif