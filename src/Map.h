/*****************************************************************************
 * Copyright (c) 2014 Przemysław Lenart <przemek.lenart@gmail.com>
 *
 * CUBE CRAWLER
 ****************************************************************************/

#ifndef MAP_H
#define MAP_H

#include <sifteo.h>

// Representation of a single tile on a map. It's size must be as small as
// possible to fit in program's memory.
struct Tile
{
    // Every tile is of certain type
    enum Type {
        FLOOR,
        WALL,
        PLAYER,
        ENEMY,
        CHEST,
        STAIRS,
        KEY,
        HP,
        MP,
        EXP
    };

    Tile() : type(Tile::FLOOR), id(0) {}
    Tile(char type, char id) : type(type), id(id) {}

    // Type of an object
    char type;

    // Id is used to identify an object
    char id;
};

// Player structure
struct Player {
    int experience;
    int healthPoints;
    int manaPoints;
    int attack;
    int defence;
    
    Sifteo::Vector2<int> pos;
};

// Enemy structure
struct Enemy {
    int healthPoints;
    int attack;
    int defence;

    Sifteo::Vector2<int> pos;
};

// Information used to describe connections between screens and theirs
// state
enum ScreenState {
    LINKED_UP = 1,
    LINKED_DOWN = 2,
    LINKED_RIGHT = 4,
    LINKED_LEFT = 8,
    LAST_LINK = 8,
    VISITED = 16,
    FOG = 32,
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

    // Return information about an screen map
    char atScreen(int x, int y) { return map[x][y]; }

    // Return information about player
    Player& getPlayer() {
        return player;
    }

    // Return information about enemy
    Enemy& getEnemy(int index) {
        return enemies[index];
    }

    // Check if screen is connected
    bool isScreenConnected(int x, int y, Sifteo::Vector2<int> *pos); 

    // DATA -------------------------------------------------------------------

    // Number of screens for one dimension of a map
    static const int screensPerMap = 8;
    // Number of tiles for one dimension of a screen
    static const int tilesPerScreen = 8;
    // Total number of tiles in one dimension of a map
    static const int mapSize = screensPerMap * tilesPerScreen;
    // Total number of enemies
    static const int enemiesNum = 32;

private:

    // Function checks connectivity of a graph based on depth search
    bool isConnected();

    // Helper function for isConnected(). It recursively visits not visited
    // nodes in a graph starting from (i,j) node.
    void visitNode(int &visited, int i, int j);

    // Helper function for isScreenConnected().
    void visitScreen(bool &visited, Sifteo::Vector2<int> *pos, int i, int j);

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

    // Helpful two dimensional structure of a map per screens used by
    // program's algorithms.
    char map[screensPerMap][screensPerMap];

    // Random generator for a map's functions.
    Sifteo::Random rnd;

    // Player
    Player player;

    // Enemies
    Enemy enemies[enemiesNum];
};


#endif //MAP_H
