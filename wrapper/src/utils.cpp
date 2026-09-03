#include <windows.h>
#include <stdio.h>
#include "utils.h"


char* LoadFileContent(const char* const& filePath)
{
	if (filePath == nullptr)
	{
		printf("LoadFileContent():filePath is nullptr!\n");
		return nullptr;
	}
	FILE* pFile = nullptr;
	fopen_s(&pFile, filePath, "rb");
	if (pFile == nullptr)
	{
		printf("LoadFileContent():open file failed!\n");
		return nullptr;
	}

	fseek(pFile, 0, SEEK_END);
	int len = ftell(pFile);
	char* buffer = new char[len + 1];
	rewind(pFile);
	fread(buffer, len, 1, pFile);
	buffer[len] = '\0';
	fclose(pFile);

	//printf("%s\n", buffer);

	return buffer;
}

//
//GLuint CreateFrameBufferObject(GLuint& colorBuffer, GLuint& depthBuffer, const int& width, const int& height, GLuint* colorBuffer2)
//{
//	GLuint fbo;
//	glGenFramebuffers(1, &fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	//colorbuffer
//	glGenTextures(1, &colorBuffer);
//	glBindTexture(GL_TEXTURE_2D, colorBuffer);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	//绑定colorbuffer
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
//	if (colorBuffer2 != nullptr)
//	{
//		glGenTextures(1, colorBuffer2);
//		glBindTexture(GL_TEXTURE_2D, *colorBuffer2);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
//		glBindTexture(GL_TEXTURE_2D, 0);
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *colorBuffer2, 0);
//		GLenum buffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
//		glDrawBuffers(2, buffers);
//	}
//	//depthbuffer
//	glGenTextures(1, &depthBuffer);
//	glBindTexture(GL_TEXTURE_2D, depthBuffer);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (status != GL_FRAMEBUFFER_COMPLETE)
//	{
//		printf("create FBO failed!\n");
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	return fbo;
//}

const unsigned long FROMAT_DXT1 = 0x31545844l;//DXT1 -> 1 T X D 的asci码
static unsigned char* DecodeDXT(const char* const& fileContent, int& width, int& height, int& size)
{
	height = *(unsigned long*)(fileContent + sizeof(unsigned long) * 3);
	width = *(unsigned long*)(fileContent + sizeof(unsigned long) * 4);
	size = *(unsigned long*)(fileContent + sizeof(unsigned long) * 5);
	unsigned long compressFormat = *(unsigned long*)(fileContent + sizeof(unsigned long) * 21);
	switch (compressFormat)
	{
	case FROMAT_DXT1:
		printf("DXT1\n");

		break;
	default:
		break;
	}

	unsigned char* pPixelData = new unsigned char[size];
	memcpy(pPixelData, (fileContent + sizeof(unsigned long) * 32), size);

	return pPixelData;
}

GLuint CreateTexture(const char* const& filePath)
{
	if (filePath == nullptr)
	{
		printf("CreateTexture(): filepath is nullpt!\n");
		return -1;
	}
	char* pFileContent = LoadFileContent(filePath);
	if (pFileContent == nullptr)
	{
		printf("CreateTexture(): pFileContent is nullpt!\n");
		return -1;
	}

	int width = 0;
	int height = 0;
	int dxt1size = 0;
	unsigned char* piexelData = nullptr;
	GLenum format = GL_RGB;
	if (*((unsigned short*)pFileContent) == 0x4D42)
	{
		piexelData = LoadBMP(filePath, width, height);
	}
	else if (memcmp(pFileContent, "DDS ", 4) == 0) //压缩格式,有alpha通道
	{
		piexelData = DecodeDXT(pFileContent, width, height, dxt1size);
		//format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	}

	if (nullptr == piexelData)
	{
		printf("CreateTexture(): decode data failed!\n");
		return -1;
	}

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //超过的变成1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//if (format == GL_RGB)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, piexelData);
	//}
	//else if (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
	//{
	//	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, dxt1size, piexelData);
	//}

	//反色input image用
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, piexelData);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete piexelData;
	piexelData = nullptr;

	return textureId;
}

GLuint Create3DTexture(const int& width, const int& height, const int& depth)
{
	char* data = new char[width*height*depth];
	char* temp = data;
	for (int i = 0; i < width; ++i)
	{
		for (int ii = 0; ii < height; ++ii)
		{
			for (int iii=0; iii < depth; ++iii)
			{
				*temp++ = rand() & 0xff;
				*temp++ = rand() & 0xff;
				*temp++ = rand() & 0xff;
				*temp++ = rand() & 0xff;
			}
		}
	}

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_3D, textureId);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_3D,0, GL_RGBA8_SNORM, width, height, depth, 0, GL_RGBA, GL_BYTE, data);
	glBindTexture(GL_TEXTURE_3D, 0);
	delete[] data;
	data = nullptr;
	return textureId;
}

void SavePixelDataToBMP(
	const char* const& filePath,
	unsigned char* const& pixelData,
	const int& width,
	const int& height)
{
	FILE* pFile;
	fopen_s(&pFile, filePath, "wb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
		bfh.bfType = 0x4D42;
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);

		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(BITMAPINFOHEADER));
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biBitCount = 24;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);

		for (int i = 0; i < width * height * 3; i += 3)
		{
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i + 2] = pixelData[i] ^ pixelData[i + 2];
			pixelData[i] = pixelData[i] ^ pixelData[i + 2];
		}
		fwrite(pixelData, width * height * 3, 1, pFile);
	}

	fclose(pFile);
	return;
}

GLuint ReverseColor(const char* const& filePath)
{
	char* pFileContent = LoadFileContent(filePath);
	if (pFileContent == nullptr)
	{
		return -1;
	}

	int width = 0;
	int height = 0;
	unsigned char* piexelData = nullptr;
	piexelData = LoadBMP(filePath, width, height);
	if (piexelData == nullptr)
	{
		return -1;
	}

	#pragma omp parallel for
	for (int i = 0; i < width*height*3; ++i)
	{
		piexelData[i] = 255 - piexelData[i];
	}

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //超过的变成1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, piexelData);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureId;
}

unsigned char* LoadBMP(const char* const& path, int& width, int& height)
{
	unsigned char* imageData = nullptr;
	FILE* pFile;
	fopen_s(&pFile, path, "rb");
	if (pFile)
	{
		BITMAPFILEHEADER bfh;
		fread(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);
		if (bfh.bfType == 0x4D42) //0x4D42:bmp
		{
			BITMAPINFOHEADER bih;
			fread(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);
			width = bih.biWidth;
			height = bih.biHeight;
			int pixelCount = width * height * 3;
			imageData = new unsigned char[pixelCount];
			fseek(pFile, bfh.bfOffBits, SEEK_SET);
			fread(imageData, 1, pixelCount, pFile);

			unsigned char temp;
			for (int i = 0; i < pixelCount; i += 3)
			{
				imageData[i] = imageData[i] ^ imageData[i + 2];
				imageData[i + 2] = imageData[i] ^ imageData[i + 2];
				imageData[i] = imageData[i] ^ imageData[i + 2];
			}
		}
		fclose(pFile);
	}
	return imageData;
}