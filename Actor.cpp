#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

using namespace std;

// Actor implementation

Actor::Actor(StudentWorld* myWorld, int imageID, double x, double y, Direction dir = 0, int depth = 0, double size = 1.0)
: GraphObject(imageID, x, y, dir, depth, size), world(myWorld), alive(true)
{}

// default implementation to be changed by applicable actors

void Actor::activateIfAppropriate(Actor* a) { return; }

void Actor::useExitIfAppropriate() { return; }

void Actor::dieByFallOrBurnIfAppropriate() { return; }

void Actor::beVomitedOnIfAppropriate() { return; }

void Actor::pickUpGoodieIfAppropriate(Goodie* g) { return; }

bool Actor::triggersZombieVomit() const { return false; }

bool Actor::triggersOnlyActiveLandmines() const { return false; }

bool Actor::canBlockMovement() const { return false; }

bool Actor::canBlockFlame() const { return false; }

bool Actor::threatensCitizens() const { return false; }

bool Actor::isAlive() const { return alive; }

void Actor::setAlive(bool state) { alive = state; }

StudentWorld* Actor::getWorld() const { return world; }

// Wall implementation

Wall::Wall(StudentWorld* myWorld, double x, double y)
: Actor(myWorld, IID_WALL, x, y, right, 0)
{}

void Wall::doSomething() { return; }

bool Wall::canBlockMovement() const { return true; }

bool Wall::canBlockFlame() const { return true; }

// ActivatingObject implementation

ActivatingObject::ActivatingObject(StudentWorld* myWorld, int imageID, double x, double y, int depth, int dir)
: Actor(myWorld, imageID, x, y, depth, dir)
{}

// Exit implementation

Exit::Exit(StudentWorld* myWorld, double x, double y)
: ActivatingObject(myWorld, IID_EXIT, x, y, right, 1)
{}

void Exit::doSomething()
{
    if(!isAlive())
        return;
    
    getWorld()->activateOnAppropriateActors(this);
}

// allows appropriate actors to exit
void Exit::activateIfAppropriate(Actor* a)
{
    a->useExitIfAppropriate();
}

bool Exit::canBlockFlame() const { return true; }

// Pit implementation

Pit::Pit(StudentWorld* myWorld, double x, double y)
: ActivatingObject(myWorld, IID_PIT, x, y, right, 0)
{}

void Pit::doSomething()
{
    if(!isAlive())
        return;
    
    getWorld()->activateOnAppropriateActors(this);
}

// kills appropriate actors
void Pit::activateIfAppropriate(Actor* a)
{
    if(a->isAlive())
        a->dieByFallOrBurnIfAppropriate();
}

// Flame implementation

Flame::Flame(StudentWorld* myWorld, double x, double y, int dir)
: ActivatingObject(myWorld, IID_FLAME, x, y, dir, 0), ticks(0)
{}

void Flame::doSomething()
{
    if(!isAlive())
        return;
    
    // flames last for two ticks
    if(ticks > 2)
    {
        setAlive(false);
        return;
    }
    
    if(isAlive())
    {
        getWorld()->activateOnAppropriateActors(this);
        ticks++;
    }
}

// kill appropriate actors
void Flame::activateIfAppropriate(Actor* a)
{
    if(a->isAlive())
        a->dieByFallOrBurnIfAppropriate();
}

// Vomit implementation

Vomit::Vomit(StudentWorld* myWorld, double x, double y, int dir)
: ActivatingObject(myWorld, IID_VOMIT, x, y, dir, 0), ticks(0)
{}

void Vomit::doSomething()
{
    if(!isAlive())
        return;
    
    // vomit lasts two ticks
    if(ticks > 2)
    {
        setAlive(false);
        return;
    }
    
    if(isAlive())
    {
        getWorld()->activateOnAppropriateActors(this);
        ticks++;
    }
    
}

// infects appropriate actors
void Vomit::activateIfAppropriate(Actor* a)
{
    a->beVomitedOnIfAppropriate();
}

// Landmine implementation

