#ifndef GAMEWORLD_H_
#define GAMEWORLD_H_

#include "GameConstants.h"
#include <string>

const int START_PLAYER_LIVES = 3;

class GameController;

class GameWorld
{
public:

    GameWorld(std::string assetPath)
     : m_lives(START_PLAYER_LIVES), m_score(0), m_level(1),
       m_controller(nullptr), m_assetPath(assetPath)
    {
    }

    virtual ~GameWorld()
    {
    }
    
    virtual int init() = 0;
    virtual int move() = 0;
    virtual void cleanUp() = 0;

    void setGameStatText(std::string text);

    bool getKey(int& value);
    void playSound(int soundID);

    int getLevel() const
    {
        return m_level;
    }

    int getLives() const
    {
        return m_lives;
    }
    
    void decLives()
    {
        m_lives--;
    }
    
    void incLives()
    {
        m_lives++;
    }

    int getScore() const
    {
        return m_score;
    }
    
    void increaseScore(int howMuch)
    {
        m_score += howMuch;
    }

    std::string assetPath() const
    {
        return m_assetPath;
    }
    
      // The following should be used by only the framework, not the student

    bool isGameOver() const
    {
        return m_lives == 0;
    }

    void advanceToNextLevel() 
    {
        ++m_level;
    }
   
    void setController(GameController* controller)
    {
        m_controller = controller;
    }
    
private:
    int m_lives;
    int m_score;
    int m_level;
    GameController* m_controller;
    std::string     m_assetPath;
};

#endif // GAMEWORLD_H_
