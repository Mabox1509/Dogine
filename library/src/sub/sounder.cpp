//[INCLUDES]
#include <dogine/sounder.hpp>
#include <dogine/log.hpp>

#include <map>
#include <unordered_map>
#include <portaudio/portaudio.h>
#include <algorithm>
#include <mutex>

//[PRIVATE TYPES]
#define SAMPLE_RATE (44100)
typedef struct
{
    float left_phase;
    float right_phase;
} audiodat_t;
class AudioInstance
{
public:
    std::shared_ptr<Dogine::Audio> audio;  // Buffer compartido
    double position;                       // Posición actual en muestras flotantes
    double pitch;                          // Velocidad de reproducción
    float volume;                          // Volumen [0..1]
    float balance;                         // Balance [-1..1] (-1=izq, 0=centro, 1=der)
    bool loop;                             // Repetir al llegar al final
    mutable std::mutex mutex;              // Bloqueo para acceso seguro desde callback

    // Constructor (todos los parámetros son obligatorios)
    AudioInstance(std::shared_ptr<Dogine::Audio> _audio,
                  double _position,
                  double _pitch,
                  float _volume,
                  float _balance,
                  bool _loop)
        : audio(std::move(_audio)),
          position(_position),
          pitch(_pitch),
          volume(_volume),
          balance(_balance),
          loop(_loop)
    {}

};


//[PRIVATE VARIABLES]



constexpr int MAX_CHANNELS = 16;
audiodat_t g_data;
PaStream* g_stream = nullptr;

std::map<Dogine::Sounder::SoundID, AudioInstance> audio_instances;
Dogine::Sounder::SoundID next_id = 1;
std::mutex mutex;


//[PRIVATE FUNCTIONS]
static int PortaudioCallback(const void* _inputbuff, void* _outputbuff,
                             unsigned long _frames,
                             const PaStreamCallbackTimeInfo* _timeinf,
                             PaStreamCallbackFlags _status,
                             void* _userdat)
{
    audiodat_t* data = (audiodat_t*)_userdat;
    float* out = (float*)_outputbuff;
    (void)_inputbuff;
    (void)_timeinf;
    (void)_status;

    for(unsigned long i = 0; i < _frames; i++)
    {
        float mixL = 0.0f, mixR = 0.0f;
        //Iterate each audio instance
        {
            std::lock_guard<std::mutex> lock(mutex);
            for(auto it = audio_instances.begin(); it != audio_instances.end(); )
            {
                AudioInstance& _inst = it->second;
                std::lock_guard<std::mutex> inst_lock(_inst.mutex);

                if(!_inst.audio)
                {
                    it = audio_instances.erase(it);
                    continue;
                }

                // Get sample
                float _lsample = _inst.audio->GetSample(_inst.position, 0);
                float _rsample = (_inst.audio->GetChannels() > 1) ? _inst.audio->GetSample(_inst.position, 1) : _lsample;
                mixL += _lsample * _inst.volume * (_inst.balance <= 0 ? 1.0f : 1.0f - _inst.balance);
                mixR += _rsample * _inst.volume * (_inst.balance >= 0 ? 1.0f : 1.0f + _inst.balance);

                // Advance position
                _inst.position += _inst.pitch * _inst.audio->GetSampleRate() / SAMPLE_RATE;
                if(_inst.position >= _inst.audio->GetLengthSamples())
                {
                    if(_inst.loop)
                    {
                        _inst.position = static_cast<double>(_inst.audio->GetLoopStart());
                    }
                    else
                    {
                        it = audio_instances.erase(it);
                        continue;
                    }
                }

                ++it;
            }
        }

        // Clamp mix to [-1.0, 1.0]
        mixL = std::clamp(mixL, -1.0f, 1.0f);
        mixR = std::clamp(mixR, -1.0f, 1.0f);
        *out++ = mixL; // Left
        *out++ = mixR; // Right
    }


    return paContinue;
}

//[NAMESPACE]
namespace Dogine
{
    namespace Sounder
    {
        //[VARIALBES]
        