Landmine::Landmine(StudentWorld* myworld, double x, double y)
: ActivatingObject(myworld, IID_LANDMINE, x, y, right, 1), safetyTicks(30)
{}

void Landmine::doSomething()
{
    if(!isAlive())
        return;
    
    // only activates after safety ticks expire
    if(safetyTicks > 0)
    {
        safetyTicks--;
        return;
    }
    
    getWorld()->activateOnAppropriateActors(this);
}

// blows up appropriate actors
void Landmine::activateIfAppropriate(Actor* a)
{
    if(a->triggersOnlyActiveLandmines())
        explode();
}

// blows up
void Landmine::dieByFallOrBurnIfAppropriate()
{
    explode();
}

void Landmine::explode()
{
    setAlive(false);
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    
    int x = getX();
    int y = getY();
    
    // calculate locations of adjacent flames
    int newX[9] = {
        x-SPRITE_WIDTH, x-SPRITE_WIDTH, x-SPRITE_WIDTH,
        x, x, x,
        x+SPRITE_WIDTH, x+SPRITE_WIDTH, x+SPRITE_WIDTH
    };
    
    int newY[9] = {
        y-SPRITE_HEIGHT, y, y+SPRITE_HEIGHT,
        y-SPRITE_HEIGHT, y, y+SPRITE_HEIGHT,
        y-SPRITE_HEIGHT, y, y+SPRITE_HEIGHT
    };
    
    for(int i = 0; i < 9; i++)              // generate flame at (x,y) and eight adjacent spots
    {
        if(!getWorld()->isFlameBlockedAt(newX[i], newY[i]))
            getWorld()->addActor(new Flame(getWorld(), newX[i], newY[i], up));
    }
    getWorld()->addActor(new Pit(getWorld(), getX(), getY()));      // pit after explosion
}

// Goodie implementation

Goodie::Goodie(StudentWorld* myWorld, int imageID, double x, double y)
: ActivatingObject(myWorld, imageID, x, y, right, 1)
{}

void Goodie::activateIfAppropriate(Actor* a)
{
    a->pickUpGoodieIfAppropriate(this);
}

void Goodie::dieByFallOrBurnIfAppropriate()
{
    setAlive(false);
}

// VaccineGoodie implementation

VaccineGoodie::VaccineGoodie(StudentWorld* myworld, double x, double y)
: Goodie(myworld, IID_VACCINE_GOODIE, x, y)
{}

void VaccineGoodie::doSomething()
{
    if(!isAlive())
        return;
    getWorld()->activateOnAppropriateActors(this);
}

// increase p's vaccine count
void VaccineGoodie::pickUp(Penelope* p)
{
    p->increaseVaccines();
}

// GasCanGoodie implementation

GasCanGoodie::GasCanGoodie(StudentWorld* myworld, double x, double y)
: Goodie(myworld, IID_GAS_CAN_GOODIE, x, y)
{}

void GasCanGoodie::doSomething()
{
    if(!isAlive())
        return;
    getWorld()->activateOnAppropriateActors(this);
}

// increase p's flame count
void GasCanGoodie::pickUp(Penelope* p)
{
    p->increaseFlameCharges();
}

// LandmineGoodie implementation

LandmineGoodie::LandmineGoodie(StudentWorld* myworld, double x, double y)
: Goodie(myworld, IID_LANDMINE_GOODIE, x, y)
{}

void LandmineGoodie::doSomething()
{
    if(!isAlive())
        return;
    getWorld()->activateOnAppropriateActors(this);
}

// increase p's mine count
void LandmineGoodie::pickUp(Penelope* p)
{
    p->increaseLandmines();
}

// Agent implementation

Agent::Agent(StudentWorld* myWorld, int imageID, double x, double y, int dir)
: Actor(myWorld, imageID, x, y, dir)
{}

bool Agent::canBlockMovement() const { return true; }

bool Agent::triggersOnlyActiveLandmines() const { return true; }

// Person implementation

Person::Person(StudentWorld* myWorld, int imageID, double x, double y)
: Agent(myWorld, imageID, x, y, right), infectionCount(0)
{}

