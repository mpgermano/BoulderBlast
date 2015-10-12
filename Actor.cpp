#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include <cstdlib>


//Throughout this code, there are many checks solely relating to the player.  This is due to that fact
//that the player is stored individually as a pointer whereas every other Actor is contained in a list

Actor::Actor(StudentWorld* swd, int ID, int x, int y, Direction start, bool Barrier, int bulletEffect, int hitPoints)
: GraphObject(ID, x, y, start), m_world(swd), m_isBar(Barrier),
    m_isAlive(true), m_bulletEffect(bulletEffect), m_hitPoints(hitPoints)
{
    setVisible(true);       //all Actors (other than exits) are immediately set visible
    
}

void Actor::doDamage()
{
    m_hitPoints -= 2;  
    
    if (m_hitPoints <= 0)
        setDead();
}

void Actor::setHealth(int toNum)
{
    m_hitPoints = toNum;
}

void Actor::setDead()
{
    m_isAlive = false;
}

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

bool Actor::isStealable() const
{
    return false;       //most actors cannot be stolen, only goodies
}

bool Actor::countsInFactoryCount() const
{
    return false;       //most actors do not count in the Factory's census
}

int Actor::getBulletEffect() const
{
    return m_bulletEffect;      //returns an integer that corresponds to the effect when a bullet hits it
}

bool Actor::isBarrier() const
{
    return m_isBar;
}

bool Actor::isAlive() const
{
    return m_isAlive;
}

int Actor::getHealth() const
{
    return m_hitPoints;
}



Agent::Agent(StudentWorld* swd, int ID, int x, int y, int health, int sound, Direction start)
: Actor(swd, ID, x, y, start, true, BULLET_STRIKES, health), m_sound(sound),
deadBot(false)
{}

bool Agent::moveRegular(Direction d)        //all agents move according to the same rules
{
    
    switch (d)      //d can be four possible values: up, down, left or right
    {
        case up:
        {
            Actor* check = getWorld()->spaceContains(getX(), getY() + 1); //returns pointer to that space
            
            
            //the following code ensures that the space is either empty or does not contain the player, and that there exists no barrier on that space.
            if (check == nullptr || (getWorld()->checkPlayer(getX(), getY() + 1) &&
                                     !getWorld()->containsBarrier(getX(), getY() + 1)))
            {
                moveTo(getX(), getY() + 1);     //move up
                return true;
            }
            return false;
        }
            
        case right:     //The content of each case is identical other than the changes necessary for
        {               //different directions
            
            Actor* check = getWorld()->spaceContains(getX() + 1, getY());
            
            if (check == nullptr || (getWorld()->checkPlayer(getX() + 1, getY()) &&
                                     !getWorld()->containsBarrier(getX() + 1, getY())))
            {
                moveTo(getX() + 1, getY());    //move right
                return true;
            }
            return false;
        }
            
        case down:
        {
            Actor* check = getWorld()->spaceContains(getX(), getY() - 1);
            
            if (check == nullptr || (getWorld()->checkPlayer(getX(), getY() - 1) &&
                                     !getWorld()->containsBarrier(getX(), getY() - 1)))
            {
                moveTo(getX(), getY() - 1);     //move down
                return true;
            }
            
            return false;
        }
            
        case left:
        {
            Actor* check = getWorld()->spaceContains(getX() - 1, getY());
            if (check == nullptr || (getWorld()->checkPlayer(getX() - 1, getY()) &&
                                     !getWorld()->containsBarrier(getX() - 1, getY())))
            {
                moveTo(getX() - 1, getY());     //move left
                return true;
            }
            
        
            return false;
        }
        default:
            return false;
    }
    return false;       //code should never be reached, assuming switch direction is given correctly
}

void Agent::shoot()
{
    switch (getDirection())     //this switch direction is used to determine the location and direction of
    {                           // dynamically allocated bullet
        case up:
        {
            getWorld()->addBullet(getX(), getY() + 1, up);
            getWorld()->playSound(m_sound);
            break;
        }
        case right:
        {
            getWorld()->addBullet(getX() + 1, getY(), right);
            getWorld()->playSound(m_sound);
            break;
        }
            
        case down:
        {
            getWorld()->addBullet(getX(), getY() - 1, down);
            getWorld()->playSound(m_sound);
            break;
        }
        case left:
        {
            getWorld()->addBullet(getX() - 1, getY(), left);
            getWorld()->playSound(m_sound);
            break;
        }
    }
    
}



