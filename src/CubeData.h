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

#ifndef CUBE_DATA_H
#define CUBE_DATA_H

#include <sifteo.h>
class Map;
class Entity;
class Enemy;
class Player;

class CubeData
{
public:

    void init(char id);
    void update();

    Sifteo::Vector2<char> getMapPosition() const {
        return m_mapPosition;
    }

    void setMapPosition(Sifteo::Vector2<char> mapPosition) {
        m_mapPosition = mapPosition;
    }

    bool isUsed() const {
        return m_isUsed;
    }

    void used(bool isUsed) {
        m_isUsed = isUsed;
    }

    bool isInMap() const;

    bool inMapRange(const Map &map);
    bool connectedWithCube(int cubeID, const Map &map);
    void detectNeighborhood(CubeData cubes[]);

    void drawMap(const Map &map, unsigned frame, const Player &player,
                 const Enemy enemies[]);
    void drawMiniMap(const Map &map, const CubeData cubes[], 
                     const Player &player);
    void drawStars(float time);
    void drawFog(float time);

    void drawEntity(const Entity &entity, unsigned frame);
    void addEntityMask(const Entity &entity, Sifteo::BG1Mask &mask);

    const Sifteo::TiltShakeRecognizer& getMotion() const {
        return m_motion;
    }

    static const char cubeCount = 6;

private:

    void checkNeighbors(CubeData *cube1, CubeData *cube2, CubeData cubes[],
                        int fromSide, char x, char y);

    void drawStaticTile(const Map &map, Sifteo::Vector2<int> scr,
                        Sifteo::Vector2<int> pos, int tileNr);

    void drawAnimatedTile(const Map &map, unsigned frame,
                          Sifteo::Vector2<int> scr, int tileNr);

    void drawBar(int y, int startTile, int value);
    void drawSkill(int skill);

    bool inCube(Sifteo::Vector2<char> pos);

    char m_id;
    Sifteo::VideoBuffer m_video;
    Sifteo::Side m_rotation;
    Sifteo::Vector2<char> m_mapPosition;
    Sifteo::TiltShakeRecognizer m_motion;
    bool m_isUsed;
};

#endif //CUBE_DATA_H
