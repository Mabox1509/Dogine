//[INCLUDES]
#include "pch.h"
#include "framework.h"
#include "dogine.h"

#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//[VARIABLES]
GLFWwindow* main_window;
int width, height;

bool should_close;
int exit_code;



//[NAMESPACE]
namespace Dogine
{
    //[VARIABLES]
    Mesh* render_quad;

    std::function<void()> on_start;

    std::function<void(double _dt)> on_update;
    std::function<void(double _dt, int _w, int _h)> on_draw;
    std::function<void(double _dt, int _w, int _h, GLuint _output)> on_postdraw;

    int target_framerate;
    Surface* application_surface;


	//[FUNCTIONS]
	void Init(int _w, int _h, const char* _title = __FILE__)
	{
        target_framerate = 60;
        width = _w;
        height = _h;

        Log::Message("Loading GLFW...");
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


        Log::Message("Creating window...");
        main_window = glfwCreateWindow(width, height, _title, NULL, NULL);
        if (main_window == NULL)
        {
            Log::Error("Failed to create GLFW window");
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(main_window);
        gladLoadGL();


        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        CenterWindow();

        application_surface = new Surface(width, height);


        ResourcesInit();
        on_start();
        double _prev_frame = glfwGetTime();
        should_close = false;





        Log::Message("========GAME LOOP========");
        //GAME LOOP
        while (!glfwWindowShouldClose(main_window))
        {
            if (should_close)
                break;

           
            double _current_frame = glfwGetTime();
            double _delta_time = _current_frame - _prev_frame;
            _prev_frame = _current_frame;


            //UPDATE
            on_update(_delta_time);



            //RENDER
            application_surface->Bind();
            on_draw(_delta_time, application_surface->GetWidth(), application_surface->GetHeight());
            application_surface->Unbind();

            

            on_postdraw(_delta_time, width, height, application_surface->color_id);
            glfwSwapBuffers(main_window);
            //TAKE CARE OF GLFW EVENTS
            glfwPollEvents();
        }
        Log::Message("========GAME END========");


        glfwDestroyWindow(main_window);
        glfwTerminate();
        exit(exit_code);
	}
    void End(int _code)
    {
        should_close = true;
        exit_code = _code;
    }

    GLFWwindow* GetWindow()
    {
        return main_window;
    }
    int GetWindowWidth()
    {
        return width;
    }
    int GetWindowHeigth()
    {
        return height;
    }
    void SetWindowSize(int _w, int _h)
    {
        width = _w;
        height = _h;

        glfwSetWindowSize(main_window, width, height);
    }
    void CenterWindow()
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        int screenWidth = mode->width;
        int screenHeight = mode->height;
        int xPos = (screenWidth - width) / 2;
        int yPos = (screenHeight - height) / 2;
        glfwSetWindowPos(main_window, xPos, yPos);
    }
    void SetWindowName(const char* _title)
    {
        glfwSetWindowTitle(main_window, _title);
    }
}
