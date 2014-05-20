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

#include "Map.h"
#include "Game.h"
#include "Enemy.h"
#include "../assets.gen.h"

using namespace Sifteo;

bool Map::isTileBlockable(int x, int y, const class Player &player,
                          const Enemy enemies[]) const {
    if (tiles[x][y].type == Tile::CHEST ||
        tiles[x][y].type == Tile::STAIRS ||
        tiles[x][y].type == Tile::WALL ||
        tiles[x][y].entity != Tile::NONE) return true;

    if (x > 0 && tiles[x-1][y].entity == Tile::PLAYER &&
        player.getAnimation() == Entity::MOVE &&
        player.getDirection() == RIGHT) return true;
    if (x < mapSize && tiles[x+1][y].entity == Tile::PLAYER && 
        player.getAnimation() == Entity::MOVE &&
        player.getDirection() == LEFT) return true;
    if (y > 0 && tiles[x][y-1].entity == Tile::PLAYER && 
        player.getAnimation() == Entity::MOVE &&
        player.getDirection() == BOTTOM) return true;
    if (y < mapSize && tiles[x][y+1].entity == Tile::PLAYER && 
        player.getAnimation() == Entity::MOVE &&
        player.getDirection() == TOP) return true;

    if (x > 0 && tiles[x-1][y].entity == Tile::ENEMY &&
        enemies[(int)tiles[x-1][y].id].getAnimation() == Entity::MOVE &&
        enemies[(int)tiles[x-1][y].id].getDirection() == RIGHT) return true;
    if (x < mapSize && tiles[x+1][y].entity == Tile::ENEMY && 
        enemies[(int)tiles[x+1][y].id].getAnimation() == Entity::MOVE &&
        enemies[(int)tiles[x+1][y].id].getDirection() == LEFT) return true;
    if (y > 0 && tiles[x][y-1].entity == Tile::ENEMY && 
        enemies[(int)tiles[x][y-1].id].getAnimation() == Entity::MOVE &&
        enemies[(int)tiles[x][y-1].id].getDirection() == BOTTOM) return true;
    if (y < mapSize && tiles[x][y+1].entity == Tile::ENEMY && 
        enemies[(int)tiles[x][y+1].id].getAnimation() == Entity::MOVE &&
        enemies[(int)tiles[x][y+1].id].getDirection() == TOP) return true;

    return false;
}

bool Map::isConnected()
{
    for (int x = 0; x < cubesPerMap; ++x)
        for(int y = 0; y < cubesPerMap; ++y)
            map[x][y] &= ~VISITED;

    int visited = 0;
    visitNode(visited,0,0);
    return visited == cubesPerMap * cubesPerMap;
}

void Map::visitNode(int &visited, int x, int y)
{
    if (map[x][y] & VISITED) return;

    visited++;
    map[x][y] |= VISITED;

    if (map[x][y] & LINKED_UP) visitNode(visited, x, y-1);
    if (map[x][y] & LINKED_DOWN) visitNode(visited, x, y+1);
    if (map[x][y] & LINKED_LEFT) visitNode(visited, x-1, y);
    if (map[x][y] & LINKED_RIGHT) visitNode(visited, x+1, y);
}

bool Map::isCubeConnection(CubeData &cube, CubeData &cube2,
                           CubeData cubes[])
{
    for (int x = 0; x < cubesPerMap; ++x)
        for(int y = 0; y < cubesPerMap; ++y)
            map[x][y] &= ~VISITED;

    bool visited = false;
    visitCube(visited, cube.getMapPosition(), cube2, cubes);
    return visited;
}

void Map::visitCube(bool &visited, Sifteo::Vector2<char> pos, CubeData &cube2,
                    CubeData cubes[])
{
    // If cube was visited or we have got a result, break.
    if ((map[(int)pos.x][(int)pos.y] & VISITED) || visited) return;

    // If it's final cube, it's the end.
    if (pos == cube2.getMapPosition()) {
        visited = true;
        return;
    }

    // Need to check if cube is used to build a map
    char cube;
    for(cube = 0; cube < CubeData::cubeCount; ++cube)
        if (cubes[(int)cube].getMapPosition() == pos) break;
    if (cube == CubeData::cubeCount) return;

    // Set as visited
    map[(int)pos.x][(int)pos.y] |= VISITED;

    // Check neighbors
    if (map[(int)pos.x][(int)pos.y] & LINKED_UP) 
        visitCube(visited, vec<char>(pos.x, pos.y-1), cube2, cubes);
    if (map[(int)pos.x][(int)pos.y] & LINKED_DOWN) 
        visitCube(visited, vec<char>(pos.x, pos.y+1), cube2, cubes);
    if (map[(int)pos.x][(int)pos.y] & LINKED_LEFT) 
        visitCube(visited, vec<char>(pos.x-1, pos.y), cube2, cubes);
    if (map[(int)pos.x][(int)pos.y] & LINKED_RIGHT) 
        visitCube(visited, vec<char>(pos.x+1, pos.y), cube2, cubes); 
}

bool Map::hasAdjTileType(int tileX, int tileY, Tile::Type type)
{
    for(int x = MAX(0,tileX-1); x <= MIN(mapSize-1,tileX+1); ++x)
        for(int y = MAX(0,tileY-1); y <= MIN(mapSize-1,tileY+1); ++y)
            if (!(x == tileX && y == tileY) && tiles[x][y].type == (char)type)
                return true;
    return false;
}

