#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* sw, int imageID, double startX, double startY, bool collisionAvoidanceWorthy, int dir, double size, unsigned int depth, int verticalSpeed):GraphObject(imageID, startX, startY, dir, size, depth), m_verticalSpeed(verticalSpeed), m_horizontalSpeed(0), m_aliveState(true), m_collisionAvoidanceWorthy(collisionAvoidanceWorthy), m_world(sw){
    
}

int Actor::getVerticalSpeed() const{
    return m_verticalSpeed;
}

void Actor::setVerticalSpeed(int newSpeed){
    m_verticalSpeed = newSpeed;
}

bool Actor::isAlive() const{
    return m_aliveState;
}

void Actor::setDead(){
    m_aliveState = false;
}

bool Actor::beSprayedIfAppropriate(){
    return false;
}

bool Actor::collisionAvoidanceWorthy() const{
    return m_collisionAvoidanceWorthy;
}

bool Actor::checkOutsideOfScreen(){
    //checks if it is out of view area, and sets it to deaf if it is
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        setDead();
        return true;
    }
    return false;
}

void Actor::moveRelativeToGhostRacer(){
    int vert_speed = getVerticalSpeed() - getWorld()->getGhostRacer()->getVerticalSpeed();
    int horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    moveTo(new_x, new_y);
    
    //checks if this move makes the actor out of screen view
    checkOutsideOfScreen();
}


int Actor::getHorizSpeed() const{
    return m_horizontalSpeed;
}

void Actor::setHorizSpeed(int s){
    m_horizontalSpeed = s;
}

double Actor::convertToRadians(int degree) const{
    return 3.1415926535 * degree / 180.0;
}

StudentWorld* Actor::getWorld() const{
    return m_world;
}

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp, int verticalSpeed):Actor(sw, imageID, x, y, true, dir, size, 0, verticalSpeed), m_HP(hp){
    
}

int Agent::getHP() const{
    return m_HP;
}

void Agent::increaseHP(int hp){
    m_HP = min(100, m_HP + hp);
}

bool Agent::takeDamageAndPossiblyDie(int hp){
    m_HP -= hp;
    if(m_HP<=0){
        setDead();
        getWorld()->playSound(soundWhenDie());
        return true;
    }
    return false;
}

bool Agent::beSprayedIfAppropriate(){
    takeDamageAndPossiblyDie(1);
    return true;
}

GhostRacer::GhostRacer(StudentWorld* sw):Agent(sw, IID_GHOST_RACER, 128.0, 32.0, 4.0, 90, 100, 0) , m_HolyWaterCount(10){
    
}