Player::Player(StudentWorld* swd, int x, int y)
: Agent(swd, IID_PLAYER, x, y, 20, SOUND_PLAYER_FIRE, right), m_ammo(20)
{}

void Player::doSomething()
{
    if (!isAlive())
        return;
    
    int key;
    if (getWorld()->getKey(key))        //function in studentWorld to get input direction from user
    {
        switch (key)        // 6 options for the user
        {
            case KEY_PRESS_DOWN:
            {
                setDirection(down);
                Actor* act = getWorld()->spaceContains(getX(), getY() - 1);
                
                if (act != nullptr)
                {
                    Boulder* bd = dynamic_cast<Boulder*>(act);      //used to check for a Boulder in
                    if (bd != nullptr)                              //the intended space to move to
                    {
                        if(bd->push(getX(), getY() - 2))            //dynamic_cast pointer needed
                            moveRegular(down);                      // since Boulders serve as exception
                                                                    //Sometimes they are a barrier,
                        break;                                      //sometimes they are moveable
                    }
                }
                
                moveRegular(down);
                break;
            }
                
            case KEY_PRESS_UP:
            {
                setDirection(up);
                Actor* act = getWorld()->spaceContains(getX(), getY() + 1);
                
                if (act != nullptr)
                {
                    Boulder* bd = dynamic_cast<Boulder*>(act);
                    if (bd != nullptr)
                    {
                        if (bd->push(getX(), getY() + 2))       //if the space does have a boulder, push it
                            moveRegular(up);
                        break;
                    }
                }
                moveRegular(up);
                break;
            }
                
            case KEY_PRESS_LEFT:
            {
                setDirection(left);
                Actor* act = getWorld()->spaceContains(getX() - 1, getY());
                
                if (act != nullptr)
                {
                    Boulder* bd = dynamic_cast<Boulder*>(act);
                    if (bd != nullptr)
                    {
                        if(bd->push(getX() - 2, getY()))
                            moveRegular(left);
                        break;
                    }
                    
                }
                moveRegular(left);
                break;
            }
                
            case KEY_PRESS_RIGHT:
            {
                setDirection(right);
                Actor* act = getWorld()->spaceContains(getX() + 1, getY());
                
                if (act != nullptr)
                {
                    Boulder* bd = dynamic_cast<Boulder*>(act);
                    if (bd != nullptr)
                    {
                        if (bd->push(getX() + 2, getY()))
                            moveRegular(right);
                        break;
                    }
                    
                }
                moveRegular(right);
                break;
            }
                
            case KEY_PRESS_ESCAPE:
            {
                setDead();      //give up life to restart the level
                break;
            }
                
            case KEY_PRESS_SPACE:
            {
                if (m_ammo > 0)     //if you have ammo, shoot
                {
                    m_ammo--;
                    shoot();        //use Player's base class Agent's shoot function
                }
                break;
            }
        }
    }
}

void Player::doDamage()
{
    Actor::doDamage();      //use Actor's doDamage() function to decrease health
    if (isAlive())
        
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    else
        getWorld()->playSound(SOUND_PLAYER_DIE);
    
}

void Player::addAmmo()
{
    m_ammo += 20;
}

int Player::healthPercent() const
{
    return (getHealth()* 5);
}

int Player::ammoCount() const
{
    return m_ammo;
}



Robot::Robot(StudentWorld* swd, int ID, int x, int y, int health, Direction d, int bonus)
: Agent(swd, ID, x, y, health, SOUND_ENEMY_FIRE, d ), m_bonus(bonus), m_continue(false)
{setTick();}

