#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// IDs for the game objects

const int IID_PLAYER               =  0;
const int IID_ZOMBIE               =  1;
const int IID_CITIZEN              =  2;
const int IID_FLAME                =  3;
const int IID_VOMIT                =  4;
const int IID_PIT                  =  5;
const int IID_LANDMINE             =  6;
const int IID_VACCINE_GOODIE       =  7;
const int IID_GAS_CAN_GOODIE       =  8;
const int IID_LANDMINE_GOODIE      =  9;
const int IID_EXIT                 = 10;
const int IID_WALL                 = 11;

// sounds

const int SOUND_PLAYER_FIRE      =  0;
const int SOUND_PLAYER_DIE       =  1;
const int SOUND_ZOMBIE_BORN      =  2;
const int SOUND_ZOMBIE_VOMIT     =  3;
const int SOUND_ZOMBIE_DIE       =  4;
const int SOUND_CITIZEN_INFECTED =  5;
const int SOUND_CITIZEN_SAVED    =  6;
const int SOUND_CITIZEN_DIE      =  7;
const int SOUND_GOT_GOODIE       =  8;
const int SOUND_LANDMINE_EXPLODE =  9;
const int SOUND_LEVEL_FINISHED   = 10;
const int SOUND_THEME            = 11;

const int SOUND_NONE             = -1;


// keys the user can hit

const int KEY_PRESS_LEFT  = 1000;
const int KEY_PRESS_RIGHT = 1001;
const int KEY_PRESS_UP    = 1002;
const int KEY_PRESS_DOWN  = 1003;
const int KEY_PRESS_SPACE = ' ';
const int KEY_PRESS_TAB   = '\t';
const int KEY_PRESS_ENTER = '\r';

// view, sprite, and level data dimensions

const int VIEW_WIDTH = 256;
const int VIEW_HEIGHT = 256;

const int SPRITE_WIDTH = 16;
const int SPRITE_HEIGHT = 16;

const int LEVEL_WIDTH = VIEW_WIDTH / SPRITE_WIDTH;
const int LEVEL_HEIGHT = VIEW_HEIGHT / SPRITE_HEIGHT;

const double SPRITE_WIDTH_GL = .6; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .5; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming


// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED   = 0;
const int GWSTATUS_CONTINUE_GAME = 1;
const int GWSTATUS_PLAYER_WON    = 2;
const int GWSTATUS_FINISHED_LEVEL= 3;
const int GWSTATUS_LEVEL_ERROR   = 4;


  // Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
    if (max < min)
        std::swap(max, min);
    static std::random_device rd;
    static std::mt19937 generator(rd());
    std::uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

#endif // GAMECONSTANTS_H_
