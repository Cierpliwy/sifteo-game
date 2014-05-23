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
#include "../assets.gen.h"

using namespace Sifteo;

void Entity::init() {
    m_direction = BOTTOM;
    m_healthPoints = 4;
    m_attack = 4;
    m_defence = 2;
    m_animation = IDLE;
    m_state = NONE;
    m_frozenTime = 0;
    m_frozenFrame = 0;
    m_visible = false;
    m_mapPosition = vec<char>(0,0);
    m_asset = &Zombie;
}

void Entity::update(Map &map) {
    if (m_animation == MOVE) {
        auto nextMapPos = nextMapPosition();
        auto entityType = map.at(m_mapPosition.x, m_mapPosition.y).entity;
        map.at(m_mapPosition.x, m_mapPosition.y).entity = Tile::NONE;
        map.at(m_mapPosition.x, m_mapPosition.y).state &= ~Tile::BLOCKABLE;
        map.at(nextMapPos.x, nextMapPos.y).entity = entityType;
        map.at(nextMapPos.x, nextMapPos.y).state |= Tile::BLOCKABLE;
        map.at(nextMapPos.x, nextMapPos.y).id = 
            map.at(m_mapPosition.x, m_mapPosition.y).id;
        map.at(m_mapPosition.x, m_mapPosition.y).id = 0;
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