void Robot::doSomething()       //this is the beginning sequence for all 3 robots doSomething()
{
    m_continue = false;
    
    if (!isAlive())
        return;
    
    if (m_tick != 1)     //ensure that the Robot is only acting one per their tick int
    {
        decTick();
        return;
    }
    
    if (doesShoot() && canAim())       //if the robot is a shooting Robot and it is facing toward the player
    {                                  //with a clear, straight path it should shoot
        shoot();
        setTick();
        return;
    }
    
    m_continue = true;
    
}

void Robot::doDamage()
{
    Actor::doDamage();  //use Actor's doDamage function to decrease health by 2 hit-points
    
    if (isAlive())
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else
    {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(m_bonus);     //every robot has an individual bonus given to the player
    }                                           //if it is killed
    
}

bool Robot::canAim()        //used to see if the player can be shot by the robot
{
    
    //The following series of code repeats the same style of checks for all directions and possibilites:
    
    //1. It checks to see if one of the coordinates of the robot is the same as the player's
    //2. Depending on how the player's second coordinate relates to the robot's second coord, it checks
                                //that the robot's direction is correct for these relative positions
    //3. It then loops through all blocks in the way to check that the path is clear for the bullet
    //4. If all of these are true, it returns true
    
    
    if (getWorld()->getPlayer()->getX() == getX())
    {
        if (getWorld()->getPlayer()->getY() > getY() && getDirection() == up)
        {
            for (int k = 1; k < getWorld()->getPlayer()->getY() - getY(); k++)
            {
                if (getWorld()->spaceContains(getX(), getY() + k) != nullptr)
                {
                    if (getWorld()->spaceContains(getX(), getY() + k)->isBarrier())
                        return false;
                }
            }
            return true;
        }
        
        else if (getWorld()->getPlayer()->getY() < getY() && getDirection() == down)
        {
            for (int k = 1; k < getY() - getWorld()->getPlayer()->getY(); k++)
            {
                if (getWorld()->spaceContains(getX(), getY() - k) != nullptr)
                {
                    if (getWorld()->spaceContains(getX(), getY() - k)->isBarrier())
                        return false;
                }
            }
            return true;
        }
    }
    
    
    else if (getWorld()->getPlayer()->getY() == getY())
    {
        if (getWorld()->getPlayer()->getX() > getX() && getDirection() == right)
        {
            
            for (int k = 1; k < getWorld()->getPlayer()->getX() - getX(); k++)
            {
                if (getWorld()->spaceContains(getX() + k, getY()) != nullptr)
                {
                    if (getWorld()->spaceContains(getX() + k, getY())->isBarrier())
                        return false;
                }
            }
            return true;
            
        }
        
        else if (getWorld()->getPlayer()->getX() < getX() && getDirection() == left)
        {
            for (int k = 1; k < getX() - getWorld()->getPlayer()->getX(); k++)
            {
                if (getWorld()->spaceContains(getX() - k, getY()) != nullptr)
                {
                    if (getWorld()->spaceContains(getX() - k, getY())->isBarrier())
                        return false;
                }
            }
            return true;
            
        }
    }
    
    return false;
}

void Robot::setTick()       //This sets the robots tick to ensure it operates at the correct pace
{
    m_tick = (28 - getWorld()->getLevel()) / 4;     //This code was given by the spec
    
    if (m_tick < 3)
        m_tick = 3;
}

void Robot::decTick()
{
    m_tick--;
}

bool Robot::doesShoot() const
{
    return true;
}

bool Robot::shouldContinue() const
{
    return m_continue;
}



SnarlBot::SnarlBot(StudentWorld* swd, int x, int y, Direction d)
: Robot(swd, IID_SNARLBOT, x, y, 10 ,d, 100)
{}

void SnarlBot::doSomething()
{
    Robot::doSomething();       //use Base class's doSomething() to complete first few checks
    
    if (!shouldContinue())      //if the Robot base class's doSomething() returned early, then return
        return;
    
    bool didMove = moveRegular(getDirection());     //move with the agent's moveRegular function
    
    if (!didMove)       //if the SnarlBot couldn't move, reverse it's current direction
    {
        if (getDirection() == right)
            setDirection(left);
        
        else if (getDirection() == left)
            setDirection(right);
        
        else if (getDirection() == up)
            setDirection(down);
        
        else if (getDirection() == down)
            setDirection(up);
        
    }
    
    setTick();      //reset tick, since tick == 1
}



