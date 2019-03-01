#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    
    // initializes game
    virtual int init();
    
    // acts out one tick of game
    virtual int move();
    
    // cleans all members of game
    virtual void cleanUp();
    
    // Returns number of citizens in StudentWorld
    int getNumCitizens() const;
    
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // Record that one more citizen on the current level is gone (exited,
    // died, or turned into a zombie).
    void recordCitizenGone();
    
    // For each actor overlapping a, activate a if appropriate.
    void activateOnAppropriateActors(Actor* a);
    
    // Is an agent blocked from moving to the indicated location?
    bool isAgentMovementBlockedAt(double x, double y, Actor* itself) const;
    
    // Is creation of a flame blocked at the indicated location?
    bool isFlameBlockedAt(double x, double y) const;
    
    // Is there something at the indicated location that might cause a
    // zombie to vomit (i.e., a human)?
    bool isZombieVomitTriggerAt(double x, double y) const;
    
    // Return true if there is a living human, otherwise false.  If true,
    // otherX, otherY, and distance will be set to the location and distance
    // of the human nearest to (x,y).
    bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);
    
    // Return true if there is a living zombie or Penelope, otherwise false.
    // If true, otherX, otherY, and distance will be set to the location and
    // distance of the one nearest to (x,y), and isThreat will be set to true
    // if it's a zombie, false if a Penelope.
    bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;
    
    // Return true if there is a living zombie, false otherwise.  If true,
    // otherX, otherY and distance will be set to the location and distance
    // of the one nearest to (x,y).
    bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;
    
    // Does DumbZombie's thrown vaccine overlap with any other actor at (x,y)?
    bool isThrownGoodieBlockedAt(double x, double y) const;

private:
    // Does euclidean calculation
    int getEuclidean(int x1, int y1, int x2, int y2) const;
    
    // Checks if (x,y) falls within boundaries spanned by otherX, otherY,
    // otherX+SPRITE_WIDTH, and otherY+SPRITE_HEIGHT
    bool checkBoundaries(int x, int y, int otherX, int otherY) const;   // boundary check
    
    Penelope* penelope;             // penelope
    std::vector<Actor*> actors;     // stores actors
    int numCitizens;                // number of citizens remaining
};

#endif // STUDENTWORLD_H_