        //[FUNCTIONS]
        bool Init()
        {
            g_data.left_phase = 0.0f;
            g_data.right_phase = 0.0f;

            Log::Message("Loading audio engine...");

            PaError err = Pa_Initialize();
            if(err != paNoError)
            {
                Log::Message("PortAudio init error: %s", Pa_GetErrorText(err));
                return false;
            }

            // Setup output
            PaStreamParameters outputParameters;
            outputParameters.device = Pa_GetDefaultOutputDevice();
            if(outputParameters.device == paNoDevice)
            {
                Log::Message("No default output device.");
                return false;
            }

            outputParameters.channelCount = 2;           // stereo
            outputParameters.sampleFormat = paFloat32;   // float samples
            outputParameters.suggestedLatency =
                Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
            outputParameters.hostApiSpecificStreamInfo = nullptr;

            // Open stream
            err = Pa_OpenStream(&g_stream, nullptr, &outputParameters,
                                SAMPLE_RATE, 256, paClipOff,
                                PortaudioCallback, &g_data);
            if(err != paNoError)
            {
                Log::Message("Pa_OpenStream error: %s", Pa_GetErrorText(err));
                return false;
            }

            // Start stream
            err = Pa_StartStream(g_stream);
            if(err != paNoError)
            {
                Log::Message("Pa_StartStream error: %s", Pa_GetErrorText(err));
                return false;
            }

            Log::Message("PortAudio started successfully.");
            return true;
        }
        void Shutdown()
        {
            if(g_stream)
            {
                Pa_StopStream(g_stream);
                Pa_CloseStream(g_stream);
                g_stream = nullptr;
            }
            Pa_Terminate();
            Log::Message("PortAudio shutdown complete.");
        }
    
        SoundID Play(std::shared_ptr<Audio> _audio,
                     float _volume,
                     float _pitch,
                     bool _loop)
        {
            std::lock_guard<std::mutex> lock(mutex);

            // Generar ID único
            SoundID id = next_id++;

            audio_instances.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(id),
                std::forward_as_tuple(_audio, 0.0, _pitch, _volume, 0.0f, _loop)
            );

            return id;
        }
    
        void Stop(SoundID _id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            audio_instances.erase(_id);
        }

        // Parámetros básicos
        void SetVolume(SoundID _id, float _volume)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end())
            {
                std::lock_guard<std::mutex> inst_lock(it->second.mutex);
                it->second.volume = _volume;
            }
        }
        void SetPitch(SoundID _id, float _pitch)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end())
            {
                std::lock_guard<std::mutex> inst_lock(it->second.mutex);
                it->second.pitch = _pitch;
            }
        }
        void SetLooping(SoundID _id, bool _loop)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end())
            {
                std::lock_guard<std::mutex> inst_lock(it->second.mutex);
                it->second.loop = _loop;
            }
        }

        // Posición en tiempo
        void SetPosition(SoundID _id, double _seconds)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end() && it->second.audio)
            {
                std::lock_guard<std::mutex> inst_lock(it->second.mutex);
                it->second.position = _seconds * it->second.audio->GetSampleRate();
                // Clamp a rango válido
                if(it->second.position < 0) it->second.position = 0;
                if(it->second.position >= it->second.audio->GetLengthSamples())
                    it->second.position = it->second.audio->GetLengthSamples() - 1;
            }
        }  // Seek
        double GetPosition(SoundID _id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end())
            {
                std::lock_guard<std::mutex> inst_lock(it->second.mutex);
                return it->second.position / it->second.audio->GetSampleRate();
            }
            return 0.0;
        }                 // Cursor actual
        double GetDuration(SoundID _id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = audio_instances.find(_id);
            if(it != audio_instances.end() && it->second.audio)
                return static_cast<double>(it->second.audio->GetLengthSamples()) / it->second.audio->GetSampleRate();
            return 0.0;
        }                 // Largo total

        // Estado
        bool IsStopped(SoundID _id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            return audio_instances.find(_id) == audio_instances.end();
        }
    }
}
