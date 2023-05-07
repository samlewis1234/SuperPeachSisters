#include "Actor.h"
#include "StudentWorld.h"

//Actor Class: includes all characters, projectiles, block
Actor::Actor(int imageID, int startX, int startY, StudentWorld* world, int dir, int depth, double size, int health) : GraphObject(imageID, startX, startY, dir, depth, size), m_world(world) {
    m_alive = true; //all Actors start alive    
    if (health == -1) { //-1 is a placeholder for objects that dont take damage (blocks)
        m_takesDamage = false;
    }
    else {
        m_takesDamage = true;
        m_health = health;
    }
}

void Actor::doSomething() {} //does nothing, overwritten by subclasses

StudentWorld* Actor::getWorld() { return m_world; }

//returns pointer to overlapping actor, nullptr if no overlap
Actor* Actor::overlap(int x, int y, Actor* selfObj) {
    int objectX;
    int objectY;
    
    //gets the list of Actors
    std::list<Actor*> actorList = getWorld()->getActors();
    std::list<Actor*>::iterator listIter = actorList.begin();
    int size = actorList.size();
    
    for (int i = 0; i < size; i++) {        //interate through list of actors
        objectX = (*listIter)->getX();      //for each one, check for overlap
        objectY = (*listIter)->getY();
        
        if (x < (objectX+SPRITE_WIDTH) && (x+SPRITE_WIDTH) > objectX && y < (objectY+SPRITE_HEIGHT) && (y+SPRITE_HEIGHT) > objectY) {
            if (*listIter != selfObj) {
                return *listIter;
            }
        }
        listIter++;
    }
    return nullptr; //if no overlap
}

//same function as overlap, but chacks if the overlapping actor blocks movement
Actor* Actor::isObjectBlocking(int x, int y, Actor* selfObj) {
    int objectX;
    int objectY;
    
    //gets the list of Actors
    std::list<Actor*> actorList = getWorld()->getActors();
    std::list<Actor*>::iterator listIter = actorList.begin();
    int size = actorList.size();
    
    for (int i = 0; i < size; i++) {        //interate through list of actors
        objectX = (*listIter)->getX();      //for each one, check for overlap
        objectY = (*listIter)->getY();
        
        if (x < (objectX+SPRITE_WIDTH) && (x+SPRITE_WIDTH) > objectX && y < (objectY+SPRITE_HEIGHT) && (y+SPRITE_HEIGHT) > objectY) {
            if (*listIter != selfObj && (*listIter)->blocksMovement()) { //if object overlaps and blocks movement
                return *listIter;
            }
        }
        listIter++;
    }
    return nullptr; //if no overlap
}

//is actor overlapping peach, T/F
bool Actor::peachOverlap(int x, int y) {
    int peachX = getWorld()->getPeach()->getX();
    int peachY = getWorld()->getPeach()->getY();
    
    if (x < (peachX+SPRITE_WIDTH) && (x+SPRITE_WIDTH) > peachX && y < (peachY+SPRITE_HEIGHT) && (y+SPRITE_HEIGHT) > peachY) {
            return true;
    }
    return false; //if no overlap
}

int Actor::getHealth() { return m_health; }

bool Actor::isDamagable() { return m_takesDamage; }

void Actor::bonk(bool isInvPeach) {}

bool Actor::isAlive() { return m_alive; }

bool Actor::blocksMovement() { return false; } //default value

void Actor::takeDamage() {
    if (isDamagable()) {
        m_health--;
    }
}

void Actor::setDead() { m_alive = false; }

void Actor::updateHealth(int setTo) { m_health = setTo; }



Block::Block(StudentWorld* world, int startX, int startY, int goodie) : Actor(IID_BLOCK, startX, startY, world, 0, 2, 1), m_goodie(goodie) {}

bool Block::blocksMovement() { return true; }

