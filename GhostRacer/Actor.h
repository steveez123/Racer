#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double startX, double startY, bool collisionAvoidanceWorthy, int dir = 0, double size = 1.0, unsigned int depth = 0, int verticalSpeed = -4);
    
    //does specified action during a tick
    virtual void doSomething() = 0;
    
    //returns whether actor is alive or not
    bool isAlive() const;
    
    //if the actor is sprayable, then spray it and return true, or else return false
    virtual bool beSprayedIfAppropriate();
    
    //returns vertical speed of actor
    int getVerticalSpeed() const;
    
    //changes vertical speed of actor to newSpeed
    void setVerticalSpeed(int newSpeed);
    
    //sets the actors state to dead
    void setDead();
    
    //returns whether the actors is collision avoidance worthy or not
    bool collisionAvoidanceWorthy() const;
    
    //checks if the actor is outside the screen or not
    bool checkOutsideOfScreen();
    
    //moves actors based on their vertical and horizontal speed, relative to the ghost racer
    void moveRelativeToGhostRacer();
    
    //returns horizontal speed of actor
    int getHorizSpeed() const;
    
    //changes horizontal speed of actor to s
    void setHorizSpeed(int s);
    
    //converts degree to the value in radians
    double convertToRadians(int degree) const;
    
    //returns pointer to student world
    StudentWorld* getWorld() const;
    
private:
    int m_verticalSpeed;
    int m_horizontalSpeed;
    bool m_aliveState;
    bool m_collisionAvoidanceWorthy;
    StudentWorld* m_world;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp, int verticalSpeed = -4);

      // Get hit points.
    int getHP() const;

      // Increase hit points by hp.
    void increaseHP(int hp);

      // Do what the spec says happens when hp units of damage is inflicted.
      // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);
    
      // return sound when agent is damaged but not dead
    virtual int soundWhenHurt() const = 0;

      // retunr sound when agent dies
    virtual int soundWhenDie() const = 0;
    
    //all agents are sprayable
    virtual bool beSprayedIfAppropriate();
     
private:
    int m_HP;
};

class GhostRacer: public Agent
{
public:
    GhostRacer(StudentWorld* sw);
    
    //moves ghost racer during a tick according to spec
    void moveAlgorithm();
    virtual void doSomething();
    
    virtual int soundWhenHurt() const;
    
    virtual int soundWhenDie() const;
    
    virtual bool beSprayedIfAppropriate();
    
    //increases number of water sprays by amt
    void increaseSprays(int amt);
    
    //returns number of sprays ghost racer has
    int numSprays() const;
    
    //spins the ghost racer
    void spin();
private:
    int m_HolyWaterCount;
};


class Pedestrian: public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double startX, double startY, double size);
    virtual int soundWhenHurt() const;
    virtual void doSomething();
    
private:
    int m_movementPlanDistance;
};

class HumanPedestrian: public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double startX, double startY);
    virtual int soundWhenDie() const;
    virtual void doSomething();
    virtual bool takeDamageAndPossiblyDie(int hp);
    
private:
    
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double startX, double startY);
    virtual int soundWhenDie() const;
    virtual void doSomething();
    virtual bool takeDamageAndPossiblyDie(int hp);
private:
    int m_ticks;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double startX, double startY, int verticalSpeed);
    virtual void doSomething();
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
    
    virtual bool takeDamageAndPossiblyDie(int hp);
private:
    bool m_damagedGhostRacer;
    int m_movementPlanDistance;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, int imageID, double startX, double startY);
    virtual void doSomething();
private:
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
private:
    int m_maxTravelDistance;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    
    virtual void doSomething();
    //what this specific object does when it overlaps with the ghost racer
    virtual void doSpecialActivity() = 0;
    //sound the actor does when overlapped with ghost racer
    virtual int getSound() const = 0;
    //score it gives
    virtual int getScore() const = 0;

private:
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doSpecialActivity();
    virtual int getSound() const;
    virtual int getScore() const;
private:
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    //healing goodies can be sprayed
    virtual bool beSprayedIfAppropriate();
    virtual void doSpecialActivity();
    virtual int getSound() const;
    virtual int getScore() const;
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    //holy water goodies can be sprayed
    virtual bool beSprayedIfAppropriate();
    virtual void doSpecialActivity();
    virtual int getSound() const;
    virtual int getScore() const;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doSpecialActivity();
    virtual int getSound() const;
    virtual int getScore() const;
};

#endif // ACTOR_H_
