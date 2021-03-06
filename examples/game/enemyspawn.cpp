#include "enemyspawn.h"
#include "game.h"
#include "gameobject.h"
#include <renderer.h>

EnemySpawn::EnemySpawn(Game * g) : m_game(g)
{
    setInterval(1000);
}

void EnemySpawn::perform()
{
    if (m_game->isPaused() == false)
    {
        // Compute position for new object
        double padding  = 12; // A small paddings out of scren borders
        double xmin = padding,  ymin = padding; 
        double xmax = m_game->renderer()->settings().width() - padding;
        double ymax = m_game->renderer()->settings().height() - padding;
        double x, y;
        x=(static_cast<double>(rand())/RAND_MAX)*(xmax - xmin) + xmin; 
        y=(static_cast<double>(rand())/RAND_MAX)*(ymax - ymin) + ymin; 

        double vx=static_cast<float>(rand())/RAND_MAX * 250 - 150;
        double vy=static_cast<float>(rand())/RAND_MAX * 250 - 150;
 
        // Produce object, set new position and speed
        GameObject * o = m_game->produce( static_cast<Objects>(rand() % 4) );
        if (o)
        {
            o->setPosition(sad::p2d::Point(x, y));
            o->setHorizontalSpeed(vx);
            o->setVerticalSpeed(vy);
        }
    }
}
