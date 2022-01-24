#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    
    //what to do at start of game
    virtual int init();
    
    //what it does each tick
    virtual int move();
    
    //what to do after life is lost
    virtual void cleanUp();
    
    //returns pointer to ghost racer
    GhostRacer* getGhostRacer();
    
    //adds actor a to list
    void addActor(Actor* a);
    
    //sprays first sprayable actor that overlaps a
    bool sprayFirstAppropriateActor(Actor* a);
    
    //returns whether a1 and a2 overlap
    bool overlaps(const Actor* a1, const Actor* a2) const;
    
    //finds closest actor in front of or behind the y value in the lane cabLane
    Actor* getClosestCollisionToCab(bool inFront, int cabLane, double y) const;
    
    //returns the lane this x value is in
    int getLane(double x) const;
    
    //saves a soul, increases souls saved by one
    void soulSaved();
private:
    list<Actor*> allActors;
    GhostRacer* m_ghostRacer;
    double lastBorderLineY;
    int m_numSavedSouls;
    int m_bonusPoints;
};

#endif // STUDENTWORLD_H_
