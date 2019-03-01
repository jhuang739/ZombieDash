#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SpriteManager.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>

const int INVALID_KEY = 0;

class GraphObject;
class GameWorld;

class GameController
{
  public:
    void run(int argc, char* argv[], GameWorld* gw, std::string windowTitle);

    bool getLastKey(int& value)
    {
        if (m_lastKeyHit != INVALID_KEY)
        {
            value = m_lastKeyHit;
            m_lastKeyHit = INVALID_KEY;
            return true;
        }
        return false;
    }

    void playSound(int soundID);

    void setGameStatText(std::string text)
    {
        m_gameStatText = text;
    }

    void doSomething();

    void reshape(int w, int h);
    void keyboardEvent(unsigned char key, int x, int y);
    void specialKeyboardEvent(int key, int x, int y);

    void quitGame();

      // Meyers singleton pattern
    static GameController& getInstance()
    {
        static GameController instance;
        return instance;
    }

private:
    enum GameControllerState : int;

    GameWorld*          m_gw;
    GameControllerState m_gameState;
    GameControllerState m_nextStateAfterPrompt;
    GameControllerState m_nextStateAfterAnimate;
    int         m_lastKeyHit;
    bool        m_singleStep;
    std::string m_gameStatText;
    std::string m_mainMessage;
    std::string m_secondMessage;
    int         m_curIntraFrameTick;
    using SoundMapType = std::map<int, std::string>;
    using DrawMapType =  std::map<int, std::string>;
    SoundMapType  m_soundMap;
    bool          m_playerWon;
    SpriteManager m_spriteManager;

    void setGameState(GameControllerState s);
    void setGameStateAfterPrompting(GameControllerState s,
                            std::string mainMessage, std::string secondMessage);

    void initDrawersAndSounds();
    void displayGamePlay();
};

inline GameController& Game()
{
    return GameController::getInstance();
}

#endif // GAMECONTROLLER_H_