//block gets boonked: drops goodie if available
void Block::bonk(bool isInvPeach) {
    if (m_goodie == 0) { //if no goodie
        getWorld()->playSound(SOUND_PLAYER_BONK);
}
    else { //if goodie
        getWorld()->playSound(SOUND_POWERUP_APPEARS);
        
        if (m_goodie == IID_STAR) {
                getWorld()->addActor(new Star(getWorld(), getX(), getY()+8));
        }
        else if (m_goodie == IID_MUSHROOM) {
                getWorld()->addActor(new Mushroom(getWorld(), getX(), getY()+8));
        }
        else if (m_goodie == IID_FLOWER) {
            getWorld()->addActor(new Flower(getWorld(), getX(), getY()+8));
        }
        m_goodie = 0;
    }
}



Peach::Peach(StudentWorld* world, int startX, int startY) : Actor(IID_PEACH, startX, startY, world, 0, 0, 1, 1) {}

void Peach::doSomething() {
    //IS ALIVE
    if (!isAlive()) {
        return;
    }
    
    //IF INVINCIBLE
    if (m_invincibleTicks > 0) {
        m_invincibleTicks--;
    }
    if (m_tempInvincibleTicks > 0) {
        m_tempInvincibleTicks--;
    }
    
    //FIREBALL RECHARGE
    if (m_fireballRechargeTicks > 0) {
        m_fireballRechargeTicks--;
    }
    
    int x = getX();
    int y = getY();
    
    //BONK
    if (overlap(x, y, this) != nullptr) {
        overlap(x, y, this)->bonk(isInvincible());
    }
    
    //IS JUMPING
    bool isFalling = true;
    if (m_remainingJump > 0) {
        isFalling = false;
        int newY = y+4;
        Actor* blockingActor = isObjectBlocking(x, newY, this);
        if (blockingActor != nullptr) {
            blockingActor->bonk();
            m_remainingJump = 0;
        }
        else {
            moveTo(x, newY);
            m_remainingJump--;
            y = newY;
        }
    }
    
    //IS FALLING
    if (isFalling) {
        for (int i = 0; i < 4; i++) {
            Actor* blockingActor = isObjectBlocking(x, y-i, this);
            if (blockingActor != nullptr) {
                isFalling = false;
            }
        }
        if (isFalling) {
            moveTo(x, y-4);
            y = y-4;
        }
    }
    
    //MOVEMENT
    int ch;
    int newX;
    if (getWorld()->getKey(ch)) { // user hit a key during this tick!
        switch (ch) {
            case KEY_PRESS_LEFT: {
                setDirection(left);
                newX = x-4;
                if(isObjectBlocking(newX, y, this) == nullptr) { //if possible to move, move
                    moveTo(newX, y);
                }
                break;
            }
            case KEY_PRESS_RIGHT: {
                setDirection(right);
                newX = x+4;
                if(isObjectBlocking(newX, y, this) == nullptr) { //if possible to move, move
                    moveTo(newX, y);
                }
                break;
            }
            case KEY_PRESS_UP: {
                if(isObjectBlocking(x, y-1, this) != nullptr) { //if possible to jump, set m_remainingJump
                    if (!m_hasJumpPower) {
                        m_remainingJump = 8;
                    }
                    else {
                        m_remainingJump = 12;
                    }
                    getWorld()->playSound(SOUND_PLAYER_JUMP);
                }
                break;
            }
            case KEY_PRESS_SPACE: { //PEACH FIREBALL
                if (m_hasShootPower && m_fireballRechargeTicks == 0) {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_fireballRechargeTicks = 8;
                    if (getDirection() == left) {
                            getWorld()->addActor(new PeachFireball(getWorld(), x-4, y, left));
                    }
                    else {
                        getWorld()->addActor(new PeachFireball(getWorld(), x+4, y, right));
                    }
                }
            }
        }
    }
}

void Peach::giveShootPower() { m_hasShootPower = true; }

