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

#include "Player.h"
#include "CubeData.h"
using namespace Sifteo;

void Player::update(Map &map, const CubeData &controller, const Enemy enemies[]) {
    Entity::update(map); 

    if (m_animation == IDLE) {
        auto tilt = controller.getMotion().tilt;
        Vector2<char> newPos = vec<char>(m_mapPosition.x + tilt.x, 
                                         m_mapPosition.y + tilt.y);

        if (((tilt.x == 0 && tilt.y != 0) ||
             (tilt.y == 0 && tilt.x != 0))) {
            if (tilt.x < 0) m_direction = LEFT;
            if (tilt.x > 0) m_direction = RIGHT;
            if (tilt.y > 0) m_direction = BOTTOM;
            if (tilt.y < 0) m_direction = TOP;
            if (!map.isTileBlockable(newPos.x, newPos.y, *this, enemies)) {
                m_animation = MOVE;
            }
        }
    }
}
