//[INCLUDES]
#include <dogine/window.hpp>
#include <dogine/log.hpp>


//[PRIVATE VARIABLES]
GLFWwindow* window = nullptr;
int width, height;
std::string title;

//[NAMESPACE]
namespace Dogine
{
    namespace Window
    {
        //[FUNCTIONS]
        void Create(int _width, int _height, std::string _title)
        {
            //Check
            if(window != nullptr)
            {
                Log::Error("Cannot initiallize the window twice");
                glfwTerminate();
                exit(1);
            }

            Log::Message("Creating window...");

            //Vars
            width = _width;
            height = _height;
            title = _title;

            //Start
            window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            if (window == NULL)
            {
                Log::Error("Failed to create GLFW window");
                glfwTerminate();
                exit(1);
            }

            glfwMakeContextCurrent(window);
        }
        void Destroy()
        {
            glfwDestroyWindow(window);
        }

        // Tamaño de la ventana
        void Resize(int _width, int _height)
        {
            width = _width;
            height = _height;

            glfwSetWindowSize(window, width, height);
        }
        int GetWidth()
        {
            return width;
        }
        int GetHeight()
        {
            return height;
        }

        // Posición de la ventana
        void SetPosition(int _x, int _y)
        {
            glfwSetWindowPos(window, _x, _y);
        }
        void Center()
        {
            //Get display size
            const GLFWvidmode* _mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            int _screen_width = _mode->width;
            int _screen_height = _mode->height;

            //Calculate pos
            int _xpos = (_screen_width - width) / 2;
            int _ypos = (_screen_height - height) / 2;

            //Set pos
            SetPosition(_xpos, _ypos);
        }
        void GetPosition(int& _x, int& _y)
        {
            glfwGetWindowPos(window, &_x, &_y);
        }


        // Título
        void SetTitle(const std::string& _title)
        {
            title = _title;
            glfwSetWindowTitle(window, title.c_str());
        }
        std::string GetTitle()
        {
            return title;
        }

        // Fullscreen / Windowed
        void SetFullscreen(bool _enabled){}
        bool IsFullscreen()
        {
            return false;
        }

        // Mostrar / ocultar cursor
        void ShowCursor(bool _show)
        {
            glfwSetInputMode(window, GLFW_CURSOR, _show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
        }
        bool IsCursorVisible()
        {
            return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
        }

        // Swap buffers y poll events
        void SwapBuffers()
        {
            glfwSwapBuffers(window);
        }
        void PollEvents()
        {
            glfwPollEvents();
        }

        // Control de cierre
        bool ShouldClose()
        {
            return glfwWindowShouldClose(window);
        }
        

        GLFWwindow* GetWindow()
        {
            return window;
        }

    } // namespace Window
} // namespace Dogine