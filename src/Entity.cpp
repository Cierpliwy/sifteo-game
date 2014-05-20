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

#include "Entity.h"
#include "Map.h"

using namespace Sifteo;

void Entity::update(Map &map) {
    if (m_animation == MOVE) {
        auto nextMapPos = nextMapPosition();
        auto entityType = map.at(m_mapPosition.x, m_mapPosition.y).entity;
        map.at(m_mapPosition.x, m_mapPosition.y).entity = Tile::NONE;
        map.at(nextMapPos.x, nextMapPos.y).entity = entityType;
        m_mapPosition = nextMapPos;
        m_animation = IDLE;
    } 
}

Vector2<char> Entity::nextMapPosition() const {
    Vector2<char> newPos = m_mapPosition;
    switch(m_direction) {
        case TOP:
            newPos.y--;
            break;
        case BOTTOM:
            newPos.y++;
            break;
        case LEFT:
            newPos.x--;
            break;
        case RIGHT:
            newPos.x++;
            break;
    }
    return newPos;
}