void GhostRacer::moveAlgorithm(){
    double max_shift_per_tick = 4.0;
    int direction = getDirection();
    double delta_x = cos(convertToRadians(direction))*max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::doSomething(){
    if(!isAlive()){
        return;
    }
    
    if(getX()<=ROAD_CENTER-ROAD_WIDTH/2){
        //hit the left side of the road
        if(getDirection()>90){
            if(takeDamageAndPossiblyDie(10)){
                return;
            }
        }
        setDirection(82);
        getWorld()->playSound(soundWhenHurt());
        
    }
    else if(getX()>=ROAD_CENTER+ROAD_WIDTH/2){
        //hit the right side of the road
        if(getDirection()<90){
            if(takeDamageAndPossiblyDie(10)){
                return;
            }
        }
        setDirection(98);
        getWorld()->playSound(soundWhenHurt());
    }
    else{
        //check for key pressed
        int ch;
        if(getWorld()->getKey(ch)){
            switch(ch)
            {
                case KEY_PRESS_SPACE:
                    if(m_HolyWaterCount>=1){
                        getWorld()->addActor(new Spray(getWorld(), getX() + SPRITE_HEIGHT * cos(convertToRadians(getDirection())), getY() + SPRITE_HEIGHT * sin(convertToRadians(getDirection())), getDirection()));
                        getWorld()->playSound(SOUND_PLAYER_SPRAY);
                        m_HolyWaterCount-=1;
                    }
                    break;
                case KEY_PRESS_LEFT:
                    if(getDirection()<114){
                        setDirection(getDirection()+8);
                    }
                    break;
                case KEY_PRESS_RIGHT:
                    if(getDirection()>66){
                        setDirection(getDirection()-8);
                    }
                    break;
                case KEY_PRESS_UP:
                    if(getVerticalSpeed()<5){
                        setVerticalSpeed(getVerticalSpeed()+1);
                    }
                    break;
                case KEY_PRESS_DOWN:
                    if(getVerticalSpeed()>-1){
                        setVerticalSpeed(getVerticalSpeed()-1);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    //move according to spec
    moveAlgorithm();
    
}


int GhostRacer::soundWhenHurt() const{
    return SOUND_VEHICLE_CRASH;
}

int GhostRacer::soundWhenDie() const{
    return SOUND_PLAYER_DIE;
}

bool GhostRacer::beSprayedIfAppropriate(){
    return false;
}

void GhostRacer::spin(){
    if(randInt(1,2)==1){
        //spin counterclockwise
        setDirection(min(getDirection()+randInt(5,20), 120));
    }
    else{
        //spin clockwise
        setDirection(max(getDirection()-randInt(5,20), 60));
    }
}

void GhostRacer::increaseSprays(int amt){
    m_HolyWaterCount += amt;
}

int GhostRacer::numSprays() const{
    return m_HolyWaterCount;
}

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double startX, double startY, double size):Agent(sw, imageID, startX, startY, size, 0, 2), m_movementPlanDistance(0){
    
}

int Pedestrian::soundWhenHurt() const{
    return SOUND_PED_HURT;
}

void Pedestrian::doSomething(){
    moveRelativeToGhostRacer();
    
    m_movementPlanDistance --;
    if(m_movementPlanDistance>0){
        return;
    }
    
    setHorizSpeed(randInt(1,3)*(2*randInt(1,2)-3));
    m_movementPlanDistance = randInt(4, 32);
    
    if(getHorizSpeed()<0){
        setDirection(180);
    }
    else{
        setDirection(0);
    }
}

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double startX, double startY):Pedestrian(sw, IID_HUMAN_PED, startX, startY, 2.0){
    
}

int HumanPedestrian::soundWhenDie() const{
    return SOUND_NONE;
}

void HumanPedestrian::doSomething(){
    if(!isAlive()){
        return;
    }
    if(getWorld()->overlaps(this, getWorld()->getGhostRacer())){
        //ghost racer will die from this much damage
        getWorld()->getGhostRacer()->takeDamageAndPossiblyDie(100);
        return;
    }
    Pedestrian::doSomething();
    
}

bool HumanPedestrian::takeDamageAndPossiblyDie(int hp){
    //doesnt lose hp, but changes direction becuase it has been hit by water spray
    setHorizSpeed(getHorizSpeed()*-1);
    setDirection(180-getDirection());
    getWorld()->playSound(soundWhenHurt());
    return false;
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double startX, double startY):Pedestrian(sw, IID_ZOMBIE_PED ,startX, startY, 3.0), m_ticks(0){
    
}

int ZombiePedestrian::soundWhenDie() const{
    return SOUND_PED_DIE;
}

void ZombiePedestrian::doSomething(){
    GhostRacer* ghostRacer = getWorld()->getGhostRacer();
    
    if(!isAlive()){
        return;
    }
    if(getWorld()->overlaps(this, ghostRacer)){
        ghostRacer->takeDamageAndPossiblyDie(5);
        takeDamageAndPossiblyDie(2);
        return;
    }
    if(abs(getX()-ghostRacer->getX())<=30 && getY() > ghostRacer->getY()){
        setDirection(270);
        if(getX()<ghostRacer->getX()){
            setHorizSpeed(1);
        }
        else if(getX()>ghostRacer->getX()){
            setHorizSpeed(-1);
        }
        else{
            setHorizSpeed(0);
        }
        m_ticks --;
        if(m_ticks<=0){
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticks = 20;
        }
    }
    Pedestrian::doSomething();
}

bool ZombiePedestrian::takeDamageAndPossiblyDie(int hp){
   
    if(Agent::takeDamageAndPossiblyDie(hp)){
        if(!getWorld()->overlaps(this, getWorld()->getGhostRacer())&&randInt(1,5)==1){
            getWorld()->addActor(new HealingGoodie(getWorld(), getX(), getY()));
        }
        getWorld()->increaseScore(150);
        return true;
    }
    getWorld()->playSound(soundWhenHurt());
    return false;
}

ZombieCab::ZombieCab(StudentWorld* sw, double startX, double startY, int verticalSpeed):Agent(sw, IID_ZOMBIE_CAB, startX, startY, 4.0, 90, 3, verticalSpeed),  m_damagedGhostRacer(false), m_movementPlanDistance(0){
}

void ZombieCab::doSomething(){
    if(!isAlive()){
        return;
    }
    
    GhostRacer* ghostRacer = getWorld()->getGhostRacer();
    
    //swerves away if first time hitting the ghost racer
    if(getWorld()->overlaps(this, ghostRacer)&&!m_damagedGhostRacer){
        getWorld()->playSound(SOUND_VEHICLE_CRASH);
        ghostRacer->takeDamageAndPossiblyDie(20);
        if(getX()<=ghostRacer->getX()){
            setHorizSpeed(-5);
            setDirection(120 + randInt(0, 19));
        }
        else{
            setHorizSpeed(5);
            setDirection(60 - randInt(0, 19));
        }
        m_damagedGhostRacer = true;
    }
    
    moveRelativeToGhostRacer();
    
    if(getVerticalSpeed()>ghostRacer->getVerticalSpeed()){
        Actor* closestInFront = getWorld()->getClosestCollisionToCab(true, getWorld()->getLane(getX()), getY());
        if(closestInFront!=nullptr && closestInFront->getY()-getY()<96){
            setVerticalSpeed(getVerticalSpeed()-0.5);
            return;
        }
    }
    else{
        Actor* closestBehind = getWorld()->getClosestCollisionToCab(false, getWorld()->getLane(getX()), getY());
        if(closestBehind!=nullptr && closestBehind!= ghostRacer && getY()-closestBehind->getY()<96){
            setVerticalSpeed(getVerticalSpeed()+0.5);
            return;
        }
    }
    
    m_movementPlanDistance--;
    if(m_movementPlanDistance>0){
        return;
    }
    
    m_movementPlanDistance = randInt(4, 32);
    setVerticalSpeed(getVerticalSpeed() + randInt(-2,2));
    
}

int ZombieCab::soundWhenHurt() const{
    return SOUND_VEHICLE_HURT;
}

int ZombieCab::soundWhenDie() const{
    return SOUND_VEHICLE_DIE;
}

bool ZombieCab::takeDamageAndPossiblyDie(int hp){
    if(Agent::takeDamageAndPossiblyDie(hp)){
        if(randInt(1,5)==1){
            getWorld()->addActor(new OilSlick(getWorld(), getX(), getY()));
        }
        getWorld()->increaseScore(200);
        return true;
    }
    getWorld()->playSound(soundWhenHurt());
    return false;
}

BorderLine::BorderLine(StudentWorld* sw, int imageID, double startX, double startY):Actor(sw, imageID, startX, startY, false, 0, 2.0, 2){
    
}

void BorderLine::doSomething(){
    moveRelativeToGhostRacer();

}

Spray::Spray(StudentWorld* sw, double x, double y, int dir):Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, false, dir, 1.0, 1, 0), m_maxTravelDistance(160){
    
}

void Spray::doSomething(){
    if(!isAlive()){
        return;
    }
    
    if(getWorld()->sprayFirstAppropriateActor(this)){
        setDead();
        return;
    }
    
    moveForward(SPRITE_HEIGHT);
    m_maxTravelDistance -= SPRITE_HEIGHT;
    
    if(checkOutsideOfScreen()){
        return;
    }
    
    if(m_maxTravelDistance<=0){
        setDead();
    }
    
}

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir):Actor(sw, imageID, x, y, false, dir, size, 2){
    
}

void GhostRacerActivatedObject::doSomething(){
    moveRelativeToGhostRacer();
    if(!isAlive()){
        return;
    }
    
    if(getWorld()->overlaps(this, getWorld()->getGhostRacer())){
        //what it does if it hits the ghost racer
        doSpecialActivity();
        getWorld()->playSound(getSound());
        getWorld()->increaseScore(getScore());
    }
}

OilSlick::OilSlick(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2,5), 0){
    
}

