#include "program.h"
#include "checkError.h"
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

Program::Program()
{
	m_program = GL_CALL(glCreateProgram());
}

void Program::start()
{
	GL_CALL(glUseProgram(m_program));
}

void Program::end()
{
	GL_CALL(glUseProgram(0));
	//delete shader
	for (auto& shader : m_shaders)
	{
		GL_CALL(glDeleteShader(shader));
	}
}

void Program::build()
{
	//link program
	int success = 0;
	char info[1024];
	GL_CALL(glLinkProgram(m_program));
	GL_CALL(glGetProgramiv(m_program, GL_LINK_STATUS, &success));
	if (!success)
	{
		GL_CALL(glGetProgramInfoLog(m_program, 1024, NULL, info));
		std::cout << "LINK PROGRAM ERROR: " << "\n" << info << std::endl;
	}
}

void Program::attachShader(const int& type, const char* const src)
{
	GLuint shader;
	switch (type)
	{
	case SHADER_TYPE::VERTEX:
		shader = GL_CALL(glCreateShader(GL_VERTEX_SHADER));
		break;
	case SHADER_TYPE::FRAGMENT:
		shader = GL_CALL(glCreateShader(GL_FRAGMENT_SHADER));
		break;
	default:
		break;
	}

	//´«ÈëÔ´Âë
	GL_CALL(glShaderSource(shader, 1, &src, NULL));

	//build shader
	int success = 0;
	char info[1024];
	GL_CALL(glCompileShader(shader));
	GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
	if (!success)
	{
		GL_CALL(glGetShaderInfoLog(shader, 1024, NULL, info));
		std::cout << src << "\n" << info << std::endl;
	}

	//attach shader
	GL_CALL(glAttachShader(m_program, shader));
}

void Program::setUniform1f(const std::string& key, const float& value)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniform1f(location, value));
}

void Program::setUniform3f(const std::string& key, const float& x, const float& y, const float& z)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniform3f(location, x, y, z));
}

void Program::setUniform3fv(const std::string& key, const float* vec)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniform3fv(location, 1, vec));
}

void Program::setUniform1i(const std::string& key, const int& value)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniform1i(location, value));
}

void Program::setUniformMatrix4fv(const std::string& key, const glm::mat4& mat)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
}

void Program::setUniform4fv(const std::string& key, const float* vec)
{
	auto location = GL_CALL(glGetUniformLocation(m_program, key.c_str()));
	GL_CALL(glUniform4fv(location, 1, vec));
}