void Peach::giveJumpPower() { m_hasJumpPower = true; }

void Peach::giveStarPower(int ticks) { m_invincibleTicks = ticks; }

void Peach::endLevel() { m_hasCompletedLevel = true; }

//peach bonked: takes damage, loses powers
void Peach::bonk(bool isInvPeach) {
    takeDamage();
    m_tempInvincibleTicks = 10;
    m_hasShootPower = false;
    m_hasJumpPower = false;
    
    if (getHealth() > 0) { //if still alive
        getWorld()->playSound(SOUND_PLAYER_HURT);
    }
    else {
        setDead();
    }
}

bool Peach::isInvincible() {
    if (m_invincibleTicks > 0) {
        return true;
    }
    return false;
}

bool Peach::hasShootPower() { return m_hasShootPower; }

bool Peach::hasJumpPower() { return m_hasJumpPower; }

bool Peach::isTempInvincible() {
    if (m_tempInvincibleTicks > 0) {
        return true;
    }
    return false;
}

bool Peach::canShootFireball() {
    if (m_fireballRechargeTicks == 0) {
        return true;
    }
    return false;
}

bool Peach::completedLevel() { return m_hasCompletedLevel; }



Pipe::Pipe(StudentWorld* world, int startX, int startY) : Actor(IID_PIPE, startX, startY, world, 0, 2, 1) {}

bool Pipe::blocksMovement() { return true; }



Flag::Flag(StudentWorld* world, int startX, int startY) : Actor(IID_FLAG, startX, startY, world, 0, 1, 1) {}

void Flag::doSomething() {
    if (isAlive() && peachOverlap(getX(), getY())) { //if peach reached the flag
        getWorld()->increaseScore(1000);
        setDead();
        getWorld()->getPeach()->endLevel();
    }
}



Projectile::Projectile(StudentWorld* world, int imageID, int startX, int startY, int dir) : Actor(imageID, startX, startY, world, dir, 1, 1) {}

void Projectile::doSomething() {
    int x = getX();
    int y = getY();
    Actor* overlapActor;
    //if it overlaps peach
    if (peachOverlap(x, y)) {
        getWorld()->getPeach()->bonk();
        setDead();
        return;
    } else {
        overlapActor = overlap(x, y, this);
        if (overlapActor != nullptr && overlapActor->isDamagable()) {
            overlapActor->takeDamage();
            setDead();
            return;
        }
    }
    //falls if midair
    overlapActor = isObjectBlocking(x, y-2, this);
    if (overlapActor == nullptr) {
        moveTo(x, y-2);
        y = y - 2;
    }
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-2, y, this);
        if (overlapActor != nullptr) {
            setDead();
            return;
        }
        moveTo(x-2, y);
    }
    else {
        overlapActor = isObjectBlocking(x+2, y, this);
        if (overlapActor != nullptr) { //if running into a blocking object, set to dead
            setDead();
            return;
        }
        moveTo(x+2, y);
    }
}



PeachFireball::PeachFireball(StudentWorld* world, int startX, int startY, int dir) : Projectile(world, IID_PEACH_FIRE, startX, startY, dir) {}

void PeachFireball::doSomething() {
    int x = getX();
    int y = getY();
    //overlap
    Actor* overlapActor = overlap(x, y, this);
    if (!peachOverlap(x,y) && overlapActor != nullptr && overlapActor->isDamagable()) {
        overlapActor->takeDamage();
        setDead();
        return;
    }
    //falling
    overlapActor = isObjectBlocking(x, y-2, this);
    if (overlapActor == nullptr) {
        moveTo(x, y-2);
        y = y - 2;
    }
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-2, y, this);
        if (overlapActor != nullptr) {
            setDead();
            return;
        }
        moveTo(x-2, y);
    }
    else {
        overlapActor = isObjectBlocking(x+2, y, this);
        if (overlapActor != nullptr) {  //if running into a blocking object, set to dead
            setDead();
            return;
        }
        moveTo(x+2, y);
    }
}



