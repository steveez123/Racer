#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    //starts at 0 saved souls, 5000 bonus points
    m_numSavedSouls = 0;
    m_bonusPoints = 5000;
    
    m_ghostRacer = new GhostRacer(this);
    
    int N = VIEW_HEIGHT/SPRITE_HEIGHT;
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    
    
    for(int j = 0; j < N; j ++){
        allActors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, j * SPRITE_HEIGHT));
        allActors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j * SPRITE_HEIGHT));
    }
    
    int M = VIEW_HEIGHT/(4*SPRITE_HEIGHT);
    
    //y value of most recent white border line added
    lastBorderLineY = (M-1) * 4 * SPRITE_HEIGHT;
    
    for(int j = 0 ; j < M; j ++){
        allActors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j * (4*SPRITE_HEIGHT)));
        allActors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j * (4*SPRITE_HEIGHT)));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //ghost racer moves
    m_ghostRacer->doSomething();
    
    list<Actor*>::iterator actorIterator = allActors.begin();
    while(actorIterator != allActors.end()){
        if((*actorIterator)->isAlive()){
            (*actorIterator)->doSomething();
        }
        if(!m_ghostRacer->isAlive()){
            //player died
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        else if(m_numSavedSouls>=getLevel() * 2 + 5){
            //level has been passed
            playSound(SOUND_FINISHED_LEVEL);
            increaseScore(max(0, m_bonusPoints));
            return GWSTATUS_FINISHED_LEVEL;
        }
        actorIterator++;
    }
    
    //deletes all dead objects from game
    actorIterator = allActors.begin();
    while(actorIterator != allActors.end()){
        if((*actorIterator)->isAlive() == false){
            list<Actor*>::iterator dead = actorIterator;
            Actor* deadObject = *dead;
            actorIterator++;
            allActors.erase(dead);
            delete deadObject;
        }
        else{
            actorIterator++;
        }
    }
    
    //updates y value of most recent white border line added
    lastBorderLineY = lastBorderLineY - 4 - m_ghostRacer->getVerticalSpeed();
    
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - lastBorderLineY;
    
    if(delta_y >= SPRITE_HEIGHT){
        allActors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH/2, new_border_y));
        allActors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH/2, new_border_y));
    }
    if(delta_y >= 4 * SPRITE_HEIGHT){
        //new white border line added
        lastBorderLineY = new_border_y;
        
        allActors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3, new_border_y));
        allActors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3, new_border_y));
    }
    
    int ChanceVehicle = max(100 - getLevel() * 10 , 20);
    
    if(randInt(0, ChanceVehicle - 1) == 0){
        int start_lane = randInt(1,3);
        
        int laneToPlace = start_lane;
        int vehicleSpeed = 0;
        double startY = 0.0;
        
        for(int i = 0; i < 3; i ++){
            //lanes chosen goes in order, either 1,2,3, or 2, 3, 1 or 3, 1, 2
            int cur_lane = (start_lane + i - 1 ) % 3 + 1;
            
            laneToPlace = cur_lane;
            
            Actor* closestActor = getClosestCollisionToCab(true, cur_lane, 0);
            
            //checks if lane is available, and if so stops looking for lane
            if(closestActor == nullptr || closestActor->getY() > VIEW_HEIGHT/3){
                startY = SPRITE_HEIGHT/2;
                vehicleSpeed = m_ghostRacer->getVerticalSpeed() + randInt(2,4);
                break;
            }
            
            closestActor = getClosestCollisionToCab(false, cur_lane, VIEW_HEIGHT);
            if(closestActor == nullptr || closestActor->getY() < 2 * VIEW_HEIGHT/3){
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                vehicleSpeed = m_ghostRacer->getVerticalSpeed() - randInt(2,4);
                break;
            }
            
            if(i == 2){
                laneToPlace = -1;
            }
        }
        
        if(laneToPlace == 1){
            allActors.push_back(new ZombieCab(this, ROAD_CENTER - ROAD_WIDTH/3, startY, vehicleSpeed));
        }
        else if(laneToPlace == 2){
            allActors.push_back(new ZombieCab(this, ROAD_CENTER, startY, vehicleSpeed));
        }
        else if(laneToPlace == 3){
            allActors.push_back(new ZombieCab(this, ROAD_CENTER + ROAD_WIDTH/3, startY, vehicleSpeed));
        }
    }
    
    int ChanceOilSlick = max(150 - getLevel()*10, 40);
    
    if(randInt(0,ChanceOilSlick-1)==0){
        allActors.push_back(new OilSlick(this, randInt(ROAD_CENTER - ROAD_WIDTH/2, ROAD_CENTER + ROAD_WIDTH/2), VIEW_HEIGHT));
    }
    
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    
    if(randInt(0, ChanceHumanPed - 1) == 0){
        allActors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH - 1), VIEW_HEIGHT));
    }
    
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    
    if(randInt(0, ChanceZombiePed - 1) == 0){
        allActors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH - 1), VIEW_HEIGHT));
    }
    
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    if(randInt(0, ChanceOfHolyWater-1) == 0){
        allActors.push_back(new HolyWaterGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH/2, ROAD_CENTER + ROAD_WIDTH/2), VIEW_HEIGHT));
    }
    
    int ChanceOfLostSoul = 100;
    if(randInt(0, ChanceOfLostSoul - 1) == 0){
        allActors.push_back(new SoulGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH/2, ROAD_CENTER + ROAD_WIDTH/2), VIEW_HEIGHT));
    }
    
    //displays text at top of screen
    
    ostringstream oss;
    oss << "Score: " << getScore() << setw(7) << "Lvl: " << getLevel() << setw(14) << "Souls2Save: " << 2 * getLevel() + 5 - m_numSavedSouls << setw(9) << "Lives: " << getLives() << setw(10) << "Health: " << m_ghostRacer->getHP() << setw(10) << "Sprays: " << m_ghostRacer->numSprays() << setw(9) << "Bonus: " << m_bonusPoints << endl;
    setGameStatText(oss.str());
    
    m_bonusPoints -= 1;
    //bonus points cant be negative
    if(m_bonusPoints<0){
        m_bonusPoints = 0;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //deletes all actors in allActors list, as well as the ghost racer
    list<Actor*>::iterator actorIterator = allActors.begin();
    while(actorIterator != allActors.end()){
        list<Actor*>::iterator dead = actorIterator;
        Actor* deadObject = *dead;
        actorIterator++;
        allActors.erase(dead);
        delete deadObject;
    }
    delete m_ghostRacer;
}

