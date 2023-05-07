#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, int dir = 0, int depth = 0, double size = 1.0, int health = -1);
    virtual void doSomething();
    StudentWorld* getWorld();
    
    Actor* overlap(int x, int y, Actor* selfObj = nullptr);
    Actor* isObjectBlocking(int x, int y, Actor* selfObj = nullptr);
    bool peachOverlap(int x, int y);
    virtual void bonk(bool isInvPeach = false);
    
    bool isDamagable();
    bool isAlive();
    virtual bool blocksMovement();
    
    virtual void takeDamage();
    void setDead();
    void updateHealth(int setTo);
    int getHealth();
    
private:
    bool m_alive;
    bool m_takesDamage;
    int m_health;
    StudentWorld* m_world;
    
};

class Block: public Actor
{
public:
    Block(StudentWorld* world, int startX, int startY, int goodie = 0);
    bool blocksMovement();
    void bonk(bool isInvPeach = false);
private:
    int m_goodie;
};

class Peach: public Actor
{
public:
    Peach(StudentWorld* world, int startX = 0, int startY = 0);
    void bonk(bool isInvPeach = false);
    void doSomething();

    bool isInvincible();
    bool hasShootPower();
    bool hasJumpPower();
    bool isTempInvincible();
    bool canShootFireball();
    bool completedLevel();
    
    void giveShootPower();
    void giveJumpPower();
    void giveStarPower(int ticks);
    void endLevel();
    
private:
    
    int m_invincibleTicks = 0;
    int m_tempInvincibleTicks = 0;
    int m_fireballRechargeTicks = 0;
    int m_remainingJump = 0;
    bool m_hasJumpPower = false;
    bool m_hasShootPower = false;
    bool m_hasCompletedLevel = false;
};

class Pipe: public Actor
{
public:
    Pipe(StudentWorld* world, int startX, int startY);
    bool blocksMovement();
};


class Flag: public Actor
{
public:
    Flag(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
};


class Projectile: public Actor
{
public:
    Projectile(StudentWorld* world, int imageID, int startX, int startY, int dir);
private:
    virtual void doSomething();
};

class PeachFireball: public Projectile
{
public:
    PeachFireball(StudentWorld* world, int startX, int startY, int dir);
private:
    void doSomething();
};
    
class PiranhaFireball: public Projectile
{
public:
    PiranhaFireball(StudentWorld* world, int startX, int startY, int dir);
};

class Shell: public Projectile
{
public:
    Shell(StudentWorld* world, int startX, int startY, int dir);
};


class Mario: public Actor
{
public:
    Mario(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
};

class Goodie: public Actor
{
public:
    Goodie(StudentWorld* world, int imageID, int startX, int startY);
};

class Flower: public Goodie
{
public:
    Flower(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
};

class Mushroom: public Goodie
{
public:
    Mushroom(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
};

class Star: public Goodie
{
public:
    Star(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
};

class Enemy: public Actor
{
public:
    Enemy(StudentWorld* world, int imageID, int startX, int startY);
    void bonk(bool isInvPeach = false);
    virtual void takeDamage();
private:
    void doSomething();
};

class Goomba: public Enemy
{
public:
    Goomba(StudentWorld* world, int startX, int startY);
};

class Koopa: public Enemy
{
public:
    Koopa(StudentWorld* world, int startX, int startY);
    void bonk(bool isInvPeach);
    void takeDamage();
};

class Piranha: public Enemy
{
public:
    Piranha(StudentWorld* world, int startX, int startY);
private:
    void doSomething();
    int m_firingDelay;
};



#endif // ACTOR_H_