PiranhaFireball::PiranhaFireball(StudentWorld* world, int startX, int startY, int dir) : Projectile(world, IID_PIRANHA_FIRE, startX, startY, dir) {}



Shell::Shell(StudentWorld* world, int startX, int startY, int dir) : Projectile(world, IID_SHELL, startX, startY, dir) {}



Mario::Mario(StudentWorld* world, int startX, int startY) : Actor(IID_MARIO, startX, startY, world, 0, 1, 1) {}

void Mario::doSomething() {
    if (isAlive()) {
        if(peachOverlap(getX(), getY())) { //if overlapping peach, game over
            getWorld()->increaseScore(1000);
            setDead();
            getWorld()->endGame();
        }
    }
}



Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY) : Actor(imageID, startX, startY, world, 0, 1, 1) {}



Flower::Flower(StudentWorld* world, int startX, int startY) : Goodie(world, IID_FLOWER, startX, startY) {}

void Flower::doSomething() {
    int x = getX();
    int y = getY();
    if (peachOverlap(x, y)) { //if overlapping peach
        getWorld()->increaseScore(50);
        getWorld()->getPeach()->giveShootPower();
        getWorld()->getPeach()->updateHealth(2);
        setDead();
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }
    
    //falling
    Actor* overlapActor = isObjectBlocking(x, y-2, this);
    if (overlapActor == nullptr) {
        moveTo(x, y-2);
        y = y - 2;
    }
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-2, y, this);
        if (overlapActor != nullptr) { //if running into blocking object, change direction
            setDirection(right);
            return;
        }
        moveTo(x-2, y);
    }
    else {
        overlapActor = isObjectBlocking(x+2, y, this);
        if (overlapActor != nullptr) { //if running into blocking object, change direction
            setDirection(left);
            return;
        }
        moveTo(x+2, y);
    }
}



Mushroom::Mushroom(StudentWorld* world, int startX, int startY) : Goodie(world, IID_MUSHROOM, startX, startY) {}

void Mushroom::doSomething() {
    int x = getX();
    int y = getY();
    if (peachOverlap(x, y)) { //if overlapping peach
        getWorld()->increaseScore(75);
        getWorld()->getPeach()->giveJumpPower();
        getWorld()->getPeach()->updateHealth(2);
        setDead();
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }
    
    //falling
    Actor* overlapActor = isObjectBlocking(x, y-2, this);
    if (overlapActor == nullptr) {
        moveTo(x, y-2);
        y = y - 2;
    }
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-2, y, this);
        if (overlapActor != nullptr) { //if running into blocking object, change direction
            setDirection(right);
            return;
        }
        moveTo(x-2, y);
    }
    else {
        overlapActor = isObjectBlocking(x+2, y, this);
        if (overlapActor != nullptr) { //if running into blocking object, change direction
            setDirection(left);
            return;
        }
        moveTo(x+2, y);
    }
}



Star::Star(StudentWorld* world, int startX, int startY) : Goodie(world, IID_STAR, startX, startY) {}

void Star::doSomething() {
    int x = getX();
    int y = getY();
    if (peachOverlap(x, y)) { //if overlapping peach
        getWorld()->increaseScore(100);
        getWorld()->getPeach()->giveStarPower(150);
        getWorld()->getPeach()->updateHealth(2);
        setDead();
        getWorld()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }
    
    //falling
    Actor* overlapActor = isObjectBlocking(x, y-2, this);
    if (overlapActor == nullptr) {
        moveTo(x, y-2);
        y = y - 2;
    }
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-2, y, this);
        if (overlapActor != nullptr) {
            setDirection(right);
            return;
        }
        moveTo(x-2, y);
    }
    else {
        overlapActor = isObjectBlocking(x+2, y, this);
        if (overlapActor != nullptr) {
            setDirection(left);
            return;
        }
        moveTo(x+2, y);
    }
}