void Map::generateTile(int count, tile_ctor_f ctor)
{
    int x, y;
    for(int k = 0; k < count; ++k) {
        x = rnd.raw() % mapSize;
        y = rnd.raw() % mapSize;
        while(1) {
            if (tiles[x][y].type == Tile::FLOOR &&
                tiles[x][y].entity == Tile::NONE &&
                !(hasAdjTileType(x,y, Tile::CHEST) ||
                  hasAdjTileType(x,y, Tile::STAIRS))) {
                tiles[x][y] = (this->*ctor)(k, x, y);
                break;
            }
            x++;
            if (x == mapSize) {x = 0; y++;}
            if (y == mapSize) y = 0;
        }
    }
}

void Map::generate()
{
    // Setup theme
    m_theme = &Map1;

    // Initialize temporary map structure as fully connected.
    // Each cube is connected with adjacent cubes.
    for (int x = 0; x < cubesPerMap; ++x)  {
        for (int y = 0; y < cubesPerMap; ++y) {
            map[x][y] = 0;
            if (x > 0) map[x][y] |= LINKED_LEFT;
            if (x < cubesPerMap - 1) map[x][y] |= LINKED_RIGHT;
            if (y > 0) map[x][y] |= LINKED_UP;
            if (y < cubesPerMap - 1) map[x][y] |= LINKED_DOWN;
        }
    }

    // Reduce map connections between cubes to add corridors.
    // We have to make sure that there is a way from each cube 
    // to another one.
    Random rnd;
    int totalEdges = 2 * cubesPerMap * cubesPerMap -
                     2 * cubesPerMap;
    int edgesToRemove = totalEdges - (cubesPerMap * cubesPerMap - 1);
    edgesToRemove *= rnd.uniform(0.9,1);

    for(int e=0; e < edgesToRemove; ++e) {
        while (1) {
            // Select random cube and it's connection
            int x,y, dir;
            x = rnd.raw() % cubesPerMap;
            y = rnd.raw() % cubesPerMap;
            dir = 1 << (rnd.raw() % 4);
            // Check if link exists
            if (map[x][y] & dir) {
                // Remove link and check connectivity
                int nx,ny,ndir;
                if (dir == LINKED_UP) {nx = x; ny = y-1; ndir=LINKED_DOWN;}
                if (dir == LINKED_DOWN) {nx = x; ny = y+1; ndir=LINKED_UP;}
                if (dir == LINKED_LEFT) {nx = x-1; ny = y; ndir=LINKED_RIGHT;}
                if (dir == LINKED_RIGHT) {nx = x+1; ny = y; ndir=LINKED_LEFT;}
                map[x][y] &= ~dir;
                map[nx][ny] &= ~ndir;

                // If a map is still connected after removing connections,
                // continue graph reduction. If not, restore links.
                if (isConnected()) break;
                map[x][y] |= dir;
                map[nx][ny] |= ndir;
            }
        }
    }

    // Based on temporary cube-based map, generate real tiles.
    for(int x = 0; x < mapSize; ++x) {
        for(int y = 0; y < mapSize; ++y) {
            tiles[x][y] = floorConstructor(0, x, y);
        }
    }
    for(int x = 0; x < cubesPerMap; ++x) {
        for(int y = 0; y < cubesPerMap; ++y) {
            int bx = x * tilesPerCube;
            int by = y * tilesPerCube;

            // Generate walls
            if (!(map[x][y] & LINKED_UP))
                for(int k = bx; k < bx + tilesPerCube; ++k)
                    tiles[k][by].type = Tile::WALL;

            if (!(map[x][y] & LINKED_DOWN))
                for(int k = bx; k < bx + tilesPerCube; ++k)
                    tiles[k][by + tilesPerCube - 1].type = Tile::WALL;

            if (!(map[x][y] & LINKED_LEFT))
                for(int k = by; k < by + tilesPerCube; ++k)
                    tiles[bx][k].type = Tile::WALL;

            if (!(map[x][y] & LINKED_RIGHT))
                for(int k = by; k < by + tilesPerCube; ++k)
                    tiles[bx + tilesPerCube - 1][k].type = Tile::WALL;
        }
    }

    // Generate players, enemies then all block able objects.
    generateTile(1, &Map::playerConstructor);
    generateTile(Enemy::enemiesCount, &Map::enemyConstructor);
    generateTile(5, &Map::chestConstructor);
    generateTile(1, &Map::stairsConstructor);
    generateTile(8, &Map::keyConstructor);
    generateTile(16, &Map::mpConstructor);
    generateTile(16, &Map::hpConstructor);
    generateTile(16, &Map::expConstructor);
}

Tile Map::playerConstructor(int count, int x, int y) {
    return Tile(Tile::FLOOR, Tile::PLAYER, 0, rnd.raw() % 4);    
}

Tile Map::enemyConstructor(int count, int x, int y) {
    return Tile(Tile::FLOOR, Tile::ENEMY, count, rnd.raw() % 4);    
}

Tile Map::chestConstructor(int count, int x, int y) {
    return Tile(Tile::CHEST, Tile::NONE, 0, 0);
}

Tile Map::stairsConstructor(int count, int x, int y) {
    return Tile(Tile::STAIRS, Tile::NONE, 0, 0);
}

Tile Map::keyConstructor(int count, int x, int y) {
    return Tile(Tile::KEY, Tile::NONE, 0, 0);
}

Tile Map::hpConstructor(int count, int x, int y) {
    return Tile(Tile::HP, Tile::NONE, 0, 0);
}

Tile Map::mpConstructor(int count, int x, int y) {
    return Tile(Tile::MP, Tile::NONE, 0, 0);
}

Tile Map::expConstructor(int count, int x, int y) {
    return Tile(Tile::EXP, Tile::NONE, 0, 0);
}

Tile Map::floorConstructor(int count, int x, int y) {
    return Tile(Tile::FLOOR, Tile::NONE, 0, rnd.raw() % 4);
}

Tile Map::wallConstructor(int count, int x, int y) {
    return Tile(Tile::WALL, Tile::NONE, 0, rnd.raw() % 4);
}
