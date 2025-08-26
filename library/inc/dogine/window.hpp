#pragma once

//[INCLUDES]
#include "./dogine.hpp"
#include <string>

//[NAMESPACE]
namespace Dogine
{
    namespace Window
    {
        //[FUNCTIONS]
        void Create(int _width, int _height, std::string _title);
        void Destroy();

        // Tamaño de la ventana
        void Resize(int _width, int _height);
        int GetWidth();
        int GetHeight();

        // Posición de la ventana
        void SetPosition(int _x, int _y);
        void Center(); // Centra la ventana en la pantalla
        void GetPosition(int& _x, int& _y);

        // Título
        void SetTitle(const std::string& _title);
        std::string GetTitle();

        // Fullscreen / Windowed
        void SetFullscreen(bool _enabled);
        bool IsFullscreen();

        // Mostrar / ocultar cursor
        void ShowCursor(bool _show);
        bool IsCursorVisible();

        // Swap buffers y poll events
        void SwapBuffers();
        void PollEvents();

        // Control de cierre
        bool ShouldClose();

        GLFWwindow* GetWindow();
        
    }
}