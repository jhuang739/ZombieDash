#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

const int MAX_INT = 2147483647;             // max integer constant

class StudentWorld;
class Penelope;
class Goodie;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* myWorld, int imageID, double x, double y, Direction dir, int depth, double size);
    
    // All Actors get to do something each tick
    virtual void doSomething() = 0;
    
    // If object can block movement
    virtual bool canBlockMovement() const;                  // default false
    
    // If object can block flames
    virtual bool canBlockFlame() const;                     // default false
    
    // If this is an activated object, perform its effect on a (e.g., for an
    // Exit have a use the exit).
    virtual void activateIfAppropriate(Actor* a);           // default return
    
    // If this object uses exits, use the exit.
    virtual void useExitIfAppropriate();                    // default return
    
    // If this object can die by falling into a pit or burning, die.
    virtual void dieByFallOrBurnIfAppropriate();            // default return
    
    // If this object can be infected by vomit, get infected.
    virtual void beVomitedOnIfAppropriate();                // default return
    
    // If this object can pick up goodies, pick up g
    virtual void pickUpGoodieIfAppropriate(Goodie* g);      // default return
    
    // Can this object cause a zombie to vomit?
    virtual bool triggersZombieVomit() const;               // default false
    
    // Does this object trigger landmines only when they're active?
    virtual bool triggersOnlyActiveLandmines() const;       // default false
    
    // Is this object a threat to citizens?
    virtual bool threatensCitizens() const;                 // default false
    
    // Accesses Actor's alive state
    bool isAlive() const;
    
    // Sets Actor alive state
    void setAlive(bool state);
    
    // Accesses Actor's StudentWorld
    StudentWorld* getWorld() const;
private:
    StudentWorld* world;
    bool alive;
};

class Wall : public Actor
{
public:
    Wall(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual bool canBlockMovement() const;
    virtual bool canBlockFlame() const;         
private:
};

class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* myWorld, int imageID, double x, double y, int depth, int dir);
    virtual void activateIfAppropriate(Actor* a) = 0;
};

class Exit : public ActivatingObject
{
public:
    Exit(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
    virtual bool canBlockFlame() const;
private:
};

class Pit : public ActivatingObject
{
public:
    Pit(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
};

class Flame : public ActivatingObject
{
public:
    Flame(StudentWorld* myWorld, double x, double y, int dir);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
private:
    int ticks;
};

class Vomit : public ActivatingObject
{
public:
    Vomit(StudentWorld* myWorld, double x, double y, int dir);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
private:
    int ticks;
};

class Landmine : public ActivatingObject
{
public:
    Landmine(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void activateIfAppropriate(Actor* a);
    virtual void dieByFallOrBurnIfAppropriate();
private:
    void explode();         // landmine explosion handling
    int safetyTicks;
};

class Goodie : public ActivatingObject
{
public:
    Goodie(StudentWorld* myWorld, int imageID, double x, double y);
    virtual void activateIfAppropriate(Actor* a);
    virtual void dieByFallOrBurnIfAppropriate();
    
    // Have p pick up this goodie.
    virtual void pickUp(Penelope* p) = 0;
};

class VaccineGoodie : public Goodie
{
public:
    VaccineGoodie(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
};

class GasCanGoodie : public Goodie
{
public:
    GasCanGoodie(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
};

class LandmineGoodie : public Goodie
{
public:
    LandmineGoodie(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void pickUp(Penelope* p);
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* myWorld, int imageID, double x, double y, int dir);
    virtual bool canBlockMovement() const;
    virtual bool triggersOnlyActiveLandmines() const;
};

class Person : public Agent
{
public:
    Person(StudentWorld* myWorld, int imageID, double x, double y);
    virtual void beVomitedOnIfAppropriate();
    virtual bool triggersZombieVomit() const;
    
    void clearInfection();              // resets infectionCount to 0
    int getInfectionCount() const;      // returns infectionCount
    int incrementInfectionCount();      // adds 1 to infectionCount
private:
    int infectionCount;
    bool exit;
};

class Penelope : public Person
{
public:
    Penelope(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    
    void increaseVaccines();            // increments numVaccines
    void increaseFlameCharges();        // increments numFlames
    void increaseLandmines();           // increments numMines
    int getNumVaccines() const;         // returns numVaccines
    int getNumFlameCharges() const;     // returns numFlames
    int getNumLandmines() const;        // returns numMines
    
    bool atExit() const;                // returns if penelope exited
    void setExit(bool state);           // sets penelope exit state
private:
    int numVaccines;
    int numMines;
    int numFlames;
    bool exit;
};

class Citizen : public Person
{
public:
    Citizen(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
    virtual void beVomitedOnIfAppropriate();
private:
    int ticks;
};

class Zombie : public Agent
{
public:
    Zombie(StudentWorld* myWorld, double x, double y);
    virtual bool threatensCitizens() const;
    void vomitIfPossible();                     // vomits on nearby actor if possible
    void zombieMove(int& movementPlan);         // handles zombie movement
};

class DumbZombie : public Zombie
{
public:
    DumbZombie(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int ticks;
    int movementPlan;
};

class SmartZombie : public Zombie
{
public:
    SmartZombie(StudentWorld* myWorld, double x, double y);
    virtual void doSomething();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int ticks;
    int movementPlan;
};

#endif // ACTOR_H_
