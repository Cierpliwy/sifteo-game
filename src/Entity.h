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

#ifndef ENTITY_H
#define ENTITY_H

#include <sifteo.h>
class Map;

// Entity
class Entity {
public:

    enum Animation {
        IDLE = 0,
        ATTACK = 1,
        MOVE = 2,
        HIT = 4
    };

    enum State {
        NONE = 0,
        DAMAGE = 1,
        FROZEN = 2
    };

    Entity() {init();}

    void init();

    void update(Map &map);
    Sifteo::Vector2<char> nextMapPosition() const;


    char getDirection() const {
        return m_direction;
    }

    void setDirection(char direction) {
        m_direction = direction;
    }

    char getHealthPoints() const {
        return m_healthPoints;
    }

    void setHealthPoints(char healthPoints) {
        m_healthPoints = healthPoints;
    }

    char getAttack() const {
        return m_attack;
    }

    void setAttack(char attack) {
        m_attack = attack;
    }

    char getDefence() const {
       return m_defence;
    } 

    void setDefence(char defence) {
        m_defence = defence;
    }

    char getAnimation() const {
        return m_animation;
    }

    void setAnimation(char animation) {
        m_animation = animation;
    }

    Sifteo::Vector2<char> getMapPosition() const {
        return m_mapPosition;
    }

    void setMapPosition(Sifteo::Vector2<char> mapPosition) {
        m_mapPosition = mapPosition;
    }

    char getState() const {
        return m_state;
    }

    void setState(char state) {
        m_state = state;
    }

    const Sifteo::PinnedAssetImage* getAsset() const {
        return m_asset;
    }

    void setAsset(const Sifteo::PinnedAssetImage &img) {
        m_asset = &img;
    }

    void setFrozenTime(char time) {
        m_frozenTime = time;
    }

    char getFrozenTime() const {
        return m_frozenTime;
    }

    void setFrozenFrame(unsigned frame) {
        m_frozenFrame = frame;
    }

    unsigned getFrozenFrame() const {
        return m_frozenFrame;
    }

    void setIsVisible(bool visible) {
        m_visible = visible;
    }

    bool isVisible() const {
        return m_visible;
    }

protected:

    char m_direction;
    char m_healthPoints;
    char m_attack;
    char m_defence;
    char m_animation;
    char m_state;
    char m_frozenTime;
    unsigned m_frozenFrame;
    bool m_visible;
    Sifteo::Vector2<char> m_mapPosition;
    const Sifteo::PinnedAssetImage *m_asset;
};

#endif //ENTITY_H
