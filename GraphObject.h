#ifndef GRAPHOBJ_H_
#define GRAPHOBJ_H_

#include "SpriteManager.h"
#include "GameConstants.h"

#include <set>
#include <cmath>

const int ANIMATION_POSITIONS_PER_TICK = 1;

using Direction = int;

class GraphObject
{
  public:

    static const int right = 0;
    static const int left = 180;
    static const int up = 90;
    static const int down = 270;

    GraphObject(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
     : m_imageID(imageID), m_x(startX), m_y(startY), m_destX(startX), m_destY(startY),
       m_animationNumber(0), m_direction(dir), m_depth(depth), m_size(size)
    {
        if (m_size <= 0)
            m_size = 1;

        getGraphObjects(m_depth).insert(this);
    }

    virtual ~GraphObject()
    {
        getGraphObjects(m_depth).erase(this);
    }

    double getX() const
    {
          // If already moved but not yet animated, use new location anyway.
        return m_destX;
    }

    double getY() const
    {
          // If already moved but not yet animated, use new location anyway.
        return m_destY;
    }

    virtual void moveTo(double x, double y)
    {
        m_destX = x;
        m_destY = y;
        increaseAnimationNumber();
    }

    Direction getDirection() const
    {
        return m_direction;
    }

    void setDirection(Direction d)
    {
        while (d < 0)
            d += 360;

        m_direction = d % 360;
    }

    void setSize(double size)
    {
        m_size = size;
    }

    double getSize() const
    {
        return m_size;
    }

      // The following should be used by only the framework, not the student

    void increaseAnimationNumber()
    {
        m_animationNumber++;
    }

    template<typename Func>
    static void drawAllObjects(Func plotFunc)
    {
        for (int depth = NUM_DEPTHS - 1; depth >= 0; depth--)
        {
            for (GraphObject* go : getGraphObjects(depth))
            {
                go->animate();
                plotFunc(go->m_imageID, go->m_animationNumber, go->m_x, go->m_y, go->m_direction, go->m_size);
            }
        }
    }

      // Prevent copying or assigning GraphObjects
    GraphObject(const GraphObject&) = delete;
    GraphObject& operator=(const GraphObject&) = delete;

  private:

    static const int NUM_DEPTHS = 4;
    int     m_imageID;
    double  m_x;
    double  m_y;
    double  m_destX;
    double  m_destY;
    int     m_animationNumber;
    Direction   m_direction;
    int     m_depth;
    double  m_size;

    void animate()
    {
        m_x = m_destX;
        m_y = m_destY;
        //moveALittle(m_x, m_destX);
        //moveALittle(m_y, m_destY);
    }

    void moveALittle(double& from, double& to)
    {
        static const double DISTANCE = 1.0/ANIMATION_POSITIONS_PER_TICK;
        if (to - from >= DISTANCE)
            from += DISTANCE;
        else if (from - to >= DISTANCE)
            from -= DISTANCE;
        else
            from = to;
    }

    static std::set<GraphObject*>& getGraphObjects(int depth)
    {
        static std::set<GraphObject*> graphObjects[NUM_DEPTHS];
        if (depth < NUM_DEPTHS)
            return graphObjects[depth];
        else
            return graphObjects[0];     // empty;
    }
};

#endif // GRAPHOBJ_H_
