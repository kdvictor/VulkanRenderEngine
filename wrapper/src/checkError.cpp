#include "checkError.h"
#include "core.h"
#include <string>
#include <assert.h>

void checkError()
{
	auto error_code = glGetError();
	std::string error = "";
	if (GL_NO_ERROR != error_code)
	{
		switch (error_code)
		{
		case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
		default:
			error = "UNKNOWN";
			break;
		}
		assert(false);
	}
}
