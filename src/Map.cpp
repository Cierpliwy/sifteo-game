/*****************************************************************************
 * Copyright (c) 2014 Przemysław Lenart <przemek.lenart@gmail.com>
 *
 * CUBE CRAWLER
 ****************************************************************************/

#include "Map.h"
#include "Game.h"

using namespace Sifteo;

bool Map::isConnected()
{
    for (int x = 0; x < screensPerMap; ++x)
        for(int y = 0; y < screensPerMap; ++y)
            map[x][y] &= ~VISITED;

    int visited = 0;
    visitNode(visited,0,0);
    return visited == screensPerMap * screensPerMap;
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

bool Map::isScreenConnected(int x, int y, Sifteo::Vector2<int> *pos)
{
    for (int x = 0; x < screensPerMap; ++x)
        for(int y = 0; y < screensPerMap; ++y)
            map[x][y] &= ~VISITED;

    bool visited = false;
    visitScreen(visited, pos, x, y);
    return visited;
}

void Map::visitScreen(bool &visited, Vector2<int> *pos, int x, int y)
{
    // If screen was visited or we have got a result, break.
    if ((map[x][y] & VISITED) || visited) return;

    // If it's player's screen. It's the end.
    if ( x == player.pos.x / tilesPerScreen && 
         y == player.pos.y / tilesPerScreen ) {
        visited = true;
        return;
    }

    // Need to check if screen is used by a player
    int screen;
    for(screen = 0; screen < Game::screensNumber; ++screen)
        if (pos[screen].x == x && pos[screen].y == y) break;
    if (screen == Game::screensNumber) return;

    // Set as visited
    map[x][y] |= VISITED;

    // Check neighbors
    if (map[x][y] & LINKED_UP) visitScreen(visited, pos, x, y-1);
    if (map[x][y] & LINKED_DOWN) visitScreen(visited, pos, x, y+1);
    if (map[x][y] & LINKED_LEFT) visitScreen(visited, pos, x-1, y);
    if (map[x][y] & LINKED_RIGHT) visitScreen(visited, pos, x+1, y);
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
    // Initialize temporary map structure as fully connected.
    // Each screen is connected with adjacent screen.
    for (int x = 0; x < screensPerMap; ++x)  {
        for (int y = 0; y < screensPerMap; ++y) {
            map[x][y] = 0;
            if (x > 0) map[x][y] |= LINKED_LEFT;
            if (x < screensPerMap - 1) map[x][y] |= LINKED_RIGHT;
            if (y > 0) map[x][y] |= LINKED_UP;
            if (y < screensPerMap - 1) map[x][y] |= LINKED_DOWN;
        }
    }

    // Reduce map connections between screens to add corridors.
    // We have to make sure that there is a way from each screen
    // to another one.
    Random rnd;
    int totalEdges = 2 * screensPerMap * screensPerMap -
                     2 * screensPerMap;
    int edgesToRemove = totalEdges - (screensPerMap * screensPerMap - 1);
    edgesToRemove *= rnd.uniform(0.9,1);

    for(int e=0; e < edgesToRemove; ++e) {
        while (1) {
            // Select random screen and it's connection
            int x,y, dir;
            x = rnd.raw() % screensPerMap;
            y = rnd.raw() % screensPerMap;
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

    // Based on temporary screen-based map, generate real tiles.
    for(int x = 0; x < mapSize; ++x) {
        for(int y = 0; y < mapSize; ++y) {
            tiles[x][y] = floorConstructor(0, x, y);
        }
    }
    for(int x = 0; x < screensPerMap; ++x) {
        for(int y = 0; y < screensPerMap; ++y) {
            int bx = x*tilesPerScreen;
            int by = y*tilesPerScreen;

            // Generate walls
            if (!(map[x][y] & LINKED_UP))
                for(int k = bx; k < bx + tilesPerScreen; ++k)
                    tiles[k][by].type = Tile::WALL;

            if (!(map[x][y] & LINKED_DOWN))
                for(int k = bx; k < bx + tilesPerScreen; ++k)
                    tiles[k][by + tilesPerScreen - 1].type = Tile::WALL;

            if (!(map[x][y] & LINKED_LEFT))
                for(int k = by; k < by + tilesPerScreen; ++k)
                    tiles[bx][k].type = Tile::WALL;

            if (!(map[x][y] & LINKED_RIGHT))
                for(int k = by; k < by + tilesPerScreen; ++k)
                    tiles[bx + tilesPerScreen - 1][k].type = Tile::WALL;
        }
    }

    // Generate players, enemies then all block able objects.
    generateTile(1, &Map::playerConstructor);
    generateTile(32, &Map::enemyConstructor);
    generateTile(5, &Map::chestConstructor);
    generateTile(1, &Map::stairsConstructor);
    generateTile(8, &Map::keyConstructor);
    generateTile(16, &Map::mpConstructor);
    generateTile(16, &Map::hpConstructor);
    generateTile(16, &Map::expConstructor);
}

Tile Map::playerConstructor(int count, int x, int y) {
    player.healthPoints = 100;
    player.manaPoints = 50;
    player.attack = 10;
    player.defence = 5;
    player.pos.x = x;
    player.pos.y = y;

    return Tile(Tile::PLAYER, count);
}

Tile Map::enemyConstructor(int count, int x, int y) {
    enemies[count].pos.x = x;
    enemies[count].pos.y = y;
    enemies[count].healthPoints = 10;
    enemies[count].attack = 7;
    enemies[count].defence = 5;

    return Tile(Tile::ENEMY, count);
}

Tile Map::chestConstructor(int count, int x, int y) {
    return Tile(Tile::CHEST, 0);
}

Tile Map::stairsConstructor(int count, int x, int y) {
    return Tile(Tile::STAIRS, 0);
}

Tile Map::keyConstructor(int count, int x, int y) {
    return Tile(Tile::KEY, 0);
}

Tile Map::hpConstructor(int count, int x, int y) {
    return Tile(Tile::HP, 20);
}

Tile Map::mpConstructor(int count, int x, int y) {
    return Tile(Tile::MP, 10);
}

Tile Map::expConstructor(int count, int x, int y) {
    return Tile(Tile::EXP, 10);
}

Tile Map::floorConstructor(int count, int x, int y) {
    return Tile(Tile::FLOOR, rnd.raw() % 4);
}

Tile Map::wallConstructor(int count, int x, int y) {
    return Tile(Tile::WALL, rnd.raw() % 4);
}
