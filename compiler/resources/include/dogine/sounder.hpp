#pragma once

//[INCLUDES]
#include "./dogine.hpp"
#include <memory>

//[NAMESPACE]
namespace Dogine
{
    namespace Sounder
    {
        // Tipo fuerte para IDs de sonido
        using SoundID = int;

        //[CORE API]

        // Inicializar / cerrar backend
        bool Init();
        void Shutdown();

        // Reproducir sonido desde un asset
        SoundID Play(std::shared_ptr<Audio> _audio,
                    float _volume,
                    float _pitch,
                    bool _loop);

        // Control de reproducción
        void Stop(SoundID _id);

        // Parámetros básicos
        void SetVolume(SoundID _id, float _volume);
        void SetPitch(SoundID _id, float _pitch);
        void SetLooping(SoundID _id, bool _loop);

        // Posición en tiempo
        void SetPosition(SoundID _id, double _seconds);   // Seek
        double GetPosition(SoundID _id);                 // Cursor actual
        double GetDuration(SoundID _id);                 // Largo total

        // Estado
        bool IsStopped(SoundID _id);
    }
}