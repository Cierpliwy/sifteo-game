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

#include "Game.h"
#include "../assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Dungeon Crawler")
    .package("this.is.my.url", "0.1")
    .icon(Icon)
    .cubeRange(CubeData::cubeCount);

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

void Game::init()
{
    LOG("Used game memory: %d\n", sizeof (Game));
    for (int i = 0; i < CubeData::cubeCount; ++i) {
        cube[i].init(i);
    }
    movementFrame = 0;
    playerCube = 0;
    minimapCube = 0;
    create();
    audioPlayer.init();
    audioPlayer.playMusic();
}

void Game::create()
{
    // Prepare map
    map.generate();

    // Prepare player and enemies
    char enemyNum = 0;
    for(char x = 0; x < Map::mapSize; ++x) {
        for(char y = 0; y < Map::mapSize; ++y) {
            if (map.at(x,y).entity == Tile::PLAYER) {
                player.init();
                player.setMapPosition(vec(x,y));
            }
            if (map.at(x,y).entity == Tile::ENEMY) {
                enemies[(int)enemyNum].init();
                enemies[(int)enemyNum].setMapPosition(vec(x,y));
                enemies[(int)enemyNum].setAsset(Zombie);
                map.at(x,y).id = enemyNum;
                enemyNum++;
            }
        }
    }
    player.setAsset(Player);
}

void Game::run()
{
    System::finish();

    //TODO: tmp
    CubeID tmpCube(minimapCube);
    if (tmpCube.isTouching()) {
        create();
    }

    // Calculate current frame
    time = SystemTime::now().uptime();
    unsigned tmpFrame = SystemTime::now().uptime() * 32;
    unsigned lastTmpFrame = 0;

    if (tmpFrame != lastTmpFrame) {
        lastTmpFrame = tmpFrame;
        frame++;
    }

    // Do all actions
    if (frame % 4 == 0 && frame != movementFrame) {
        movementFrame = frame;

        // Update cubes 
        for (int i = 0; i < CubeData::cubeCount; ++i)
            cube[i].update();
        
        // Update player and enemy movement
        for (int i = 0; i < Enemy::enemiesCount; ++i) {

            // Update only enemies on the screen
            int s;
            for(s = 0; s < CubeData::cubeCount; ++s) {
                if (cube[s].isUsed() && ((
                    enemies[i].getMapPosition().x / Map::cubesPerMap ==
                    cube[s].getMapPosition().x &&
                    enemies[i].getMapPosition().y / Map::cubesPerMap ==
                    cube[s].getMapPosition().y) ||
                    (enemies[i].nextMapPosition().x / Map::cubesPerMap ==
                    cube[s].getMapPosition().x &&
                    enemies[i].nextMapPosition().y / Map::cubesPerMap ==
                    cube[s].getMapPosition().y)))
                    break;
            }
            if (s != CubeData::cubeCount) {
                enemies[i].setIsVisible(true);
                enemies[i].update(map, audioPlayer, player, enemies, rnd);
            } else {
                enemies[i].setIsVisible(false);
            }
        }
        player.update(map, audioPlayer, cube[(int)minimapCube], enemies, frame);
    }
 
    // Detect neighbor cubes next to player's cube
    auto playerPos = player.getMapPosition();

    // Change player cube for easy transition
    for(int i = 0; i < CubeData::cubeCount; ++i) {
        if (cube[i].getMapPosition().x == player.getMapPosition().x / Map::cubesPerMap &&
            cube[i].getMapPosition().y == player.getMapPosition().y / Map::cubesPerMap)
            playerCube = i;
    }

    // Detect neighborhood
    cube[(int)playerCube].setMapPosition(vec<char>(playerPos.x / Map::cubesPerMap,
                                                   playerPos.y / Map::cubesPerMap));
    cube[(int)playerCube].detectNeighborhood(cube);

    // Detect minimap cube
    bool shownMiniMap = false;
    if (!cube[(int)minimapCube].isUsed()) shownMiniMap = true;

    // Draw proper scene on each cube
    for(int s = 0; s < CubeData::cubeCount; ++s) {
        // If cube is used for building map.
        if (cube[s].isUsed()) {
            // If cube is in map area
            if (cube[s].isInMap()) {
                if (map.isCubeConnection(cube[s], cube[(int)playerCube], cube)){
                    cube[s].drawMap(map, frame, player, enemies);

                    // Clear fog of war
                    map.atCube(cube[s].getMapPosition().x,
                               cube[s].getMapPosition().y) |= NO_FOG; 
                } else {
                    cube[s].drawFog(time);
                }
            } else cube[s].drawStars(time);
        } else {
            // We should draw mini map on single cube
            if (shownMiniMap) {
                if (s == minimapCube) cube[s].drawMiniMap(map, cube, player);
                else cube[s].drawStars(time);
            } else {
                shownMiniMap = true;
                minimapCube = s;
                cube[s].drawMiniMap(map, cube, player);
            }
        }
    }

    // Paint all changes
    System::paint();
    ts.next();
}

void Game::cleanup()
{

}

/*
void Game::setEntityMask(Sifteo::BG1Mask &mask, Entity &entity, 
                         Sifteo::Vector2<int> scr) {

    mask.fill(scr, vec(2,2));
    if (entity.animation == Entity::MOVE) {
        switch(entity.dir) {
            case TOP:
                scr.y -= 2;
                break;
            case BOTTOM:
                scr.y += 2;
                break;
            case RIGHT:
                scr.x += 2;
                break;
            case LEFT:
                scr.x -= 2;
                break;
        }
        if (scr.x >= 0 && scr.x < 16 &&
            scr.y >= 0 && scr.y < 16) {
            mask.fill(scr, vec(2,2));
        }
    }
}

void Game::drawEntity(int cubeID, Entity &entity, const PinnedAssetImage &img, 
                      Sifteo::Vector2<int> scr) {
    int baseFrame = (entity.animation & 0x3) * 16;
    if (entity.animation == Entity::MOVE) {
        baseFrame += entity.dir * 8;
        vid[cubeID].bg1.image(scr, img, baseFrame + 2*(frame % 4));
    } else {
        baseFrame += entity.dir * 4;
        vid[cubeID].bg1.image(scr, img, baseFrame + frame % 4);
    }

    if (entity.animation == Entity::MOVE) {
        switch(entity.dir) {
            case TOP:
                scr.y -= 2;
                break;
            case BOTTOM:
                scr.y += 2;
                break;
            case RIGHT:
                scr.x += 2;
                break;
            case LEFT:
                scr.x -= 2;
                break;
        }
        if (scr.x >= 0 && scr.x < 16 &&
            scr.y >= 0 && scr.y < 16) {
            vid[cubeID].bg1.image(scr, img, baseFrame + 2*(frame % 4) + 1);
        }
    }

}
*/