KleptoBot::KleptoBot(StudentWorld* swd, int ID, int x, int y, int health, int bonus)
: Robot(swd, ID, x, y, health, right , bonus), hasGoodie(false), m_currentDirectionSteps(0)
{
    m_distanceBeforeTurning = rand() % 6 + 1;       //sets the distance to a random number between 1 & 6
}

void KleptoBot::doSomething()
{
    Robot::doSomething();       //start the function off as all robot's do
    
    if (!shouldContinue())
        return;
    
    bool didMove = false;
    
    Pickup* goodie = getWorld()->takeStealable(getX(), getY());
    
    if (goodie != nullptr && !hasGoodie)        //if the Robot is on a goodie, and hasn't already taken one:
    {
        int stealChance = rand() % 10 + 1;  //set stealChance to a random number between 1 & 10
        if (stealChance == 5)   //5 is an arbitray number as it could be any number between 1 & 10
        {
            goodie->setDead();      //destroy the goodie
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            hasGoodie = true;
            if (goodie->getBonus() == 1000)     //By knowning the bonuns associated with the bonus, it's
                goodieKind = 'L';               //type can be known and set to be re-created later
            else if (goodie->getBonus() == 500) //when/if the KleptoBot is killed
                goodieKind = 'R';
            else if (goodie->getBonus() == 100)
                goodieKind = 'A';
            setTick();                          //reset the tick, since it == 1
            return;
        }
        
    }
    //if the KleptoBot hasn't moved it's random limit of steps before it must turn
    //and if the KleptoBot can move, then it is to move, increase it's count, reset it ticks and return
    if (m_currentDirectionSteps <= m_distanceBeforeTurning)
    {
        didMove = moveRegular(getDirection());
        
        if (didMove)
        {
            m_currentDirectionSteps++;
            setTick();
            return;
        }
    }
    
    
    if (!didMove)       //if it couldn't move, then it must select a random direction and try to move again
    {
        m_distanceBeforeTurning = rand() % 6 + 1;
        
        int y = 0;
        bool doneUp = false;
        bool doneRight = false;
        bool doneDown = false;
        bool doneLeft = false;
        
        
        while (int x = rand() % 4 + 1)  //loop serves to check all 4 direction randomly if needed
        {                               //the done(Dir) boolean ensures that no direction is tested twice
            
            if (y == 0)   //if it's the first iteration of the loop, set y = to the random number from 1 - 4
                y = x;    //if the KleptoBot cannot move, y will be used to set the direction the Bot faces
            
            if (x == 1 && !doneUp)
            {
                
                if (moveRegular(up))
                {
                    setDirection(up);
                    setTick();
                    return;
                }
                
                doneUp = true;
            }
            
            if (x == 2 && !doneRight) // if it hasn't been tested:
            {
                if (moveRegular(right))     //check to move right
                {
                    setDirection(right);        //if it can move, change the direction and reset the Tick
                    setTick();
                    return;
                }
                doneRight = true;
            }
            
            if (x == 3 && !doneDown)
            {
                if (moveRegular(down))
                {
                    setDirection(right);
                    setTick();
                    return;
                }
                doneDown = true;
            }
            if (x == 4 && !doneLeft)
            {
                if(moveRegular(left))
                {
                    setDirection(left);
                    setTick();
                    return;
                }
                doneLeft = true;
            }
            
            if (doneUp && doneRight && doneDown && doneLeft)
                break;
        }
        
        //if the Bot cannot move in any direction, set the direction the Bot faces to the first random dir
        
        if (y == 1)
            setDirection(up);
        if (y == 2)
            setDirection(right);
        if (y == 3)
            setDirection(down);
        if (y == 4)
            setDirection(left);
    }
    
    setTick();
}

void KleptoBot::doDamage()
{
    Robot::doDamage();
    if (!isAlive() && hasGoodie)       //if the KleptoBot has a goodie
    {                                  //add it back to where the Bot is when it dies
        getWorld()->addGoodieBack(getX(), getY(), goodieKind);
        
    }
    
}

