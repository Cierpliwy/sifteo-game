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

#include "CubeData.h"
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "../assets.gen.h"

using namespace Sifteo;

void CubeData::init(char id) {
    m_video.attach(id);
    m_motion.attach(id);
    m_rotation = TOP;
    m_isUsed = false;
    m_id = id;
    m_mapPosition.x = 0;
    m_mapPosition.y = 0;
}

void CubeData::update() {
    m_motion.update();
}

bool CubeData::isInMap() const {
    if (m_mapPosition.x < 0) return false;
    if (m_mapPosition.x >= Map::cubesPerMap) return false;
    if (m_mapPosition.y < 0) return false;
    if (m_mapPosition.y >= Map::cubesPerMap) return false;
    return true;
}

void CubeData::detectNeighborhood(CubeData cubes[])
{
    // Cubes are not used by default
    for(int i = 0; i < CubeData::cubeCount; ++i) {
        cubes[i].used(false);
    }

    // Start detecting neighborhood from this cube
    checkNeighbors(NULL, this, cubes, 0, m_mapPosition.x, m_mapPosition.y);
}
    
void CubeData::checkNeighbors(CubeData *cube1, CubeData *cube2, 
                              CubeData cubes[], int fromSide, char x, char y)
{
    // If cube was already processed, let's finish.
    if (cube2->m_isUsed) return;

    // If function was called by other cube find it's position and calibrate
    // cube rotation.
    auto n = cube2->m_video.physicalNeighbors();
    if (cube1) {
        // Find caller cube
        int i;
        for(i = TOP; i < NUM_SIDES; ++i) {
            CubeID id = n.cubeAt((Side)i);
            if (id == cube1->m_id) break;
        }

        // Correct your cube rotation
        cube2->m_rotation = (Side)((NUM_SIDES + i - fromSide) % NUM_SIDES);
    }

    cube2->m_isUsed = true;
    cube2->m_mapPosition = vec(x, y);

    // Detect other cubes
    for(int j = TOP; j < NUM_SIDES; ++j) {
        CubeID id = n.cubeAt((Side)j);
        if (id != CubeID()) {
            int nposx = x;
            int nposy = y;

            int virt = (NUM_SIDES + j - cube2->m_rotation) % NUM_SIDES;
            if (virt == TOP) nposy--;
            if (virt == LEFT) nposx--;
            if (virt == BOTTOM) nposy++;
            if (virt == RIGHT) nposx++;
           
            checkNeighbors(cube2, &cubes[id], cubes, 
                           (virt + BOTTOM) % NUM_SIDES, nposx, nposy);
        }
    }
}

void CubeData::drawStaticTile(const Map &map, Vector2<int> scr,
                              Vector2<int> pos, int tileNr)
{
   m_video.bg0.image(scr, *map.getTheme(), tileNr + 
                         (map.at(pos.x, pos.y).state & 0x3) % 4);
}
void CubeData::drawAnimatedTile(const Map &map, unsigned frame,
                                Vector2<int> scr, int tileNr)
{
   m_video.bg0.image(scr, *map.getTheme(), tileNr + frame % 4);
}


