#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QWindow>
#include <iostream>

#ifdef _WIN32
//#include <QtGui/qpa/qplatformnativeinterface.h>
#include <windows.h>
#endif

#include "vulkan_engine.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.setWindowTitle("Qt + Vulkan Sample");
    w.resize(800, 600);
    w.show();

    // 获取窗口句柄
    QWindow* window = w.windowHandle();
    if (!window)
    {
        std::cerr << "Failed to get window handle\n";
        return -1;
    }

#ifdef _WIN32
    HWND hwnd = reinterpret_cast<HWND>(window->winId());
#else
    void* hwnd = reinterpret_cast<void*>(window->winId());
#endif

    VulkanEngine engine;
    if (!engine.init(hwnd, 800, 600))
    {
        std::cerr << "Vulkan init failed\n";
        return -1;
    }

    QTimer timer;
    timer.setInterval(16);
    QObject::connect(&timer, &QTimer::timeout, [&engine]() {
        engine.renderLoop();
        });
    timer.start();

    return a.exec();
}
