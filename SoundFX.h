#ifndef SOUNDFX_H_
#define SOUNDFX_H_

#include <string>

#if defined(_MSC_VER)

#include "irrKlang/irrKlang.h"
#pragma comment(lib, "irrKlang.lib")
#include <iostream>

class SoundFXController
{
  public:

    void playClip(std::string soundFile)
    {
        if (m_engine != nullptr)
            m_engine->play2D(soundFile.c_str(), false);
    }

    void abortClip()
    {
        if (m_engine != nullptr)
            m_engine->stopAllSounds();
    }

    static SoundFXController& getInstance();

  private:
    irrklang::ISoundEngine* m_engine;

    SoundFXController()
    {
        m_engine = irrklang::createIrrKlangDevice();
        if (m_engine == nullptr)
            std::cout << "Cannot create sound engine!  Game will be silent."
                      << std::endl;
    }

    ~SoundFXController()
    {
        if (m_engine != nullptr)
            m_engine->drop();
    }

    SoundFXController(const SoundFXController&);
    SoundFXController& operator=(const SoundFXController&);
};

#elif defined(__APPLE__)

#include <memory>
#include <spawn.h>
#include <csignal>
#include <cstring>

class SoundFXController
{
  public:
    SoundFXController()
     : pidValid(false)
    {}

    void playClip(std::string soundFile)
    {
        char cmd[] = "/usr/bin/afplay";
        std::unique_ptr<char[]> fileName(new char[soundFile.size()+1]);
        std::strcpy(fileName.get(), soundFile.c_str());
        char* argv[] = { cmd, fileName.get(), nullptr };
        abortClip();  // stop anything currently playing
        pidValid = (posix_spawn(&pid, argv[0], nullptr, nullptr, argv, nullptr) == 0);
    }
    
    void abortClip()
    {
        if (pidValid)
            kill(pid, SIGINT);
        pidValid = false;
    }
    
    static SoundFXController& getInstance();

  private:
    pid_t pid;
    bool pidValid;
};

#else  // forget about sound

class SoundFXController
{
  public:
    void playClip(std::string) {}
    void abortClip() {}
    static SoundFXController& getInstance();
};

#endif

  // Meyers singleton pattern
inline SoundFXController& SoundFXController::getInstance()
{
    static SoundFXController instance;
    return instance;
}

inline SoundFXController& SoundFX()
{
    return SoundFXController::getInstance();
}

#endif // SOUNDFX_H_
