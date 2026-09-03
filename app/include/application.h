#pragma once


#define _app Application::getInstance()

using ResizeCallback = void(*)(const int&, const int&);
using KeyCallback = void(*)(const int& key, const int& action, const int& mods);

class GLFWwindow;
class Application
{
public:
	static Application& getInstance()
	{
		static Application app;
		return app;
	}

	bool init(const int& width = 800, const int& height = 600);
	bool update();
	void destory();

	void setResizeCallback(ResizeCallback callback);
	void setKeyCallback(KeyCallback callback);

private:
	static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	Application() = default;
	~Application() = default;

private:
	int _width{ 0 };
	int _height{ 0 };
	GLFWwindow* _window{ nullptr };
	ResizeCallback _resizeCallback{ nullptr };
	KeyCallback _keyCallback{ nullptr };
};
