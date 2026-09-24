/**
 * @file main.cpp
 * @brief Main entry point for the Vulkan Render Engine application
 * @details This file integrates Qt GUI framework with Vulkan rendering engine,
 *          creating a window and handling the render loop through Qt's event system.
 */

#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QWindow>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "vulkan_engine.h"

/**
 * @brief Main application entry point
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit code (0 for success, -1 for failure)
 *
 * @details This function initializes the Qt application, creates a main window,
 *          retrieves the native window handle, initializes the Vulkan engine,
 *          and sets up a timer-based render loop.
 */
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.setWindowTitle("Qt + Vulkan Simple");
    w.resize(800, 600);
    w.show();

    // Get native window handle after show()
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

    // Qt timer-based render loop (simple approach, can be improved with multi-threading)
    QTimer timer;
    timer.setInterval(16); // ~60 FPS
    QObject::connect(&timer, &QTimer::timeout, [&engine]() {
        engine.renderLoop();
        });
    timer.start();

    return a.exec();
}
