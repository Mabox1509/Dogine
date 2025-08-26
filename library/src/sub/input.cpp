//[INCLUDES]
#include <dogine/input.hpp>

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <iomanip>


//[PRIVATE VARIABLES]
GLFWwindow* window_context = nullptr;
namespace {
    // Estado actual y anterior del teclado (usando keycodes de GLFW)
    std::unordered_map<int, bool> keys_current;
    std::unordered_map<int, bool> keys_previous;

    // Estado actual y anterior de botones de mouse
    std::unordered_map<int, bool> mouse_current;
    std::unordered_map<int, bool> mouse_previous;

    // Posición del mouse
    double mouse_x = 0.0, mouse_y = 0.0;
    double mouse_prev_x = 0.0, mouse_prev_y = 0.0;

    // Scroll
    double scroll_offset = 0.0;
}

// Callback de scroll
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    scroll_offset += yoffset;
}

// Llamar al inicio de cada frame para actualizar estados
static void UpdateInputStates()
{
    // Teclado
    keys_previous = keys_current;
    for(int k = GLFW_KEY_SPACE; k <= GLFW_KEY_LAST; ++k)
        keys_current[k] = (glfwGetKey(window_context, k) == GLFW_PRESS);

    // Mouse
    mouse_previous = mouse_current;
    for(int b = GLFW_MOUSE_BUTTON_1; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
        mouse_current[b] = (glfwGetMouseButton(window_context, b) == GLFW_PRESS);

    // Mouse delta
    mouse_prev_x = mouse_x;
    mouse_prev_y = mouse_y;
    glfwGetCursorPos(window_context, &mouse_x, &mouse_y);
}

namespace Dogine
{
    namespace Input
    {
        void Init(GLFWwindow* _window)
        {
            window_context = _window;
            // Inicializar callbacks
            glfwSetScrollCallback(window_context, ScrollCallback);

            // Inicializar estados actuales
            UpdateInputStates();
        }

        // Debe llamarse al inicio de cada frame
        void Update()
        {
            glfwPollEvents();
            UpdateInputStates();
        }

        // Teclado
        bool IsKeyPressed(int _key)   { return keys_current[_key]; }
        bool IsKeyDown(int _key)      { return keys_current[_key] && !keys_previous[_key]; }
        bool IsKeyReleased(int _key)  { return !keys_current[_key] && keys_previous[_key]; }

        // Mouse
        bool IsMouseButtonPressed(int _button)  { return mouse_current[_button]; }
        bool IsMouseButtonDown(int _button)     { return mouse_current[_button] && !mouse_previous[_button]; }
        bool IsMouseButtonReleased(int _button) { return !mouse_current[_button] && mouse_previous[_button]; }

        void GetMousePosition(double& _x, double& _y)
        {
            _x = mouse_x;
            _y = mouse_y;
        }

        void GetMouseDelta(double& _dx, double& _dy)
        {
            _dx = mouse_x - mouse_prev_x;
            _dy = mouse_y - mouse_prev_y;
        }

        void SetMousePosition(double _x, double _y)
        {
            glfwSetCursorPos(window_context, _x, _y);
            mouse_x = _x;
            mouse_y = _y;
        }

        double GetScrollOffset()
        {
            double temp = scroll_offset;
            scroll_offset = 0.0; // se resetea cada frame
            return temp;
        }

    } // namespace Input
} // namespace Dogine
