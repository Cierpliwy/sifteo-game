/*****************************************************************************
 * Copyright (c) 2014 Przemysław Lenart <przemek.lenart@gmail.com>
 * 
 * This file is part of Cube Crawler.
 *
 * Cube Crawler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cube Crawler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cube Crawler.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "CubeData.h"
#include "Player.h"
#include "Enemy.h"
#include "AudioPlayer.h"

class Game
{
public:
    void init();
    void run();
    void cleanup();
    bool finished() { return false; }

    void create();

private:

    // Game map containing information about it's state
    Map map;    

    // Cube specific data
    CubeData cube[CubeData::cubeCount];
    char playerCube;
    char minimapCube;

    // Player
    class Player player;

    // Enemies
    Enemy enemies[Enemy::enemiesCount];

    // Audio class
    AudioPlayer audioPlayer;

    // Detecting frame time
    Sifteo::TimeStep ts;
    unsigned frame;
    unsigned movementFrame;
    float time;

    // Random number generator
    Sifteo::Random rnd;
};

#endif //GAME_H