void OilSlick::doSpecialActivity(){
    getWorld()->getGhostRacer()->spin();
}

int OilSlick::getSound() const{
    return SOUND_OIL_SLICK;
}

int OilSlick::getScore() const{
    return 0;
}

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1.0, 0){
    
}

bool HealingGoodie::beSprayedIfAppropriate(){
    setDead();
    return true;
}

void HealingGoodie::doSpecialActivity(){
    
    getWorld()->getGhostRacer()->increaseHP(10);
    setDead();
}

int HealingGoodie::getSound() const{
    return SOUND_GOT_GOODIE;
}

int HealingGoodie::getScore() const{
    return 250;
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2.0, 90){
    
}

bool HolyWaterGoodie::beSprayedIfAppropriate(){
    setDead();
    return true;
}

void HolyWaterGoodie::doSpecialActivity(){
    getWorld()->getGhostRacer()->increaseSprays(10);
    setDead();
}

int HolyWaterGoodie::getSound() const{
    return SOUND_GOT_GOODIE;
}

int HolyWaterGoodie::getScore() const{
    return 50;
}

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y):GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4.0, 0){
    
}

void SoulGoodie::doSomething(){
    GhostRacerActivatedObject::doSomething();
    if(!isAlive()){
        return;
    }
    //changes direction if it has not been picked up
    setDirection(getDirection()-10);
}

void SoulGoodie::doSpecialActivity(){
    getWorld()->soulSaved();
    setDead();
}

int SoulGoodie::getSound() const{
    return SOUND_GOT_SOUL;
}

int SoulGoodie::getScore() const{
    return 100;
}
