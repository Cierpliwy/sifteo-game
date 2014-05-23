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

#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Map.h"
class Player;
class AudioPlayer;

// Enemy structure
class Enemy : public Entity {
public:
    void update(Map &map, AudioPlayer& audio, Player &player, Enemy enemies[],
                Sifteo::Random &rnd);
    static const char enemiesCount = 32;
};

#endif //ENEMY_H