GhostRacer* StudentWorld::getGhostRacer(){
    return m_ghostRacer;
}

void StudentWorld::addActor(Actor* a){
    allActors.push_back(a);
}

bool StudentWorld::sprayFirstAppropriateActor(Actor *a){
    //finds first sprayable actor by iterating through the allActors list, ghost racer is not sprayable so it is ignored
    list<Actor*>::iterator actorIterator = allActors.begin();
    while(actorIterator != allActors.end()){
        if((*actorIterator)->isAlive()&&overlaps(a, *actorIterator)&&(*actorIterator)->beSprayedIfAppropriate()){
            return true;
        }
        actorIterator++;
    }
    return false;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const{
    double delta_x = abs(a1->getX()-a2->getX());
    double delta_y = abs(a1->getY()-a2->getY());
    
    double radius_sum = a1->getRadius() + a2->getRadius();
    
    if(delta_x<radius_sum*0.25 && delta_y < radius_sum * 0.6){
        return true;
    }
    return false;
}

Actor* StudentWorld::getClosestCollisionToCab(bool inFront, int cabLane, double y) const{
    //lane must be between 1 and 3 inclusive
    if(cabLane <= 0 || cabLane >= 4){
        return nullptr;
    }
    
    //default closest nullptr, view_height, which is what will return if none are found
    Actor* closestActor = nullptr;
    double closestDistance = VIEW_HEIGHT;
    
    list<Actor*>::const_iterator actorIterator = allActors.begin();
    if(inFront){
        //if looking for collision in front of current y value
        while(actorIterator != allActors.end()){
            Actor* currActor = (*actorIterator);
            if(currActor->isAlive() && currActor->collisionAvoidanceWorthy() && cabLane == getLane(currActor->getX()) && currActor->getY()>y){
                if(currActor->getY()-y<=closestDistance){
                    //updated closest distance, closest actor
                    closestDistance = currActor->getY() - y;
                    closestActor = currActor;
                }
            }
            actorIterator++;
        }
        if(m_ghostRacer->isAlive() && cabLane == getLane(m_ghostRacer->getX()) && m_ghostRacer->getY()>y){
            if(m_ghostRacer->getY()-y<=closestDistance){
                closestDistance = m_ghostRacer->getY() - y;
                closestActor = m_ghostRacer;
            }
        }
    }
    else{
        //if looking for collision behind current y value
        while(actorIterator != allActors.end()){
            Actor* currActor = (*actorIterator);
            if(currActor->isAlive() && currActor->collisionAvoidanceWorthy() && cabLane == getLane(currActor->getX()) && currActor->getY()<y){
                if(y - currActor->getY()<=closestDistance){
                    //updated closest distance, closest actor
                    closestDistance = y - currActor->getY();
                    closestActor = currActor;
                }
            }
            actorIterator++;
        }
        if(m_ghostRacer->isAlive() && cabLane == getLane(m_ghostRacer->getX()) && m_ghostRacer->getY()<y){
            if(y - m_ghostRacer->getY()<=closestDistance){
                closestDistance = y - m_ghostRacer->getY();
                closestActor = m_ghostRacer;
            }
        }
    }
    return closestActor;
}

int StudentWorld::getLane(double x) const{
    //either lane 1 (left), lane 2(middle), or lane 3(right)
    if(x>=ROAD_CENTER-ROAD_WIDTH/2 && x<ROAD_CENTER - ROAD_WIDTH/6){
        return 1;
    }
    else if(x>=ROAD_CENTER-ROAD_WIDTH/6 && x<ROAD_CENTER + ROAD_WIDTH/6){
        return 2;
    }
    else if(x>=ROAD_CENTER+ROAD_WIDTH/6 && x<ROAD_CENTER + ROAD_WIDTH/2){
        return 3;
    }
    return -1;
}

void StudentWorld::soulSaved(){
    m_numSavedSouls +=1;
}
