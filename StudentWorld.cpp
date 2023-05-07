#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}

int StudentWorld::init()
{
    int level = getLevel();    //get level, format level file name correctly
    string lvlStr;
    if (level < 10) {
        lvlStr = "0" + to_string(level);
    }
    else {
        lvlStr = to_string(level);
    }
    
    Level lev(assetPath());
    string levFile = "level" + lvlStr + ".txt";
    if(lev.loadLevel(levFile) == Level::load_fail_file_not_found) {
        return GWSTATUS_LEVEL_ERROR;
    }
    
    //for each spot in the grid, if there is supposed to be an actor, add it
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (lev.getContentsOf(i, j) == Level::peach) {
                m_peach = new Peach(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(m_peach);
            }
            
            if (lev.getContentsOf(i, j) == Level::block) {
                Block* act = new Block(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::star_goodie_block) {
                Block* act = new Block(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, IID_STAR);
                m_actors.push_back(act);
            }

            if (lev.getContentsOf(i, j) == Level::flower_goodie_block) {
                Block* act = new Block(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, IID_FLOWER);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::mushroom_goodie_block) {
                Block* act = new Block(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT, IID_MUSHROOM);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::flag) {
                Flag* act = new Flag(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::mario) {
                Mario* act = new Mario(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::pipe) {
                Pipe* act = new Pipe(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::goomba) {
                Goomba* act = new Goomba(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::koopa) {
                Koopa* act = new Koopa(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
            if (lev.getContentsOf(i, j) == Level::piranha) {
                Piranha* act = new Piranha(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT);
                m_actors.push_back(act);
            }
            
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_peach->doSomething();
    
    if (m_peach->isAlive() == false) {  //if dead, end game
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    

    if (m_peach->completedLevel() == true) { //if finished level, go to next
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    if (m_gameOver) { //if game over, end game with win
        playSound(SOUND_GAME_OVER);
        return GWSTATUS_PLAYER_WON;
    }
    
    int size = m_actors.size();
    list<Actor*>::iterator listIter = m_actors.begin();
    for (int i = 0; i < size; i++) {       //otherwise, for each Actor in our list, call doSomething
        if ((*listIter) != m_peach && (*listIter)->isAlive()) {
            (*listIter)->doSomething();
        }
        listIter++;
    }
    
    //remove dead actors (other than peach)
    listIter = m_actors.begin();
    for (int i = 0; i < size; i++) {
        if ((*listIter) != m_peach && (*listIter)->isAlive() == false) {
            delete *listIter;
            auto del = listIter;
            listIter++;
            m_actors.erase(del);
            size--;
        }
        else {
            listIter++;
        }
    }
    
    //display text
    string s = "Lives: " + to_string(getLives()) + "  Level: " + to_string(getLevel()) + "  Points: " + to_string(getScore());
    if (m_peach->isInvincible()) {
        s = s + " StarPower!";
    }
    if (m_peach->hasShootPower()) {
        s = s + " ShootPower!";
    }
    if (m_peach->hasJumpPower()) {
        s = s + " JumpPower!";
    }
    setGameStatText(s);
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    int size = m_actors.size();
    list<Actor*>::iterator listIter = m_actors.begin();
    for (int i = 0; i < size; i++) {  //for each Actor in our list, delete it
            delete *listIter;
            auto del = listIter;
            listIter++;
            m_actors.erase(del);
    }
}

Peach* StudentWorld::getPeach() {
    return m_peach;
}

std::list<Actor*> StudentWorld::getActors() {
    return m_actors;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_back(actor);
}
