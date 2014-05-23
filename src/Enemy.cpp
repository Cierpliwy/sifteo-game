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

#include "Enemy.h"
#include "Player.h"
#include "AudioPlayer.h"
using namespace Sifteo;

static int sgn(int x) {
    if (x < 0) return -1;
    if (x > 0) return 1;
    return 0;
}

void Enemy::update(Map &map, AudioPlayer &audio, Player &player, 
                   Enemy enemies[], Random &rnd) {
    if (m_healthPoints <= 0) return;

    // Frozen enemies don't move
    m_frozenTime = MAX(m_frozenTime - 1, 0);
    if (m_frozenTime > 0) m_state |= FROZEN;
    else m_state &= ~FROZEN;
    if (m_state & FROZEN) return;

    Entity::update(map);

    // New movement
    if (m_animation != MOVE) {

        Byte3 tilt;
        if (rnd.raw() % 3 == 0) {
            // Move randomly
            tilt = vec<int8_t>(rnd.raw() % 3 - 1, rnd.raw() % 3 - 1, 0);
        } else {
            // Move into players direction
            tilt.set(player.getMapPosition().x - m_mapPosition.x,
                     player.getMapPosition().y - m_mapPosition.y, 0);
            tilt.set(sgn(tilt.x), sgn(tilt.y), 0);

            // If need double direction, choose one
            if (tilt.x != 0 && tilt.y != 0) {
                if (rnd.raw() % 2 == 0) tilt.x = 0;
                else tilt.y = 0;
            }
        }

        //Move
        Vector2<char> newPos = vec<char>(m_mapPosition.x + tilt.x, 
                                         m_mapPosition.y + tilt.y);
        if (((tilt.x == 0 && tilt.y != 0) ||
             (tilt.y == 0 && tilt.x != 0))) {
            if (tilt.x < 0) m_direction = LEFT;
            if (tilt.x > 0) m_direction = RIGHT;
            if (tilt.y > 0) m_direction = BOTTOM;
            if (tilt.y < 0) m_direction = TOP;
            if (!map.isTileBlockable(newPos.x, newPos.y)) {
                m_animation = MOVE;
                audio.playSound(AudioPlayer::STEP);
                map.at(newPos.x, newPos.y).state |= Tile::BLOCKABLE;
            }
        }
    }

    // Update state
    m_state = 0;
    if (m_mapPosition.x > 0 && 
        map.at(m_mapPosition.x-1, m_mapPosition.y).entity == Tile::PLAYER &&
        player.getDirection() == RIGHT) m_state = DAMAGE;
    if (m_mapPosition.x < Map::mapSize && 
        map.at(m_mapPosition.x+1, m_mapPosition.y).entity == Tile::PLAYER &&
        player.getDirection() == LEFT) m_state = DAMAGE;
    if (m_mapPosition.y > 0 && 
        map.at(m_mapPosition.x, m_mapPosition.y-1).entity == Tile::PLAYER &&
        player.getDirection() == BOTTOM) m_state = DAMAGE;
    if (m_mapPosition.y < Map::mapSize && 
        map.at(m_mapPosition.x, m_mapPosition.y+1).entity == Tile::PLAYER &&
        player.getDirection() == TOP) m_state = DAMAGE;

    if (player.getHealthPoints() == 0) return;

    // Update animation
    if (m_animation != MOVE) {
        if (map.at(nextMapPosition().x, nextMapPosition().y).entity == Tile::PLAYER) {
            m_animation = ATTACK;
            audio.playSound(AudioPlayer::HIT);
            int damage = MAX(m_attack - player.getDefence(), 0);
            if (player.getHealthPoints() - damage <= 0) {
                player.setHealthPoints(0);
                map.at(player.getMapPosition().x, player.getMapPosition().y).entity = Tile::NONE;
                map.at(player.getMapPosition().x, player.getMapPosition().y).type = Tile::BLOODED_FLOOR;
                map.at(player.getMapPosition().x, player.getMapPosition().y).state &= ~Tile::BLOCKABLE;
            } else {
                player.setHealthPoints(player.getHealthPoints() - damage);
            }
        }
        else
            m_animation = IDLE;
    }
}
