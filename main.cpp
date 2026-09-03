#include <iostream>
#include <vector>
#include "core.h"
#include "shader.h"
#include "checkError.h"
#include "application.h"
#include "program.h"
#include "glm/mat4x4.hpp"
#include "glm/common.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "objmodel.h"

GLuint vao;
Program* program;
int elementNum = 0;
ObjModel model;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void OnKeyChange(const int& key, const int& action, const int& mods)
{
	//GLFW_PRESS,GLFW_RELEASE,GLFW_MOD_CONTROL,GLFW_MOD_SHIFT
	if (key == GLFW_KEY_W)
	{
		std::cout << "key: " << key << std::endl;
		std::cout << "action: " << action << std::endl;
		std::cout << "mods: " << mods << std::endl;
	}
}


void onResize(const int& width, const int& height)
{
	GL_CALL(glViewport(0, 0, width, height));
	std::cout << "onResize" << std::endl;
}

void buildProgram()
{
	program = new Program();
	program->attachShader(VERTEX, SPECULAR_VS);
	program->attachShader(FRAGMENT, SPECULAR_FS);
	program->build();
}

void prepareEBO()
{
	//read obj model
	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;
	unsigned int* indexes = nullptr;
	model.LoadObjModel("./asserts/model/obj/Sphere.obj", &indexes, vertexCount,indexCount);
	auto& vertexData = model.mVertexes;
	elementNum = indexCount;

	//vertex vbo
	GLuint vetexVbo;
	GL_CALL(glGenBuffers(1, &vetexVbo));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vetexVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VertexData), vertexData, GL_STATIC_DRAW));

	//ebo
	GLuint vertexEbo;
	GL_CALL(glGenBuffers(1, &vertexEbo));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexEbo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indexes, GL_STATIC_DRAW));

	//vao
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	//description
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vetexVbo));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));

	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))));

	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexEbo));//open ebo and bind to current vao

	GL_CALL(glBindVertexArray(0));
}

void render()
{
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
	program->start();
	glm::mat4 model = glm::translate<float>(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-5.0f));

	//glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	//glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
	glm::mat4 NM = glm::transpose(glm::inverse(glm::mat4( model)));

	program->setUniformMatrix4fv("M", model);
	program->setUniformMatrix4fv("V", view);
	program->setUniformMatrix4fv("P", projection);
	program->setUniformMatrix4fv("NM", NM);


	//light
	float ambientStrength = 0.1;
	float lightPos[3] = { 1.0f,1.0f,0.0f };
	float lightColor[4] = { 1.0f,1.0f,1.0f,1.0f };
	float objColor[4] = { 0.4f,0.4f,0.4f,1.0f };
	float camerPos[3] = { 0.0f,0.0f,0.0f };
	float shiness = 32.0f;
	program->setUniform1f("ambientStrength", ambientStrength);
	program->setUniform4fv("lightColor", lightColor);
	program->setUniform4fv("objColor", objColor);
	program->setUniform3fv("lightPos", lightPos);
	program->setUniform1f("shiness", shiness);
	program->setUniform3fv("camerPos", camerPos);

	//bind vao
	GL_CALL(glBindVertexArray(vao));

	//render
	//GL_CALL(glDrawElements(GL_TRIANGLES, elementNum, GL_UNSIGNED_INT, (void*)0));
	GL_CALL(glDrawElements(GL_TRIANGLES, elementNum, GL_UNSIGNED_INT, (void*)0));

	program->end();
}

int main()
{
	_app.init(800.600);
	
	//listen events
	_app.setResizeCallback(onResize);
	_app.setKeyCallback(OnKeyChange);

	//build program
	buildProgram();

	//prepare ebo
	prepareEBO();

	//texture

	//set viewport and color
	GL_CALL(glViewport(0, 0, 800, 600));
	GL_CALL(glClearColor(0.0, 0.0, 0.0, 1.0));
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//loop
	while (_app.update())
	{
		render();
	}

	_app.destory();

	return 0;
}