Enemy::Enemy(StudentWorld* world, int imageID, int startX, int startY) : Actor(imageID, startX, startY, world, randInt(0, 100)*180, 0, 1, 1) {}

void Enemy::takeDamage() {
    getWorld()->increaseScore(100);
    setDead();
}

void Enemy::doSomething() {
    if (!isAlive()) {
        return;
    }
    int x = getX();
    int y = getY();
    if (peachOverlap(x, y)) {
        getWorld()->getPeach()->bonk();
        return;
    }
    
    Actor* overlapActor;
    
    //if unable to move in dierection, switch direction
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-1, y, this);
    }
    else {
        overlapActor = isObjectBlocking(x+1, y, this);
    }
    if (overlapActor != nullptr) {
        setDirection(getDirection()+180);
        return;
    }
    
    //directional movement
    if (getDirection() == left) {
        overlapActor = isObjectBlocking(x-SPRITE_WIDTH, y-1, this);
        if (overlapActor != nullptr) {
            overlapActor = isObjectBlocking(x-1, y, this);
            if (overlapActor != nullptr) {
                return;
            }
            moveTo(x-1, y);
        }
        else {
            setDirection(right);
        }
    }
    else {
        overlapActor = isObjectBlocking(x+SPRITE_WIDTH, y-1, this);
        if (overlapActor != nullptr) {
            overlapActor = isObjectBlocking(x+1, y, this);
            if (overlapActor != nullptr) {
                return;
            }
            moveTo(x+1, y);
        }
        else {
            setDirection(left);
        }
    }
}

void Enemy::bonk(bool isInvPeach) {
    if (isInvPeach) { //if bonked by invincible peach
        getWorld()->playSound(SOUND_PLAYER_KICK);
        getWorld()->increaseScore(100);
        setDead();
    }
}



Goomba::Goomba(StudentWorld* world, int startX, int startY) : Enemy(world, IID_GOOMBA, startX, startY) {}



Koopa::Koopa(StudentWorld* world, int startX, int startY) : Enemy(world, IID_KOOPA, startX, startY) {}

void Koopa::bonk(bool isInvPeach) {
    if (isInvPeach) {
        getWorld()->playSound(SOUND_PLAYER_KICK);
        takeDamage();
    }
}

void Koopa::takeDamage() {
    getWorld()->increaseScore(100);
    setDead();
    getWorld()->addActor(new Shell(getWorld(), getX(), getY(), getDirection()));
}



Piranha::Piranha(StudentWorld* world, int startX, int startY) : Enemy(world, IID_PIRANHA, startX, startY), m_firingDelay(0) {}

void Piranha::doSomething() {
    if (!isAlive()) {
        return;
    }
    
    increaseAnimationNumber();
    
    int x = getX();
    int y = getY();
    if(peachOverlap(x, y)) {
        getWorld()->getPeach()->bonk();
        return;
    }
    
    int peachY = getWorld()->getPeach()->getY();
    if (y+1.5*SPRITE_HEIGHT < peachY || y-1.5*SPRITE_HEIGHT > peachY) {
        return;
    }
    
    int peachX = getWorld()->getPeach()->getX();
    if (x > peachX) {
        setDirection(left);
    }
    else {
        setDirection(right);
    }
    
    if (m_firingDelay > 0) {
        m_firingDelay--;
        return;
    }
    else {
        if (x > peachX) {
            if (x - peachX < 8*SPRITE_WIDTH) {
                getWorld()->addActor(new PiranhaFireball(getWorld(), x-8, y, left));
            }
        }
        else {
            if (peachX - x < 8*SPRITE_WIDTH) {
                getWorld()->addActor(new PiranhaFireball(getWorld(), x+8, y, right));
            }
        }
        getWorld()->playSound(SOUND_PIRANHA_FIRE);
        m_firingDelay = 40;
    }
}