bool KleptoBot::countsInFactoryCount() const
{
    return true;    //KleptoBot are the only thing counted in the Factory Census
}



RegularKleptoBot::RegularKleptoBot(StudentWorld* swd, int x, int y)
:KleptoBot(swd, IID_KleptoBot, x, y, 5, 10)
{}

bool RegularKleptoBot::doesShoot() const
{
    return false;
}



AngryKleptoBot::AngryKleptoBot(StudentWorld* swd, int x, int y)
:KleptoBot(swd,IID_ANGRY_KleptoBot, x, y, 8, 20)
{}



Barrier::Barrier(StudentWorld* swd, int ID, int x, int y, int hitPoints)
: Actor(swd, ID, x, y, none, true, BULLET_DESTROYED, hitPoints)
{}

void Barrier::doSomething(){}       //Barriers (other than Factories) do nothing



Wall::Wall(StudentWorld* swd, int x, int y)
: Barrier(swd, IID_WALL, x, y)
{}



Boulder::Boulder(StudentWorld* swd, int x, int y)
: Barrier(swd, IID_BOULDER, x, y, 10)
{}

bool Boulder::push(int moveX, int moveY)
{
    Actor* space = getWorld()->spaceContains(moveX, moveY);
    
    Hole* hol = dynamic_cast<Hole*>(space);     //dynamic_cast pointer needed to check for a Hole
    
    if (space == nullptr || hol != nullptr)     //if the space is empty, or there is a hole, move there
    {
        moveTo(moveX, moveY);
        return true;
    }
    
    return false;
}

int Boulder::getBulletEffect() const
{
    return BULLET_STRIKES;      //unlike other Barriers, a Boulder is affected and damaged by a bullet
}



Hole::Hole(StudentWorld* swd, int x, int y)
:Barrier(swd, IID_HOLE, x, y)
{}

void Hole::doSomething()
{
    if (!isAlive())
        return;
    
    Boulder* check = getWorld()->getBoulder(getX(), getY());
    
    if (check != nullptr)       //if a Boulder is on itself, is dies and the boulder does as well
    {
        setDead();
        check->setDead();
    }
    
}

int Hole::getBulletEffect() const
{
    return BULLET_NOTHING;      //unlike other barriers, a bullet is unaffected by a Hole
}



Factory::Factory(StudentWorld* swd, int x, int y, bool angry)
:Barrier(swd, IID_ROBOT_FACTORY, x, y), m_angry(angry)
{}

