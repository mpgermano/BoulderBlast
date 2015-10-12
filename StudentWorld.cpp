#include "StudentWorld.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <sstream>
#include <list>
#include <iomanip>
using namespace std;

StudentWorld::~StudentWorld()
{
    cleanUp();              //cleanup function and destructor do the same thing essentially
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}



int StudentWorld::init()
{
    m_bonus = 1000;             //Start of each level, bonus must be 1000
    nJewel = 0;                 // 0 jewels until added by loadLevel()
    revealed = false;           //exit isn't revealed yet
    levelComplete = false;      //level has not been completed
    
    return loadLevel();
}

int StudentWorld::move()
{
    setTopDisplay();        //Update the top screen;
    
    player->doSomething();
    
    if (!player->isAlive())     //if the player dies, immediately decrease lives and return
    {
        decLives();
        return  GWSTATUS_PLAYER_DIED;
    }
    
    if (levelComplete)
        return GWSTATUS_FINISHED_LEVEL;
    
    for (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        (*p)->doSomething();        //loop through all actors and call their doSomething() function
        
        if (!player->isAlive())     //ensure player is still alive
        {
            decLives();
            return  GWSTATUS_PLAYER_DIED;
        }
        
    }
    removeDead();       //remove actors that died on the current tick
    m_bonus--;
    
    if (nJewel == 0 && !revealed)       //if the exit is hidden and all Jewels have been collect, open it
    {
        openExit();
    }
    
    
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()        //used to delete dynamically allocated actors and erase pointer nodes
{                                   //for dead actors after each tick
    delete player;
    for (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
        delete *p;
    
    allActors.erase(allActors.begin(), allActors.end());
}



int StudentWorld::loadLevel()
{
    int lee = getLevel();
    
    if (lee == 100)     //can't play past level 100
        return GWSTATUS_PLAYER_WON;
    
    ostringstream lvl;      //ostringstream needed to set "level0x.dat" correctly for each level
    lvl.fill('0');
    lvl << setw(2) << lee;
    
    string currentLev = "level" + lvl.str() + ".dat";
    
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(currentLev);
    
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_PLAYER_WON;
    
    if (result == Level::load_fail_bad_format)
       return GWSTATUS_LEVEL_ERROR;
    
    for (int x = 0; x < VIEW_WIDTH; x++)      //loop through each space and allocate correct actor each time
    {
        for (int y = 0; y < VIEW_HEIGHT; y++)
        {
            Level::MazeEntry item = lev.getContentsOf(x, y);
            
            switch(item)        //push the appropriate actor onto the back of the list for each space
            {
                case Level::wall:
                {
                    allActors.push_back(new Wall(this, x, y));
                    break;
                }
            
                case Level::player:
                {
                    player = new Player(this, x, y);
                    break;
                }
                
                case Level::boulder:
                {
                    allActors.push_back(new Boulder(this, x, y));
                    break;
                }
                
                case Level::jewel:
                {
                    allActors.push_back(new Jewel(this, x, y));
                    nJewel++;
                    break;
                }
                    
                case Level::exit:
                {
                    allActors.push_back(new Exit(this, x, y));
                    break;
                }
                    
                case Level::extra_life:
                {
                    allActors.push_back(new ExtraLife(this, x, y));
                    break;
                }
                case Level::restore_health:
                {
                    allActors.push_back(new RestoreHealth(this, x, y));
                    break;
                }
                    
                case Level::ammo:
                {
                    allActors.push_back(new Ammo(this, x, y));
                    break;
                }
                 
                case Level::hole:
                {
                    allActors.push_back(new Hole(this, x, y));
                    break;
                }
                
                case Level::horiz_snarlbot:
                {
                    allActors.push_back(new SnarlBot(this, x, y, GraphObject::right));
                    break;
                }
                    
                case Level::vert_snarlbot:
                {
                    allActors.push_back(new SnarlBot(this, x, y, GraphObject::down));
                    break;
                }
                    
                case Level::KleptoBot_factory:
                {
                    allActors.push_back(new Factory(this, x ,y, false));
                    break;
                }
                    
                case Level::angry_KleptoBot_factory:
                {
                    allActors.push_back(new Factory(this, x, y, true));
                    break;
                }
            }
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setTopDisplay()
{
    int score = getScore();     //update variables to current amounts
    int level = getLevel();
    int lives = getLives();
    
    ostringstream temp;
    temp << score;
    
    ostringstream scr;
    scr.fill('0');      //use .fill to fill in unfilled spaces in the score spot with 0's
    scr << setw(7) << score;        //setw(7) ensures that the score will have 7 digit places
    
    ostringstream lvl;
    lvl.fill('0');
    lvl << setw(2) << level;
    
    ostringstream liv;
    liv << setw(2) << lives;
    
    ostringstream hea;
    hea << setw(3) << player->healthPercent();  //fill in the health space with 3 digit places
    
    ostringstream amm;
    amm << setw(3) << player->ammoCount();
    
    ostringstream bon;
    bon << setw(4) << m_bonus;
    
    
    
    string top = "Score: " + scr.str() + "  Level: " + lvl.str() + "  Lives: " + liv.str() + "  Health: " + hea.str() + "%  Ammo: " + amm.str() + "  Bonus: " + bon.str();
    
    setGameStatText(top);       //set the top stat area to the above string
    
}

void StudentWorld::removeDead()
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        Jewel* jel = dynamic_cast<Jewel*>(*p);
        if (jel != nullptr && !jel->isAlive())
            nJewel--;   //if the Actor is a jewel, decrement nJewel--
        
        if (!(*p)->isAlive())
        {
            delete *p;
            allActors.erase(p);    //if the actor is dead delete and erase
        }
    }
}

void StudentWorld::setLevelComplete()
{
    levelComplete = true;
}

void StudentWorld::addRunningBonus()
{
    increaseScore(m_bonus);    //increase the score by the running bonus that is left as the player finishes
}

void StudentWorld::openExit()
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        Exit* ex = dynamic_cast<Exit*>(*p);
        if (ex != nullptr)      //find the exit
        {
            ex->setVisible(true);       //make it visible and set the variable to true
            ex->setOpen();
        }
        
    }
    playSound(SOUND_REVEAL_EXIT);
    revealed = true;
}



bool StudentWorld::doCensusCount(int x, int y, int& count)
{
    //the census is taken of a 7 x 7 area, the top, bottom, left, and right of the area must be set prior
    if (objectsOnSpace(x, y) > 1) //ensure the factory is by itself
        return false;
    
    int rightCheck = x + 3;
    if (rightCheck > VIEW_WIDTH -1)     //ensure right is not too big
        rightCheck = VIEW_WIDTH - 1;
    
    int leftCheck = x - 3;
    if (leftCheck < 0)                  //ensure left is not too small
        leftCheck = 0;
    
    int topCheck = y + 3;
    if (topCheck > VIEW_HEIGHT - 1)     //ensure top is not too high
        topCheck = VIEW_HEIGHT - 1;
    
    int bottomCheck = y - 3;
    if (bottomCheck < 0)                //ensure bottom is not too low
        bottomCheck = 0;
    
    
    for (int k = leftCheck; k <= rightCheck; k++)      //loop through area
    {
        for (int h = bottomCheck; h <= topCheck; h++)
        {
            
            Actor* checkSpot = spaceContains(h, k);
            if (checkSpot != nullptr && checkSpot->countsInFactoryCount()) //if it is a Kleptobot increment
                count++;
        }
    }
    
    return true;
}

bool StudentWorld::containsBarrier(int x, int y)
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        if ((*p)->getX() == x && (*p)->getY() == y &&(*p)->isBarrier()) //check that any object at given
            return true;                                                //space serves as a barrier
    }
    
    return false;
}

