#pragma once
#include <string>
#include <vector>

#include "core.h"
#include "glm/glm.hpp"

enum SHADER_TYPE
{
	VERTEX = 0,
	FRAGMENT
};

class Program
{
public:
	Program();
	~Program() = default;

public:
	void start();
	void end();

	void build();
	void attachShader(const int& type, const char* const src);

public:
	void setUniform1f(const std::string& key, const float& value);
	void setUniform3f(const std::string& key, const float& x, const float& y, const float& z);
	void setUniform3fv(const std::string& key, const float* vec);
	void setUniform1i(const std::string& key, const int& value);
	void setUniformMatrix4fv(const std::string& key, const glm::mat4& mat);
	void setUniform4fv(const std::string& key, const float* vec);

private:
	GLuint m_program;
	std::vector<GLuint> m_shaders;
};
