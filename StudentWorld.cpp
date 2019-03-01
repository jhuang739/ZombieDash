#include "StudentWorld.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), penelope(nullptr), numCitizens(0)
{}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

// initializes game
int StudentWorld::init()
{
    numCitizens = 0;

    // load level
    Level lev(assetPath());
    
    string levelFile = "level0" + to_string(getLevel()) + ".txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    
    if (result == Level::load_fail_file_not_found || getLevel() == 100)
    {
        cerr << "Cannot find level01.txt data file" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted" << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::MazeEntry ge;
        for(int x = 0; x < 16; x++)
        {
            for(int y = 0; y < 16; y++)
            {
                // go through level file and add respective actors
                ge = lev.getContentsOf(x,y);
                switch (ge)
                {
                    case Level::empty:
                        cerr << "Location " << x << " " << y << " is empty" << endl;
                        break;
                    case Level::player:
                        cerr << "Location " << x << " " << y << " is where Penelope starts" << endl;
                        penelope = new Penelope(this, x*LEVEL_WIDTH,y*LEVEL_HEIGHT);
                        break;
                    case Level::wall:
                        cerr << "Location " << x << " " << y << " holds a Wall" << endl;
                        actors.push_back(new Wall(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::exit:
                        cerr << "Location " << x << " " << y << " holds an exit" << endl;
                        actors.push_back(new Exit(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::pit:
                        cerr << "Location " << x << " " << y << " holds a pit" << endl;
                        actors.push_back(new Pit(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::vaccine_goodie:
                        cerr << "Location " << x << " " << y << " holds a vaccine goodie" << endl;
                        actors.push_back(new VaccineGoodie(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::gas_can_goodie:
                        cerr << "Location " << x << " " << y << " holds a gas can goodie" << endl;
                        actors.push_back(new GasCanGoodie(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::landmine_goodie:
                        cerr << "Location " << x << " " << y << " holds a landmine goodie" << endl;
                        actors.push_back(new LandmineGoodie(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::citizen:
                        cerr << "Location " << x << " " << y << " holds a citizen" << endl;
                        actors.push_back(new Citizen(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        numCitizens++;
                        break;
                    case Level::dumb_zombie:
                        cerr << "Location " << x << " " << y << " holds a dumb zombie" << endl;
                        actors.push_back(new DumbZombie(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    case Level::smart_zombie:
                        cerr << "Location " << x << " " << y << " holds a smart zombie" << endl;
                        actors.push_back(new SmartZombie(this, x*LEVEL_WIDTH, y*LEVEL_HEIGHT));
                        break;
                    default:
                        cerr << "Location " << x << " " << y << " is another object" << endl;
                        break;
                }
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

// each tick of the game is a move call
int StudentWorld::move()
{
    // penelope gets to do something each tick
    penelope->doSomething();

    // all actors get a chance to do something
    for(int i = 0; i < actors.size(); i++)
    {
        if (actors[i] != nullptr && actors[i]->isAlive())
        {
            actors[i]->doSomething();
            
            if(!penelope->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if(penelope->atExit())
            {
                playSound(SOUND_LEVEL_FINISHED);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    // clean dead actors
    for(vector<Actor*>::iterator p = actors.begin(); p != actors.end();)
    {
        if(!(*p)->isAlive())
        {
            delete *p;
            p = actors.erase(p);
        }
        else
            p++;
    }
    
    // stringstream to display game information
    ostringstream oss;
    
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int vaccines = penelope->getNumVaccines();
    int flames = penelope->getNumFlameCharges();
    int landmines = penelope->getNumLandmines();
    int infectCount = penelope->getInfectionCount();
    
    oss.fill('0');
    oss << "Score: \t";
    if(score < 0)
        oss << "-" << setw(5) << -1*score;
    else
        oss << setw(6) << score;
    oss << "\t \t Level: \t \t" << level;
    oss << "\t \t Lives: \t" << lives;
    oss << "\t \t Vaccines: \t" << vaccines;
    oss << "\t \t Flames: \t" << flames;
    oss << "\t \t Mines: \t" << landmines;
    oss << "\t \t Infected: \t" << infectCount;
    
    string text = oss.str();
    setGameStatText(text);
    
    return GWSTATUS_CONTINUE_GAME;
}

// destroy all actors
void StudentWorld::cleanUp()
{
    for(int i = 0; i < actors.size(); i++)
    {
        delete actors[i];
        actors[i] = nullptr;
    }
    actors.clear();
    
    delete penelope;
    penelope = nullptr;
}

// find euclidean distance
int StudentWorld::getEuclidean(int x1, int y1, int x2, int y2) const
{
    int deltaX = x1-x2;
    int deltaY = y1-y2;
    int result = deltaX * deltaX + deltaY * deltaY;
    return result;
}

int StudentWorld::getNumCitizens() const { return numCitizens; }

void StudentWorld::addActor(Actor* a) { actors.push_back(a); }

void StudentWorld::recordCitizenGone() { numCitizens--; }

// checks overlap of ActivatingObjects on each actor in the StudentWorld
void StudentWorld::activateOnAppropriateActors(Actor* a)
{
    if(getEuclidean(a->getX(), a->getY(), penelope->getX(), penelope->getY()) <= 100)
    {
        if(penelope->isAlive())
            a->activateIfAppropriate(penelope);
    }
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(getEuclidean(a->getX(), a->getY(), actors[i]->getX(), actors[i]->getY()) <= 100)
        {
            if(actors[i]->isAlive())
                a->activateIfAppropriate(actors[i]);
        }
    }
}

// checks if agent movement blocked by other actors in StudentWorld
bool StudentWorld::isAgentMovementBlockedAt(double x, double y, Actor* itself) const
{
    int otherX, otherY;
    
    if(penelope->isAlive() && itself != penelope)
    {
        otherX = penelope->getX();
        otherY = penelope->getY();
        
        if(checkBoundaries(x, y, otherX, otherY))
            return true;
    }
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive() && actors[i]->canBlockMovement() && actors[i] != itself)
        {
            otherX = actors[i]->getX();
            otherY = actors[i]->getY();
            
            if(checkBoundaries(x, y, otherX, otherY))
                return true;
        }
    }
    
    return false;
}

// checks if (x,y) lies within another object's image based on
// the other object's (x,y) coordinates
bool StudentWorld::checkBoundaries(int x, int y, int otherX, int otherY) const
{
    if(x >= otherX && x <= otherX+SPRITE_WIDTH-1 &&
       y >= otherY && y <= otherY+SPRITE_HEIGHT-1)  // bottom left
        return true;
    
    if(x >= otherX && x <= otherX+SPRITE_WIDTH-1 &&
       y+SPRITE_HEIGHT-1 >= otherY && y+SPRITE_HEIGHT-1 <= otherY+SPRITE_HEIGHT-1)  // top left
        return true;
    
    if(x+SPRITE_WIDTH-1 >= otherX && x+SPRITE_WIDTH-1 <= otherX+SPRITE_WIDTH-1 &&
       y >= otherY && y <= otherY+SPRITE_HEIGHT-1)  // bottom right
        return true;
    
    if(x+SPRITE_WIDTH-1 >= otherX && x+SPRITE_WIDTH-1 <= otherX+SPRITE_WIDTH-1 &&
       y+SPRITE_HEIGHT-1 >= otherY && y+SPRITE_HEIGHT-1 <= otherY+SPRITE_HEIGHT-1)       // top right
        return true;
    return false;
}

// Checks if flames are blocked by other actors at (x,y)
bool StudentWorld::isFlameBlockedAt(double x, double y) const
{
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive() && actors[i]->canBlockFlame() && getEuclidean(x, y, actors[i]->getX(), actors[i]->getY()) <= 100)
            return true;
    }
    
    return false;
}

// checks if there is a Person that a zombie can vomit on at (x,y)
bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const
{
    if(penelope->isAlive() && getEuclidean(x, y, penelope->getX(), penelope->getY()) <= 100)
        return true;
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive() && actors[i]->triggersZombieVomit() && getEuclidean(x, y, actors[i]->getX(), actors[i]->getY()) <= 100)
            return true;
    }
    return false;
}

// checks if there is a Person within smart zombie's range to follow
// if it is within range, set closest Person coordinates to otherX, otherY
// and store Euclidean distance between actors in distance
bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
    bool found = false;
    
    distance = 6400;
    
    if(penelope->isAlive() && getEuclidean(x, y, penelope->getX(), penelope->getY()) <= distance)
    {
        distance = getEuclidean(x, y, penelope->getX(), penelope->getY());
        otherX = penelope->getX();
        otherY = penelope->getY();
        found = true;
    }
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive() && actors[i]->triggersZombieVomit() && getEuclidean(x, y, actors[i]->getX(), actors[i]->getY()) <= distance)
        {
            distance = getEuclidean(x, y, actors[i]->getX(), actors[i]->getY());
            otherX = actors[i]->getX();
            otherY = actors[i]->getY();
            found = true;
        }
    }
    return found;
}

// checks if there is Penelope or zombie in citizen's range to
// follow or run from, respectively if there is an actor in range,
// store closest trigger coordinates at otherX, otherY and store
// Euclidean distance between actors to distance. If the closest
// actor is a zombie, isThreat is set to false. If closest is
// penelope, isThreat is set to true
bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const
{
    bool found = false;
    
    if(penelope->isAlive())
    {
        otherX = penelope->getX();
        otherY = penelope->getY();
        distance = getEuclidean(x, y, penelope->getX(), penelope->getY());
        isThreat = false;
        found = true;
    }
    
    double zombieX = otherX;
    double zombieY = otherY;
    double zombiedist = distance;
    
    if(locateNearestCitizenThreat(x, y, zombieX, zombieY, zombiedist))
    {
        if(distance > zombiedist)
        {
            distance = zombiedist;
            otherX = zombieX;
            otherY = zombieY;
            isThreat = true;
            found = true;
        }
    }
    
    return found;
}

// checks if there is a zombie within citizen's range to run from
// if there is, stores closest zombie coordinates in otherX, otherY, and
// stores Euclidean distance between actors in distance
bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const
{
    bool found = false;
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive() && actors[i]->threatensCitizens())
        {
            if(getEuclidean(x, y, actors[i]->getX(), actors[i]->getY()) < distance)
            {
                otherX = actors[i]->getX();
                otherY = actors[i]->getY();
                distance = getEuclidean(x, y, actors[i]->getX(), actors[i]->getY());
                found = true;
            }
        }
    }
    
    return found;
}

// checks if any overlap occurs with any actor in the StudentWorld
bool StudentWorld::isThrownGoodieBlockedAt(double x, double y) const
{
    int otherX, otherY;
    
    if(penelope->isAlive())
    {
        otherX = penelope->getX();
        otherY = penelope->getY();
        
        if(checkBoundaries(x, y, otherX, otherY))
            return true;
    }
    
    for(int i = 0; i < actors.size(); i++)
    {
        if(actors[i]->isAlive())
        {
            otherX = actors[i]->getX();
            otherY = actors[i]->getY();
            
            if(checkBoundaries(x, y, otherX, otherY))
                return true;
        }
    }
    
    return false;
}
