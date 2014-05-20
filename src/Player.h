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

#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Map.h"

// Player structure
class Player : public Entity {
public:
    Player() : m_experience(0),
               m_manaPoints(10),
               m_level(0) {}

    void update(Map &map, const CubeData &controller, const Enemy enemies[]);

    char getExperience() const {
        return m_experience;
    }

    void setExperience(char experience) {
        m_experience = experience;
    }

    char getManaPoints() const {
        return m_manaPoints;
    }

    void setManaPoints(char manaPoints) {
        m_manaPoints = manaPoints;
    }

    char getLevel() const {
        return m_level;
    }

    void setLevel(char level) {
        m_level = level;
    }

private:
    char m_experience;
    char m_manaPoints;
    char m_level;
    
};

#endif //PLAYER_H
