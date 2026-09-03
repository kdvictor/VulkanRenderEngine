#pragma once
#include <functional>
#include "core.h"

char* LoadFileContent(const char* const& filePath);

GLuint CreateGPUBufferObject(GLenum targetType, GLsizeiptr size, GLenum usage, const void* data = nullptr);

GLuint CreatVAO(std::function<void()> setting);

GLuint CreateFrameBufferObject(GLuint& colorBuffer, GLuint& depthBuffer, const int& width, const int& height, GLuint* colorBuffer2 = nullptr);

GLuint CreateTexture(const char* const& filePath);

GLuint Create3DTexture(const int& width, const int& height, const int& depth);

unsigned char* LoadBMP(const char* const& path, int& width, int& height);

void SavePixelDataToBMP(
	const char* const& filePath,
	unsigned char* const& pixelData,
	const int& width,
	const int& height);

GLuint ReverseColor(const char* const& filePath);

void CheckGLError(const char*const& pFile, const int& line);
#define GL_CHECK(x) \
do\
{\
	x;\
	CheckGLError(__FILE__, __LINE__);\
} while (false);

