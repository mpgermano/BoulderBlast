#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <iostream>
#include <list>
#include <string>

class Player;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{}
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    
    //Game Functioning
    int loadLevel();
    void setTopDisplay();
    void removeDead();
    void setLevelComplete();
    void addRunningBonus();
    void openExit();
    
    
    //Checks (Related to Actors Functioning Properly)
    bool doCensusCount(int x, int y, int& count);
    bool containsBarrier(int x, int y);
    bool checkPlayer(int x, int y);
    int objectsOnSpace(int x, int y);
    
    
    //Add Actor
    void addBullet(int x, int y, Accessible::Direction facing);
    void addKleptoBot(int x, int y, int bonus);
    void addGoodieBack(int x, int y, char goodieKind);
    
    //Pointers
    Actor* spaceContains(int x, int y);
    Actor* secondBulletCheck(int x, int y);
    Player* getPlayer();
    Boulder* getBoulder(int x, int y);
    Pickup* takeStealable(int x, int y);
           // needed to check for robots on the same space as a
                                                  // as a pickup
    
private:
    list<Actor*> allActors;
    Player* player;
    unsigned int m_bonus;
    int nJewel;
    bool revealed;
    bool levelComplete;
};
#endif // STUDENTWORLD_H_
