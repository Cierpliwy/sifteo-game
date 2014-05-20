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

#ifndef MAP_H
#define MAP_H

#include <sifteo.h>

class CubeData;
class Enemy;

// Representation of a single tile on a map. It's size must be as small as
// possible to fit in program's memory.
struct Tile
{
    // Every tile is of certain type
    enum Type {
        FLOOR,
        BLOODED_FLOOR,
        WALL,
        CHEST,
        STAIRS,
        KEY,
        HP,
        MP,
        EXP
    };

    // Tile can have players or enemies on them.
    enum Entity {
        NONE,
        PLAYER,
        ENEMY
    };

    Tile() : type(Tile::FLOOR), entity(NONE), id(0), state(0) {}
    Tile(char type, char entity, char id, char state) : 
        type(type), 
        entity(entity), 
        id(id), 
        state(state) {}

    // Type of a tile
    char type;

    // Is player or enemy on a tile
    char entity;

    // Id is used to identify an object
    char id;

    // State
    // (0-1): used for tiles graphics
    char state;
};

// Information used to describe connections between cubes and theirs
// state
enum CubeState {
    LINKED_UP = 1,
    LINKED_DOWN = 2,
    LINKED_RIGHT = 4,
    LINKED_LEFT = 8,
    LAST_LINK = 8,
    VISITED = 16,
    NO_FOG = 32,
};

// Map class has all informations required for map graphical representation
// and processing of events happening on it.
class Map
{
public:

    // Generate new map
    void generate();

    // Return information about a tile in global position
    Tile& at(int x, int y) { return tiles[x][y]; }
    const Tile& at(int x, int y) const { return tiles[x][y]; }
    bool isTileBlockable(int x, int y, const class Player &player,
                         const Enemy enemies[]) const;

    // Return information about an cube
    char& atCube(int x, int y) { return map[x][y]; }
    char atCube(int x, int y) const { return map[x][y]; }

    // Check if cubes are connected
    bool isCubeConnection(CubeData &cube, CubeData &cube2,
                          CubeData cubes[]); 
    
    // Get map asset
    const Sifteo::PinnedAssetImage* getTheme() const {
        return m_theme;
    }

    // DATA -------------------------------------------------------------------

    // Number of cubes for one dimension of a map
    static const int cubesPerMap= 8;
    // Number of tiles for one dimension of a cube
    static const int tilesPerCube = 8;
    // Total number of tiles in one dimension of a map
    static const int mapSize = cubesPerMap * tilesPerCube;

private:

    // Function checks connectivity of a graph based on depth search
    bool isConnected();

    // Helper function for isConnected(). It recursively visits not visited
    // nodes in a graph starting from (i,j) node.
    void visitNode(int &visited, int i, int j);

    // Helper function for isCubeConnection().
    void visitCube(bool &visited, Sifteo::Vector2<char> pos, CubeData &cube2,
                   CubeData cubes[]); 

    // Detect if around tile at (i,j) position there is a tile
    // of specified type.
    bool hasAdjTileType(int i, int j, Tile::Type type);

    // Tile constructor function
    typedef Tile (Map::* tile_ctor_f)(int count, int x, int y);

    // Generate number of specified tiles on a map. Object is constructed
    // by passed function.
    void generateTile(int count, tile_ctor_f constructor);

    // Object constructors
    Tile playerConstructor(int count, int x, int y);
    Tile enemyConstructor(int count, int x, int y);
    Tile chestConstructor(int count, int x, int y);
    Tile stairsConstructor(int count, int x, int y);
    Tile keyConstructor(int count, int x, int y);
    Tile hpConstructor(int count, int x, int y);
    Tile mpConstructor(int count, int x, int y);
    Tile expConstructor(int count, int x, int y);
    Tile floorConstructor(int count, int x, int y);
    Tile wallConstructor(int count, int x, int y);

    // DATA -------------------------------------------------------------------
 
    // Main map representation structure. It stores current state of a map.
    Tile tiles[mapSize][mapSize];

    // Helpful two dimensional structure of a map per cubes used by
    // program's algorithms.
    char map[cubesPerMap][cubesPerMap];

    // Map theme
    const Sifteo::PinnedAssetImage *m_theme;

    // Random generator for a map's functions.
    Sifteo::Random rnd;
};


#endif //MAP_H