// get infected
void Person::beVomitedOnIfAppropriate()
{
    infectionCount++;
}

bool Person::triggersZombieVomit() const { return true; }

void Person::clearInfection() { infectionCount = 0; }

int Person::getInfectionCount() const { return infectionCount; }

int Person::incrementInfectionCount() { return infectionCount++; }

// Penelope implementation

Penelope::Penelope(StudentWorld* myWorld, double x, double y)
: Person(myWorld, IID_PLAYER, x, y), numFlames(0), numMines(0), numVaccines(0), exit(false)
{}

void Penelope::doSomething()
{
    if(!isAlive())          // if Penelope is dead
        return;
    
    if(getInfectionCount() > 0)        // if Penelope is infected
    {
        incrementInfectionCount();
        
        if(getInfectionCount() >= 500)
        {
            setAlive(false);
            getWorld()->playSound(SOUND_PLAYER_DIE);
            return;
        }
    }
    
    int ch;
    if(getWorld()->getKey(ch))          // if user hits a key during tick
    {
        int dest_x, dest_y;
        switch (ch) {
            case KEY_PRESS_UP:          // move up
                setDirection(up);
                dest_x = getX();
                dest_y = getY() + 4;
                if(!getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                    moveTo(dest_x, dest_y);
                break;
            case KEY_PRESS_DOWN:        // move down
                setDirection(down);
                dest_x = getX();
                dest_y = getY() - 4;
                if(!getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                    moveTo(dest_x, dest_y);
                break;
            case KEY_PRESS_LEFT:        // move left
                setDirection(left);
                dest_x = getX() - 4;
                dest_y = getY();
                if(!getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                    moveTo(dest_x, dest_y);
                break;
            case KEY_PRESS_RIGHT:       // move right
                setDirection(right);
                dest_x = getX() + 4;
                dest_y = getY();
                if(!getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                    moveTo(dest_x, dest_y);
                break;
            case KEY_PRESS_TAB:         // plant landmine
                if(numMines > 0)
                {
                    getWorld()->addActor(new Landmine(getWorld(), getX(), getY()));
                    numMines--;
                }
                break;
            case KEY_PRESS_ENTER:       // use vaccine
                if(numVaccines > 0)
                {
                    clearInfection();
                    numVaccines--;
                }
                break;
            case KEY_PRESS_SPACE:       // spit flames
                if(numFlames > 0)
                {
                    int newX = getX();
                    int newY = getY();
                    
                    for(int i = 1; i <= 3; i++)
                    {
                        if(getDirection() == up)
                        {
                            newY += SPRITE_HEIGHT;
                        }
                        else if(getDirection() == down)
                        {
                            newY -= SPRITE_HEIGHT;
                        }
                        else if(getDirection() == right)
                        {
                            newX += SPRITE_WIDTH;
                        }
                        else if(getDirection() == left)
                        {
                            newX -= SPRITE_WIDTH;
                        }
                        
                        if(getWorld()->isFlameBlockedAt(newX, newY))        // stops flames when blocked
                            break;
                        getWorld()->addActor(new Flame(getWorld(), newX, newY, getDirection()));
                    }
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    numFlames--;
                }
                break;
            default:
                break;
        }
    }
}

void Penelope::useExitIfAppropriate()
{
    if(getWorld()->getNumCitizens() == 0)
        setExit(true);
}

void Penelope::dieByFallOrBurnIfAppropriate()
{
    setAlive(false);
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

// remove goodie and gain perks
void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
    g->pickUp(this);
    getWorld()->increaseScore(50);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    g->setAlive(false);
}

void Penelope::increaseVaccines() { numVaccines++; }

void Penelope::increaseFlameCharges() { numFlames += 5; }

void Penelope::increaseLandmines() { numMines += 2; }

int Penelope::getNumFlameCharges() const { return numFlames; }

int Penelope::getNumLandmines() const { return numMines; }

int Penelope::getNumVaccines() const { return numVaccines; }

bool Penelope::atExit() const { return exit; }

void Penelope::setExit(bool state) { exit = state; }

// Citizen implementation

Citizen::Citizen(StudentWorld* myWorld, double x, double y)
: Person(myWorld, IID_CITIZEN, x, y), ticks(0)
{}

void Citizen::doSomething()
{
    if(!isAlive())
        return;

    if(getInfectionCount() > 0)        // if citizen is infected
    {
        incrementInfectionCount();
        
        if(getInfectionCount() >= 500)
        {
            setAlive(false);
            getWorld()->playSound(SOUND_ZOMBIE_BORN);
            getWorld()->increaseScore(-1000);
            getWorld()->recordCitizenGone();
            if(randInt(1, 10) <= 7)                 // 70% chance of turning into dumb zombie
                getWorld()->addActor(new DumbZombie(getWorld(), getX(), getY()));
            else
                getWorld()->addActor(new SmartZombie(getWorld(), getX(), getY()));
            return;
        }
    }
    
    // paralyzed every other tick
    if(ticks % 2 == 0)
    {
        ticks++;
        return;
    }
    
    ticks--;
    
    double distance = MAX_INT;
    double otherX;
    double otherY;
    bool isThreat;
    
    int dest_x, dest_y;
    
    // locate closest trigger
    getWorld()->locateNearestCitizenTrigger(getX(), getY(), otherX, otherY, distance, isThreat);
    
    if(!isThreat && distance <= 6400)
    {
        // moves toward penelope
        if(getY() == otherY)
        {
            dest_x = getX() + 2;
            dest_y = getY();
            if(getX() < otherX && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
            {
                setDirection(right);
                moveTo(dest_x, dest_y);
                return;
            }
            dest_x = getX() - 2;
            dest_y = getY();
            if(getX() > otherX && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
            {
                setDirection(left);
                moveTo(dest_x, dest_y);
                return;
            }
        }
        else if(getX() == otherX)
        {
            dest_x = getX();
            dest_y = getY() + 2;
            if(getY() < otherY && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
            {
                setDirection(up);
                moveTo(dest_x, dest_y);
                return;
            }
            dest_x = getX();
            dest_y = getY() - 2;
            if(getY() > otherY && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
            {
                setDirection(down);
                moveTo(dest_x, dest_y);
                return;
            }
        }
        else
        {
            if(randInt(0, 1) == 0)  // horizontal
            {
                dest_x = getX() + 2;
                dest_y = getY();
                if(getX() < otherX && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                {
                    setDirection(right);
                    moveTo(dest_x, dest_y);
                    return;
                }
                dest_x = getX() - 2;
                dest_y = getY();
                if(getX() > otherX && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                {
                    setDirection(left);
                    moveTo(dest_x, dest_y);
                    return;
                }
            }
            else                    // vertical
            {
                dest_x = getX();
                dest_y = getY() + 2;
                if(getY() < otherY && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                {
                    setDirection(up);
                    moveTo(dest_x, dest_y);
                    return;
                }
                dest_x = getX();
                dest_y = getY() - 2;
                if(getY() > otherY && !getWorld()->isAgentMovementBlockedAt(dest_x, dest_y, this))
                {
                    setDirection(down);
                    moveTo(dest_x, dest_y);
                    return;
                }
            }
        }
    }
    
    double zombieDistance = MAX_INT;

    // get nearest zombie info
    getWorld()->locateNearestCitizenThreat(getX(), getY(), otherX, otherY, zombieDistance);
    
    if(isThreat && zombieDistance <= 6400)
    {
        // move away from zombies
        double distanceUp = MAX_INT;
        double distanceDown = MAX_INT;
        double distanceLeft = MAX_INT;
        double distanceRight = MAX_INT;
        
        int distanceTracker = zombieDistance;
        int longest = -1;
        
        getWorld()->locateNearestCitizenThreat(getX(), getY()+2, otherX, otherY, distanceUp);
        if(distanceUp > distanceTracker && !getWorld()->isAgentMovementBlockedAt(getX(), getY()+2, this))
        {
            longest = up;
            distanceTracker = distanceUp;
        }
        
        getWorld()->locateNearestCitizenThreat(getX(), getY()-2, otherX, otherY, distanceDown);
        if(distanceDown > distanceTracker && !getWorld()->isAgentMovementBlockedAt(getX(), getY()-2, this))
        {
            longest = down;
            distanceTracker = distanceDown;
        }
        
        getWorld()->locateNearestCitizenThreat(getX()-2, getY(), otherX, otherY, distanceLeft);
        if(distanceLeft > distanceTracker && !getWorld()->isAgentMovementBlockedAt(getX()-2, getY(), this))
        {
            longest = left;
            distanceTracker = distanceLeft;
        }
        
        getWorld()->locateNearestCitizenThreat(getX()+2, getY(), otherX, otherY, distanceRight);
        if(distanceRight > distanceTracker && !getWorld()->isAgentMovementBlockedAt(getX()+2, getY(), this))
        {
            longest = right;
            distanceTracker = distanceRight;
        }
        
        if(longest == -1)
            return;
        
        // set direction and move farthest away from nearest zombie
        switch (longest) {
            case up:
                setDirection(up);
                dest_x = getX();
                dest_y = getY() + 2;
                moveTo(dest_x, dest_y);
                break;
            case down:
                setDirection(down);
                dest_x = getX();
                dest_y = getY() - 2;
                moveTo(dest_x, dest_y);
                break;
            case left:
                setDirection(left);
                dest_x = getX() - 2;
                dest_y = getY();
                moveTo(dest_x, dest_y);
                break;
            case right:
                setDirection(right);
                dest_x = getX() + 2;
                dest_y = getY();
                moveTo(dest_x, dest_y);
                break;
        }
    }
}

// citizen exit handling
void Citizen::useExitIfAppropriate()
{
    getWorld()->increaseScore(1000);
    getWorld()->recordCitizenGone();
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    setAlive(false);
}

void Citizen::dieByFallOrBurnIfAppropriate()
{
    setAlive(false);
    getWorld()->increaseScore(-1000);
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->recordCitizenGone();
}

// infect citizen
void Citizen::beVomitedOnIfAppropriate()
{
    if(getInfectionCount() == 0)
        getWorld()->playSound(SOUND_CITIZEN_INFECTED);
    Person::beVomitedOnIfAppropriate();
}

// Zombie implementation

Zombie::Zombie(StudentWorld* myWorld, double x, double y)
: Agent(myWorld, IID_ZOMBIE, x, y, right)
{}

bool Zombie::threatensCitizens() const { return true; }

void Zombie::vomitIfPossible()
{
    // if person in front of direction its facing
    double vomitX = getX();     // change according to direction
    double vomitY = getY();     // change according to direction
    
    switch (getDirection()) {
        case up:
            vomitY += SPRITE_HEIGHT;
            break;
        case down:
            vomitY -= SPRITE_HEIGHT;
            break;
        case left:
            vomitX -= SPRITE_WIDTH;
            break;
        case right:
            vomitX += SPRITE_WIDTH;
            break;
    }
    
    if(getWorld()->isZombieVomitTriggerAt(vomitX, vomitY))
    {
        if(randInt(1, 3) == 3)          // 1 in 3 chance of vomiting
        {
            getWorld()->addActor(new Vomit(getWorld(), vomitX, vomitY, getDirection()));
            getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
            return;
        }
    }
}

// move like zombies do
void Zombie::zombieMove(int& movementPlan)
{
    int destX = getX();
    int destY = getY();
    
    switch(getDirection()) {
        case up:
            destY += 1;
            break;
        case down:
            destY -= 1;
            break;
        case left:
            destX -= 1;
            break;
        case right:
            destX += 1;
            break;
    }
    
    if(!getWorld()->isAgentMovementBlockedAt(destX, destY, this))
    {
        moveTo(destX, destY);
        movementPlan--;
    }
    else
        movementPlan = 0;
}

// DumbZombie implementation

DumbZombie::DumbZombie(StudentWorld* myWorld, double x, double y)
: Zombie(myWorld, x, y), ticks(0), movementPlan(0)
{}

void DumbZombie::doSomething()
{
    if(!isAlive())
        return;
    
    // paralyzed every other tick
    if(ticks % 2 == 0)
    {
        ticks++;
        return;
    }
    
    ticks--;
    
    // vomits on nearby
    vomitIfPossible();
    
    if(movementPlan == 0)
    {
        // randomized movement
        movementPlan = randInt(3, 10);
        int direct = randInt(1, 4);
        
        if(direct == 1)
            setDirection(up);
        else if(direct == 2)
            setDirection(down);
        else if(direct == 3)
            setDirection(left);
        else if(direct == 4)
            setDirection(right);
    }
    
    zombieMove(movementPlan);
}

void DumbZombie::dieByFallOrBurnIfAppropriate()
{
    setAlive(false);
    getWorld()->increaseScore(1000);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    
    // 1 in 10 chance to fling vaccine at death
    if(randInt(1, 10) == 10)
    {
        // random flinging location and direction
        int random = randInt(1, 4);
        if(random == 1)
        {
            if(!getWorld()->isThrownGoodieBlockedAt(getX()+SPRITE_WIDTH, getY()))
                getWorld()->addActor(new VaccineGoodie(getWorld(), getX()+SPRITE_WIDTH, getY()));
        }
        else if(random == 2)
        {
            if(!getWorld()->isThrownGoodieBlockedAt(getX()-SPRITE_WIDTH, getY()))
                getWorld()->addActor(new VaccineGoodie(getWorld(), getX()-SPRITE_WIDTH, getY()));
        }
        else if(random == 3)
        {
            if(!getWorld()->isThrownGoodieBlockedAt(getX(), getY()+SPRITE_HEIGHT))
                getWorld()->addActor(new VaccineGoodie(getWorld(), getX(), getY()+SPRITE_HEIGHT));
        }
        else if(random == 4)
        {
            if(!getWorld()->isThrownGoodieBlockedAt(getX(), getY()-SPRITE_HEIGHT))
                getWorld()->addActor(new VaccineGoodie(getWorld(), getX(), getY()-SPRITE_HEIGHT));
        }
    }
}

// SmartZombie implementation

SmartZombie::SmartZombie(StudentWorld* myWorld, double x, double y)
: Zombie(myWorld, x, y), ticks(0), movementPlan(0)
{}

void SmartZombie::doSomething()
{
    if(!isAlive())
        return;
    
    // paralyzed every other tick
    if(ticks % 2 == 0)
    {
        ticks++;
        return;
    }
    
    ticks--;
    
    vomitIfPossible();
    
    double otherX, otherY, distance;
    
    if(movementPlan == 0)
    {
        movementPlan = randInt(3, 10);
        
        if(getWorld()->locateNearestVomitTrigger(getX(), getY(), otherX, otherY, distance))
        {
            // move toward trigger
            if(getY() == otherY)
            {
                if(getX() < otherX)
                    setDirection(right);
                if(getX() > otherX)
                    setDirection(left);
            }
            else if(getX() == otherX)
            {
                if(getY() < otherY)
                    setDirection(up);
                if(getY() > otherY)
                    setDirection(down);
            }
            else
            {
                if(randInt(0, 1) == 0)  // horizontal
                {
                    if(getX() < otherX)
                        setDirection(right);
                    if(getX() > otherX)
                        setDirection(left);
                }
                else                    // vertical
                {
                    if(getY() < otherY)
                        setDirection(up);
                    if(getY() > otherY)
                        setDirection(down);
                }
            }
        }
        else
        {
            // move randomly
            int direct = randInt(1, 4);
            
            if(direct == 1)
                setDirection(up);
            else if(direct == 2)
                setDirection(down);
            else if(direct == 3)
                setDirection(left);
            else if(direct == 4)
                setDirection(right);
        }
    }
    
    zombieMove(movementPlan);
}

void SmartZombie::dieByFallOrBurnIfAppropriate()
{
    setAlive(false);
    getWorld()->increaseScore(2000);
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
}
