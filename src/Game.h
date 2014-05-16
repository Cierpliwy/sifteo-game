/*****************************************************************************
 * Copyright (c) 2014 Przemysław Lenart <przemek.lenart@gmail.com>
 *
 * CUBE CRAWLER
 ****************************************************************************/
#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "../assets.gen.h"

class Game
{
public:
    void init();
    void run();
    void cleanup();
    bool finished() { return false; }
 
    // Function is used to detect connection between cubes and specify
    // theirs screen rotations. 
    void detectNeighborhood();
   
    // Number of screens used by an application 
    const static int screensNumber = 5;

private:

    // Function used internally by detectNeighborhood()
    void checkNeighbors(int cubeID, int cubeID2, int fromSide, int x, int y);

    // Check if cube is in map range
    bool cubeInMap(int cubeID);

    // Check if cube is connected by path with player's cube
    bool cubeConnectedWithPlayersCube(int cubeID);

    // Draw static tile
    void drawStaticTile(int cubeID, Sifteo::Vector2<int> scr,
                                    Sifteo::Vector2<int> pos, int tileNr);

    // Draw animated tile
    void drawAnimatedTile(int cubeID, Sifteo::Vector2<int> scr, int tileNr);

    // Draw map on selected cube
    void drawMap(int cubeID);

    // Draw mini map on selected cube
    void drawMiniMap(int cubeID);

    // Draw stars 
    void drawStars(int cubeID);

    // Draw fog
    void drawFog(int cubeID);

    // Game map containing information about it's state
    Map map;    

    // Cube specific data
    Sifteo::VideoBuffer vid[screensNumber];
    bool cubesOnMap[screensNumber];
    Sifteo::Side cubesRotation[screensNumber];
    Sifteo::Vector2<int> cubesScreenPosition[screensNumber];
    int baseScreen;

    // Detecting frame time
    Sifteo::TimeStep ts;
    unsigned frame;
    float time;
};

#endif //GAME_H
