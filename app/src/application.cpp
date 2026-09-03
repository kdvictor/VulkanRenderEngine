#include "application.h"
#include "core.h"
#include <iostream>

bool Application::init(const int& width, const int& height)
{
	_width = width;
	_height = height;

	//初始化GLFW的基本环境
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); //兼容模式

	//创建窗体对象
	_window = glfwCreateWindow(_width, _height, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(_window);

	//使用glad加载当前版本的opengl函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "load opengl failed! " << std::endl;
		return false;
	}

	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << version << std::endl;

	//resize事件
	glfwSetFramebufferSizeCallback(_window, frameBufferSizeCallback);

	//key事件
	glfwSetKeyCallback(_window, keyCallback);

	//user pointer
	glfwSetWindowUserPointer(_window, this);

	return true;
}

bool Application::update()
{
	if (glfwWindowShouldClose(_window))
	{
		return false;
	}

	//接收并分发窗口消息
	glfwPollEvents();

	//渲染

	//切换双缓存
	glfwSwapBuffers(_window);

	return true;
}

void Application::destory()
{
	//退出程序
	glfwTerminate();
}

void Application::setResizeCallback(ResizeCallback callback)
{
	_resizeCallback = callback;
}

void Application::setKeyCallback(KeyCallback callback)
{
	_keyCallback = callback;
}

void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	if (nullptr != app->_resizeCallback)
	{
		app->_resizeCallback(width, height);
	}
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	if (nullptr != app)
	{
		app->_keyCallback(key, action, mods);
	}
}

