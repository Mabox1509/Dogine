#pragma once

//[INCLUDES]
#include "./dogine.hpp"

//[NAMESPACE]
namespace Dogine
{
    namespace Input
    {
        //[FUNCTIONS]
        void Init(GLFWwindow* _window);
        void Update();

        bool IsKeyPressed(int _key);      // Tecla presionada (una vez)
        bool IsKeyDown(int _key);         // Tecla mantenida
        bool IsKeyReleased(int _key);     // Tecla liberada (una vez)

        bool IsMouseButtonPressed(int _button);  // Botón presionado (una vez)
        bool IsMouseButtonDown(int _button);     // Botón mantenido
        bool IsMouseButtonReleased(int _button); // Botón liberado (una vez)

        void GetMousePosition(double& _x, double& _y); // Posición del ratón en ventana
        void GetMouseDelta(double& _dx, double& _dy);  // Delta de movimiento del ratón desde último frame
        void SetMousePosition(double _x, double _y);   // Mover el cursor a una posición en ventana

        double GetScrollOffset(); // Offset de scroll vertical desde último frame
    }
}