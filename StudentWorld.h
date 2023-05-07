#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h" //REMOVE if possible
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    ~StudentWorld();
    std::list<Actor*> getActors();
    Peach* getPeach();
    void addActor(Actor* actor);
    void updateScore(int add);
    void endGame() {m_gameOver = true;}

private:
    std::list<Actor*> m_actors;
    Peach* m_peach;
    int m_score = 0;
    
    bool m_gameOver = false;
};

#endif // STUDENTWORLD_H_