void CubeData::drawMap(const Map &map, unsigned frame, 
                       const class Player& player, const Enemy enemies[]) {
    // Initialize mode specific for map drawing
    if (m_video.mode() != BG0_SPR_BG1) {
        m_video.initMode(BG0_SPR_BG1);
    }
    // Erase mask
    BG1Mask mask;
    mask.clear();
    // Set cube orientation
    m_video.setOrientation(m_rotation);
    // Get starting tile position
    Vector2<int> tpos;
    tpos.x = m_mapPosition.x * Map::tilesPerCube;
    tpos.y = m_mapPosition.y * Map::tilesPerCube;

    // Screen tile position
    Vector2<int> scr = vec(0,0);

    // For each tile
    for(int x = tpos.x; x < tpos.x + Map::tilesPerCube; ++x) {
        for(int y = tpos.y; y < tpos.y + Map::tilesPerCube; ++y) {
            switch(map.at(x,y).type) {
                case Tile::FLOOR:
                    drawStaticTile(map, scr, vec(x,y), 0);
                    break;
                case Tile::WALL:
                    drawStaticTile(map, scr, vec(x,y), 4);
                    break;
                case Tile::STAIRS:
                    drawStaticTile(map, scr, vec(x,y), 12);
                    break;
                case Tile::CHEST:
                    drawStaticTile(map, scr, vec(x,y), 8);
                    break;
                case Tile::MP:
                    drawAnimatedTile(map, frame, scr, 16);
                    break;
                case Tile::HP:
                    drawAnimatedTile(map, frame, scr, 20);
                    break;
                case Tile::EXP:
                    drawAnimatedTile(map, frame, scr, 24);
                    break;
                case Tile::KEY:
                    drawAnimatedTile(map, frame, scr, 28);
                    break;
                case Tile::OBSTACLE:
                    drawStaticTile(map, scr, vec(x,y), 36);
                    break;
                case Tile::BLOODED_FLOOR:
                    drawStaticTile(map, scr, vec(x,y), 32);
                    break;
                default:
                    drawStaticTile(map, scr, vec(x,y), 0);
            }
            scr.y += 2;
        }
        scr.x += 2;
        scr.y = 0;
    }

    // Add player and enemies mask
    addEntityMask(player, mask);
    for(int i = 0; i < Enemy::enemiesCount; ++i)
        addEntityMask(enemies[i], mask);

    // Setup mask
    m_video.bg1.setMask(mask);

    // Draw entities on a mask
    drawEntity(player, frame);
    for(int i = 0; i < Enemy::enemiesCount; ++i)
        drawEntity(enemies[i], frame);
     
}

void CubeData::drawStars(float time) {
    if (m_video.mode() != BG0) {
        m_video.initMode(BG0);
    }
    m_video.setOrientation(m_rotation);
    m_video.bg0.image(vec(0,0), Stars, 0);
    m_video.bg0.setPanning(vec((int)(time*8),(int)(time*8)));
}

void CubeData::drawFog(float time) {
    if (m_video.mode() != BG0) {
        m_video.initMode(BG0);
    }
    m_video.setOrientation(m_rotation);
    m_video.bg0.image(vec(0,0), Fog, 0);
    m_video.bg0.setPanning(vec((int)(time*30), 0));
}

void CubeData::drawBar(int y, int startTile, int value) {
    m_video.bg0.image(vec(1, y), Minimap, startTile);
    m_video.bg0.image(vec(2, y), Minimap, startTile + 1);
    m_video.bg0.image(vec(3, y), Minimap, 41);
    for(int i = 0; i < 10; ++i) {
        if (i == 0) {
            if (value <= i)
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 2);
            else 
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 3);
        } else if (i == 9) {
            if (value <= i)
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 6);
            else 
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 7);
        } else {
            if (value <= i)
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 4);
            else 
                m_video.bg0.image(vec(5+i, y), Minimap, startTile + 5);
        }
    }
}

void CubeData::drawSkill(int skill) {
    for(int i = 0; i < 16; ++i)
        m_video.bg0.image(vec(1 + i % 4, 11 + (i/4)), Minimap, 84 + i);
}

