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
#include "Enemy.h"
#include "CubeData.h"
#include "AudioPlayer.h"
using namespace Sifteo;

void Player::init() {
    Entity::init();
    m_experience = 0;
    m_manaPoints = 10;
    m_healthPoints = 10;
    m_level = 0;
    m_keys = 0;
    m_attack = 3;
    m_defence = 2;
}

void Player::update(Map &map, AudioPlayer &audio, const CubeData &controller, 
                    Enemy enemies[], unsigned frame) {
    if (m_healthPoints <= 0) return;
    Entity::update(map); 

    // Get updates
    if (map.at(m_mapPosition.x, m_mapPosition.y).type == Tile::HP) {
        m_healthPoints += 1;
        if (m_healthPoints > 10) m_healthPoints = 10;
        map.at(m_mapPosition.x, m_mapPosition.y).type = Tile::FLOOR;
        audio.playSound(AudioPlayer::ORB);
    }
    if (map.at(m_mapPosition.x, m_mapPosition.y).type == Tile::MP) {
        m_manaPoints += 2;
        if (m_manaPoints > 10) m_manaPoints = 10;
        map.at(m_mapPosition.x, m_mapPosition.y).type = Tile::FLOOR;
        audio.playSound(AudioPlayer::ORB);
    }
    if (map.at(m_mapPosition.x, m_mapPosition.y).type == Tile::EXP) {
        increaseExperience(1, audio);
        audio.playSound(AudioPlayer::ORB);
        map.at(m_mapPosition.x, m_mapPosition.y).type = Tile::FLOOR;
    }
    if (map.at(m_mapPosition.x, m_mapPosition.y).type == Tile::KEY) {
        m_keys++;
        audio.playSound(AudioPlayer::KEY);
        map.at(m_mapPosition.x, m_mapPosition.y).type = Tile::FLOOR;
    }
    if (map.at(nextMapPosition().x, nextMapPosition().y).type == Tile::CHEST) {
        char anim = map.at(nextMapPosition().x, nextMapPosition().y).state & 0x3;
        if (anim != 3) {
            anim++;
            audio.playSound(AudioPlayer::ORB);
            increaseExperience(1, audio);
        }
        map.at(nextMapPosition().x, nextMapPosition().y).state = anim | Tile::BLOCKABLE;
    }

    // Skill usage
    if (controller.getMotion().shake && m_manaPoints >= 5) {
        m_manaPoints -= 5;
        for(int i = 0; i < Enemy::enemiesCount; ++i) {
            if (enemies[i].isVisible()) {
                enemies[i].setFrozenTime(20);
                enemies[i].setFrozenFrame(frame);
            }
        }
    }

    // New movement
    if (m_animation != MOVE) {
        auto tilt = controller.getMotion().tilt;
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
        map.at(m_mapPosition.x-1, m_mapPosition.y).entity == Tile::ENEMY &&
        enemies[(int)map.at(m_mapPosition.x-1, m_mapPosition.y).id].getDirection() ==
        RIGHT) m_state = DAMAGE;
    if (m_mapPosition.x < Map::mapSize && 
        map.at(m_mapPosition.x+1, m_mapPosition.y).entity == Tile::ENEMY &&
        enemies[(int)map.at(m_mapPosition.x+1, m_mapPosition.y).id].getDirection() ==
        LEFT) m_state = DAMAGE;
    if (m_mapPosition.y > 0 && 
        map.at(m_mapPosition.x, m_mapPosition.y-1).entity == Tile::ENEMY &&
        enemies[(int)map.at(m_mapPosition.x-1, m_mapPosition.y).id].getDirection() ==
        BOTTOM) m_state = DAMAGE;
    if (m_mapPosition.y < Map::mapSize && 
        map.at(m_mapPosition.x, m_mapPosition.y+1).entity == Tile::ENEMY &&
        enemies[(int)map.at(m_mapPosition.x, m_mapPosition.y+1).id].getDirection() ==
        TOP) m_state = DAMAGE;

    // Update animation
    if (m_animation != MOVE) {
        if (map.at(nextMapPosition().x, nextMapPosition().y).entity == Tile::ENEMY) {
            int id = map.at(nextMapPosition().x, nextMapPosition().y).id;
            int damage = MAX(m_attack - enemies[id].getDefence(), 0);
            if (enemies[id].getHealthPoints() - damage <= 0) {
                // Enemy dies
                increaseExperience(2, audio);
                enemies[id].setHealthPoints(0);
                map.at(nextMapPosition().x, nextMapPosition().y).entity = Tile::NONE;
                map.at(nextMapPosition().x, nextMapPosition().y).type = Tile::BLOODED_FLOOR;
                map.at(nextMapPosition().x, nextMapPosition().y).state &= ~Tile::BLOCKABLE;
                if (enemies[id].getAnimation() == MOVE) {
                    map.at(enemies[id].nextMapPosition().x,
                           enemies[id].nextMapPosition().y).state &= ~Tile::BLOCKABLE;
                }

            } else {
                enemies[id].setHealthPoints(enemies[id].getHealthPoints() - damage);
            }
            m_animation = ATTACK;
            audio.playSound(AudioPlayer::HIT);
        }
        else
            m_animation = IDLE;
    }
}

void Player::increaseExperience(int exp, AudioPlayer &audio) {
        m_experience += exp;
        if (m_experience > 10) {
            m_experience = 0;
            m_level++;
            m_attack++;
            m_defence++;
            audio.playSound(AudioPlayer::LEVEL);
        }
}