void Factory::doSomething()
{
    int random = rand() % 50 + 1;
    int count = 0;
    bool result = getWorld()->doCensusCount(getX(), getY(), count);     //take census of 7 x 7 surrounding
    
    if (result && count < 3)    //if there is no Bot on top of the factory, and the count of Kleptos < 3:
    {
        if (random == 10 && !m_angry)          //25 is an arbitrary number, could be any number x for which 1 <= x <= 50
            
        //There is a 1 in 50 chance every tick that a KleptoBot will be added.  Depending on the
        //specification of the level, a particular factory makes either RegularKleptoBots or AngryKleptos
        {
            getWorld()->addKleptoBot(getX(), getY(), 10);
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
        
        else if (random == 10 && m_angry)
        {
            getWorld()->addKleptoBot(getX(), getY(), 20);
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
    
}



Accessible::Accessible(StudentWorld* swd, int ID, int x, int y, Direction start)
: Actor(swd, ID, x, y,start,false)
{}

Pickup::Pickup(StudentWorld* swd, int ID, int x, int y, int bonus, int sound)
: Accessible(swd, ID, x, y), m_bonus(bonus), m_sound(sound)
{}

void Pickup::doSomething()
{
    if (!isAlive() || !isOpen())        //if it's Alive and open (only meaningful for the exit), continue
        return;
    
    if (!getWorld()->checkPlayer(getX(), getY()))
    {                                                   //if the player picks up the Pickup
        
        getWorld()->increaseScore(m_bonus);     //give the associated bonus, play the associated sound, kill
        setDead();
        getWorld()->playSound(m_sound);
        inform();                           //this function does different things for different pickups
    }                                       //this function allows the general Base class doSomething to
}                                           //function properly

bool Pickup::isOpen() const
{
    return true;
}

bool Pickup::isStealable() const
{
    return true;
}

int Pickup::getBonus() const
{
    return m_bonus;
}


Jewel::Jewel(StudentWorld* swd, int x, int y)
: Pickup(swd, IID_JEWEL, x, y, 50)
{}

void Jewel::inform()
{}

bool Jewel::isStealable() const
{
    return false;
}



Exit::Exit(StudentWorld* swd, int x, int y)
: Pickup(swd, IID_EXIT, x, y, 2000, SOUND_FINISHED_LEVEL), m_open(false)
{
    setVisible(false);      //unlike all other Actors, the exit starts off as invisible
    
}

void Exit::inform()
{
    getWorld()->addRunningBonus();      //the Exit additionally adds the running bonus
    getWorld()->setLevelComplete();     //and sets the level as completed
}

void Exit::setOpen()
{
    m_open = true;
}

bool Exit::isOpen() const
{
    return m_open;
}

bool Exit::isStealable() const
{
    return false;
}



ExtraLife::ExtraLife(StudentWorld* swd, int x, int y)
:Pickup(swd, IID_EXTRA_LIFE, x, y, 1000)
{}

void ExtraLife::inform()
{
    getWorld()->incLives();
}


RestoreHealth::RestoreHealth(StudentWorld* swd, int x, int y)
:Pickup(swd, IID_RESTORE_HEALTH, x, y, 500)
{}

void RestoreHealth::inform()
{
    getWorld()->getPlayer()->setHealth(20);
}



Ammo::Ammo(StudentWorld* swd, int x, int y)
: Pickup(swd, IID_AMMO, x, y, 100)
{}

void Ammo::inform()
{
    getWorld()->getPlayer()->addAmmo();
}



Bullet::Bullet(StudentWorld* swd, int x, int y, Direction d)
:Accessible(swd, IID_BULLET, x, y, d)
{}

void Bullet::doSomething()
{
    if (!isAlive())
        return;
    

    if (attemptHit())       //attempts to hit the Actor on the current space
        return;
    
    switch (getDirection()) //if couldn't hit, it move ahead one space in it's given direction
    {
        case up:
        {
            moveTo(getX(), getY() + 1);
            break;
        }
        case right:
        {
            moveTo(getX() + 1, getY());
            break;
        }
        case down:
        {
            moveTo(getX(), getY() - 1);
            break;
        }
        case left:
        {
            moveTo(getX() - 1, getY());
            break;
        }
    
    }
    
    attemptHit();       //attempts to hit what is on it's new space
    
}

bool Bullet::attemptHit()
{
    bool playerShot = getWorld()->checkPlayer(getX(), getY());
    
    if (!playerShot)     //if the bullet and the player are on the same space, doDamage & set bullet dead
    {
        getWorld()->getPlayer()->doDamage();
        setDead();
        return true;
    }
    
    Actor* target = getWorld()->spaceContains(getX(), getY());
    Actor* target2 = getWorld()->secondBulletCheck(getX(), getY());
    
    
    if (target != nullptr)      // realistically target can never be the nullptr since spaceContains will
        // at least return a pointer to the actor in bullet.
    {
        
        if (target->getBulletEffect() == BULLET_STRIKES)        //BULLET_STRIKES means to do damage
        {
            target->doDamage();
            setDead();
            return true;
        }
        
        else if(target->getBulletEffect() == BULLET_DESTROYED)      //hit a barrier
        {
            if (target2 == nullptr) //this checks to ensure there are not multiple Actors on the same space
            {                       //For example, if there is a jewel and robot, the bullet must strike.
                setDead();          //If the first target pointer is set pointing to the jewel though, the
                return true;        //bullet will think to continue on.  THis shows that a 2nd check is
            }                       //neede
            else
            {
                target2->doDamage();
                setDead();
                return true;
            }
        }
        
        else if(target->getBulletEffect() == BULLET_NOTHING)
        {
            if (target2 != nullptr)     //do 2nd check again
            {
                target2->doDamage();
                setDead();
                return true;
            }
        }
    }
    return false;
}







