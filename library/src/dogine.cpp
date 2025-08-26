//[INCLUDES]
#include <dogine/dogine.hpp>
#include <dogine/log.hpp>
#include <dogine/window.hpp>
#include <dogine/input.hpp>
#include <dogine/resources.hpp>
#include <dogine/sounder.hpp>
#include <dogine/filesys.hpp>

//[PRIVATE VARIABLES]
const char version[] = "0.5.0";
int exit_code;
bool should_close = false;

std::string persistent_path;

//[PRIVATE FUNCTIONS]
void PersistentPath(const std::string& _name)
{
        std::string _base_path;

#if defined(_WIN32) || defined(_WIN64)
    // Windows
    const char* appdata = std::getenv("LOCALAPPDATA"); // o "APPDATA" si quieres roaming
    if(!appdata) appdata = "C:/"; // fallback
    base_path = std::string(appdata);
#elif defined(__linux__)
    // Linux
    const char* xdg = std::getenv("XDG_DATA_HOME");
    if(xdg) _base_path = xdg;
    else {
        const char* home = std::getenv("HOME");
        if(!home) home = "/tmp"; // fallback
        _base_path = std::string(home) + "/.local/share";
    }
#else
    #error "Sistema operativo no soportado"
#endif

    // Añadir el nombre de la carpeta del juego
    _base_path += "/" + _name;

    // Crear la carpeta si no existe
    if(!Dogine::FileSys::DirectoryExists(_base_path))
        Dogine::FileSys::CreateDirectory(_base_path);

    persistent_path = _base_path;
}

//[NAMESPACE]
namespace Dogine
{
    //[VARIABLES]
    std::function<void()> OnStart;
    std::function<void(int)> OnEnd;

    std::function<void(double)> OnUpdate;
    std::function<void()> OnDraw;
    
    std::string save_name;

    //[FUNCTIONS]
    void Startup(int _width, int _height, const std::string& _title, const std::string& _savename)
    {
        PersistentPath(_savename); //Create folder for saves
        Log::Message("PERSISTEND PATH: %s", persistent_path.c_str());

        Log::Message("STARTING DOGINE - %s", version);

        //GLFW
        Log::Message("Loading glfw...");
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        //Window
        Window::Create(_width, _height, _title);

        //Glad & OpenGL
        gladLoadGL();
        glfwSwapInterval(1); //Vsync

        //Set viewport
        glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
        Window::Center();

        //Load assets
        Resources::Load();

        //Time
        double _prev_frame = glfwGetTime();

        //Start audio
        if(!Sounder::Init())
        {
            Log::Error("Cannot initialize audio system");
            Window::Destroy();
            glfwTerminate();
            exit(1);
        }

        Input::Init(Window::GetWindow());
        //Game loop
        OnStart();
        Log::Message("================GAME LOOP================");
        while (!Window::ShouldClose())
        {
            if(should_close)
                break;

            //TAKE CARE OF GLFW EVENTS
            Input::Update();

            //Update
            double _current_frame = glfwGetTime();
            double _dt = _current_frame - _prev_frame;
            OnUpdate(_dt); //Update event
            _prev_frame = _current_frame;
            
            
            //Draw
            OnDraw(); //Draw event
            Window::SwapBuffers();            
        }
        Log::Message("================GAME END=================");

        OnEnd(exit_code);

        Sounder::Shutdown();
        Window::Destroy();
        glfwTerminate();
        exit(exit_code);
    }

    void End(int _code)
    {
        should_close = true;
        exit_code = _code;
    }

    std::string GetPersistentPath(){ return persistent_path; }
} // namespace Dogine