bool StudentWorld::checkPlayer(int x, int y)        //check that the player is not on the given space
{
    if (player->getX() == x && player->getY() == y)
        return false;
    
    return true;
}

int StudentWorld::objectsOnSpace(int x, int y)  //counts the number of Actors on the given space
{                                               //***Player not included in the count
    int count = 0;
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        if ((*p)->getX() == x && (*p)->getY() == y)
            count++;
    }
    
    return count;
}



void StudentWorld::addBullet(int x, int y, Accessible::Direction facing)
{
    allActors.push_back(new Bullet(this, x, y, facing));
}

void StudentWorld::addKleptoBot(int x, int y, int bonus)
{
    if (bonus == 10)    // bonus if 10 for a RegularKleptoBot
        allActors.push_back(new RegularKleptoBot(this, x, y));
    
    else if (bonus == 20)   // bonus if 20 for an AngryKleptoBot
        allActors.push_back(new AngryKleptoBot(this, x, y));
}

void StudentWorld::addGoodieBack(int x, int y, char goodieKind)
{
    if (goodieKind == 'L')      //the char goodieKind determines the kind of goodie to add back to the game
        allActors.push_back(new ExtraLife(this, x, y));
    
    if (goodieKind == 'R')
        allActors.push_back(new RestoreHealth(this, x, y));
    
    if (goodieKind == 'A')
        allActors.push_back(new Ammo(this, x, y));
}



Actor* StudentWorld::spaceContains(int x, int y)        //valuable function throughout the program
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        Actor* ptr = *p;
        if ((*p)->getX() == x && (*p)->getY() == y)
            return ptr; //return first object in the list on the give space
    }
    
    if (player->getX() == x && player->getY() == y)
        return player;
    
    
    return nullptr;
}

Actor* StudentWorld::secondBulletCheck(int x, int y)    //used to ensure that no Actor on the space
{                                                       //can be struck by a bullet
        for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
        {
            if ((*p)->getX() == x && (*p)->getY() == y && (*p)->getBulletEffect() == BULLET_STRIKES)
            return *p;
        }
    
    return nullptr;
}

Player* StudentWorld::getPlayer()
{
    return player;
}

Boulder* StudentWorld::getBoulder(int x, int y)     //return a pointer to a boulder at the intended space
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        Boulder* bd = dynamic_cast<Boulder*>(*p);
        if (bd != nullptr && bd->getX() ==  x && bd->getY() == y)
            return bd;
    }
    
    return nullptr;
        
}

Pickup* StudentWorld::takeStealable(int x, int y)       //return a pointer to a Stealable item a given (x,y)
{
    for  (list<Actor*>::iterator p = allActors.begin(); p != allActors.end(); p++)
    {
        if ((*p)->getX() == x && (*p)->getY() == y && (*p)->isStealable())
        {
            Pickup* pck = dynamic_cast<Pickup*>(*p);
            if (pck != nullptr)
               return pck;
        }
    }
    
    return nullptr;
}
