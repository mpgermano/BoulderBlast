#ifndef ACTOR_H
#define ACTOR_H

#include "GraphObject.h"
#include "GameConstants.h"
#include <iostream>
#include <string>
using namespace std;

const int BULLET_DESTROYED = -1;
const int BULLET_NOTHING = 0;
const int BULLET_STRIKES = 1;

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* swd,int ID, int x, int y, Direction start = none, bool barrier = true, int bulletEffect = BULLET_NOTHING, int hitPoints = 0);
    virtual ~Actor(){}
    
    virtual void doSomething() = 0;
    virtual void doDamage();
    
    void setHealth(int toNum);
    void setDead();

    StudentWorld* getWorld() const;
    virtual bool isStealable() const;
    virtual bool countsInFactoryCount() const;
    virtual int getBulletEffect() const;
    bool isBarrier() const;
    bool isAlive() const;
    int getHealth() const;
    
private:
    StudentWorld* m_world;
    bool m_isBar;
    bool m_isAlive;
    int m_bulletEffect;
    int m_hitPoints;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* swd, int ID, int x, int y, int health, int sound, Direction start = none);
    virtual ~Agent(){}
    
    bool moveRegular(Direction d);
    void shoot();
    
private:
    int m_health;
    int m_sound;
    bool deadBot;
};


class Player : public Agent
{
public:
    Player(StudentWorld* swd, int x, int y);
    
    virtual void doSomething();
    virtual void doDamage();
    void addAmmo();
    
    int healthPercent() const;
    int ammoCount() const;

private:
    int m_ammo;
};


class Robot : public Agent
{
public:
    Robot(StudentWorld* swd, int ID, int x, int y, int health, Direction d, int bonus);
    ~Robot(){}
    virtual void doSomething();
    virtual void doDamage();

    bool canAim();
    void setTick();
    void decTick();
    
    virtual bool doesShoot() const;
    bool shouldContinue() const;
    
private:
    int m_tick;
    int m_bonus;
    bool m_continue;
};


class SnarlBot : public Robot
{
public:
    SnarlBot(StudentWorld* swd, int x, int y, Direction d);
    virtual void doSomething();
};


class KleptoBot : public Robot
{
public:
    KleptoBot(StudentWorld* swd, int ID, int x, int y, int health, int bonus);
    ~KleptoBot(){}
    virtual void doSomething();
    virtual void doDamage();
    virtual bool countsInFactoryCount() const;
    
private:
    int m_distanceBeforeTurning;
    int m_currentDirectionSteps;
    bool hasGoodie;
    char goodieKind;
};


class RegularKleptoBot : public KleptoBot
{
public:
    RegularKleptoBot(StudentWorld* swd, int x, int y);
    virtual bool doesShoot() const;
};


class AngryKleptoBot : public KleptoBot
{
public:
    AngryKleptoBot(StudentWorld* swd, int x, int y);
};


class Barrier: public Actor
{
public:
    Barrier(StudentWorld* swd, int ID, int x, int y, int hitPoints = 0);
    virtual ~Barrier(){}
    virtual void doSomething();
};


class Wall : public Barrier
{
public:
    Wall(StudentWorld* swd, int x, int y);
};


class Boulder : public Barrier
{
public:
    Boulder(StudentWorld* swd, int x, int y);
    bool push(int moveX, int moveY);
    virtual int getBulletEffect() const;
};

class Hole : public Barrier
{
public:
    Hole(StudentWorld* swd, int x, int y);
    virtual void doSomething();
    virtual int getBulletEffect() const;
};

class Factory : public Barrier
{
public:
    Factory(StudentWorld* swd, int x, int y, bool angry);
    virtual void doSomething();
private:
    bool m_angry;
};



class Accessible : public Actor
{
public:
    Accessible(StudentWorld* swd, int ID, int x, int y, Direction start = none);
    virtual ~Accessible(){}
};


class Pickup : public Accessible
{
public:
    Pickup(StudentWorld* swd, int ID, int x, int y, int bonus,
           int sound = SOUND_GOT_GOODIE);
    virtual ~Pickup(){}
    
    virtual void doSomething();
    virtual void inform() = 0;
    virtual bool isOpen() const;
    virtual bool isStealable() const;
    int getBonus() const;
private:
    int m_bonus;
    int m_sound;
};


class Jewel : public Pickup
{
public:
    Jewel(StudentWorld* swd, int x, int y);
    virtual void inform();
    virtual bool isStealable() const;
};


class Exit : public Pickup
{
public:
    Exit(StudentWorld* swd, int x, int y);
    virtual void inform();
    void setOpen();
    virtual bool isOpen() const;
    virtual bool isStealable() const;

private:
    bool m_open;
};


class ExtraLife : public Pickup
{
public:
    ExtraLife(StudentWorld* swd, int x, int y);
    virtual void inform();
};


class RestoreHealth : public Pickup
{
public:
    RestoreHealth(StudentWorld* swd, int x, int y);
    virtual void inform();
};


class Ammo : public Pickup
{
public:
    Ammo(StudentWorld* swd, int x, int y);
    virtual void inform();
private:
};


class Bullet : public Accessible
{
public:
    Bullet (StudentWorld* swd, int x, int y, Direction d);
    virtual void doSomething();
    bool attemptHit();
};

#endif // ACTOR_H