void CubeData::drawMiniMap(const Map &map, const CubeData cubes[], 
                           const class Player &player)
{
    // Init minimap mode
    if (m_video.mode() != BG0_BG1) {
        m_video.initMode(BG0_BG1);
    }
    m_video.setOrientation(m_rotation);

    // Clear map
    for (int x = 0; x < 16; ++x)
        for(int y = 0; y < 16; ++y)
            m_video.bg0.image(vec(x,y), Minimap, 0);

    // Minimap offset:
    int mX = 7, mY = 7;

    // Draw minimap border
    for(int x = mX; x < mX + 8; x++) {
        m_video.bg0.image(vec(x, mY - 1), Minimap, 37);
        m_video.bg0.image(vec(x, mY + 8), Minimap, 36);
    }
    for(int y = mY; y < mY + 8; y++) {
        m_video.bg0.image(vec(mX - 1, y), Minimap, 38);
        m_video.bg0.image(vec(mX + 8, y), Minimap, 39);
    }
    m_video.bg0.image(vec(mX - 1, mY - 1), Minimap, 32);
    m_video.bg0.image(vec(mX + 8, mY - 1), Minimap, 33);
    m_video.bg0.image(vec(mX - 1, mY + 8), Minimap, 34);
    m_video.bg0.image(vec(mX + 8, mY + 8), Minimap, 35);

    // Draw minimap content
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (!(map.atCube(x,y) & NO_FOG)) {
                m_video.bg0.image(vec(mX + x, mY + y), Minimap, 40);
                continue;
            }
            char info = map.atCube(x,y);
            int s;
            for(s = 0; s < CubeData::cubeCount; ++s)
                if (cubes[s].m_isUsed && 
                    cubes[s].m_mapPosition.x == x &&
                    cubes[s].m_mapPosition.y == y) break;
            info &= 0xF;
            if (s != CubeData::cubeCount)
                info += 16;
            m_video.bg0.image(vec(mX + x, mY + y), Minimap, info);
        }
    }

    // Draw HP bar
    drawBar(1, 44, player.getHealthPoints());
    drawBar(3, 52, player.getManaPoints());
    drawBar(5, 60, player.getExperience());

    // Draw LVL
    m_video.bg0.image(vec(1, 7), Minimap, 68);
    m_video.bg0.image(vec(2, 7), Minimap, 69);
    m_video.bg0.image(vec(3, 7), Minimap, 68);
    m_video.bg0.image(vec(4, 7), Minimap, 41);
    m_video.bg0.image(vec(5, 7), Minimap, 70 + player.getLevel());

    // Draw skill title
    m_video.bg0.image(vec(1, 9), Minimap, 80);
    m_video.bg0.image(vec(2, 9), Minimap, 81);
    m_video.bg0.image(vec(3, 9), Minimap, 82);
    m_video.bg0.image(vec(4, 9), Minimap, 68);
    m_video.bg0.image(vec(5, 9), Minimap, 68);

    // Draw skill logo
    drawSkill(0); 
}

bool CubeData::inCube(Sifteo::Vector2<char> pos) {
    return pos.x / Map::cubesPerMap == m_mapPosition.x &&
           pos.y / Map::cubesPerMap == m_mapPosition.y;
}

void CubeData::addEntityMask(const Entity &entity, Sifteo::BG1Mask &mask)
{
    if (entity.getHealthPoints() <= 0) return;

    // Check if player is in a map
    auto pos = entity.getMapPosition();
    if (inCube(pos)) {
        mask.fill(vec((pos.x % Map::tilesPerCube) * 2,
                      (pos.y % Map::tilesPerCube) * 2), vec(2,2)); 
    }
    if (entity.getAnimation() == Entity::MOVE) {
        pos = entity.nextMapPosition();
        if (inCube(pos)) {
            mask.fill(vec((pos.x % Map::tilesPerCube) * 2,
                          (pos.y % Map::tilesPerCube) * 2), vec(2,2)); 
        }
    }
}

void CubeData::drawEntity(const Entity &entity, unsigned frame)
{
    if (entity.getHealthPoints() <= 0) return;
    if (entity.getFrozenTime() > 0)
        frame = entity.getFrozenFrame();

    // Base frame
    int baseFrame = 64 * entity.getState() + 
                    (entity.getAnimation() & 0x3) * 16;

    // Check if player is in a map
    auto pos = entity.getMapPosition();
    bool correctAgain = true;
    if (inCube(pos)) {
        // Check if player is in a cube
        correctAgain = false;
        if (entity.getAnimation() == Entity::MOVE) {
            baseFrame += entity.getDirection() * 8;
            m_video.bg1.image(vec((pos.x % Map::tilesPerCube) * 2,
                                  (pos.y % Map::tilesPerCube) * 2),
                              *entity.getAsset(), baseFrame + 2*(frame % 4));
        } else {
            baseFrame += entity.getDirection() * 4;
            m_video.bg1.image(vec((pos.x % Map::tilesPerCube) * 2, 
                                  (pos.y % Map::tilesPerCube) * 2),
                              *entity.getAsset(), baseFrame + frame % 4);
        }    
    }     

    if (entity.getAnimation() == Entity::MOVE) {
        pos = entity.nextMapPosition();
        if (inCube(pos)) {
            if (correctAgain) baseFrame += entity.getDirection() * 8;
            m_video.bg1.image(vec((pos.x % Map::tilesPerCube) * 2,
                                  (pos.y % Map::tilesPerCube) * 2),
                              *entity.getAsset(), baseFrame + 2*(frame % 4) + 1);
        }
    }
}
