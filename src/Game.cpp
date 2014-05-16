/*****************************************************************************
 * Copyright (c) 2014 Przemysław Lenart <przemek.lenart@gmail.com>
 *
 * CUBE CRAWLER
 ****************************************************************************/
#include "Game.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Dungeon Crawler")
    .package("this.is.my.url", "0.1")
    .icon(Icon)
    .cubeRange(Game::screensNumber);

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

void Game::init()
{
    LOG("Used game memory: %d\n", sizeof (Game));
    baseScreen = 0;
    for (int i = 0; i < screensNumber; ++i) {
        cubesRotation[i] = TOP;
        vid[i].attach(i);
    }
    map.generate();
}

void Game::run()
{
    CubeID cube(0);
    if (cube.isTouching()) {
        map.generate();
    }

    // Calculate current frame
    time = SystemTime::now().uptime();
    frame = SystemTime::now().uptime() * 8;

    // For each cube
    for(int s = 0; s < screensNumber; ++s) {
        detectNeighborhood();
        if (cubesOnMap[s]) {
            if (cubeInMap(s)) {
                if (cubeConnectedWithPlayersCube(s)) drawMap(s);
                else drawFog(s);
            } else drawStars(s);
        } else {
            drawMiniMap(s);
        }
    }

    System::paint();
    ts.next();
}

void Game::cleanup()
{

}

void Game::detectNeighborhood()
{
    // Reset all data
    for(int i = 0; i < screensNumber; ++i) {
        cubesOnMap[i] = false;
        cubesRotation[i] = TOP;
        cubesScreenPosition[i] = vec(0,0);
    }

    // Start detecting neighborhood from cube where is the player located.
    checkNeighbors(-1, baseScreen, 0, map.getPlayer().pos.x / Map::screensPerMap,
                                      map.getPlayer().pos.y / Map::screensPerMap);
}

void Game::checkNeighbors(int cubeID, int cubeID2, int fromSide, int x, int y)
{
    // If cube was already processed, let's finish.
    if (cubesOnMap[cubeID2]) return;

    // If function was called by other cube find it's position and calibrate
    // screen rotation.
    auto n = vid[cubeID2].physicalNeighbors();
    if (cubeID >= 0) {

        // Find caller cube
        int i;
        for(i = TOP; i < NUM_SIDES; ++i) {
            CubeID id = n.cubeAt((Side)i);
            if (id == cubeID) break;
     
        }

        // Correct your screen rotation
        cubesRotation[cubeID2] = 
            (Side)((NUM_SIDES + i - fromSide) % NUM_SIDES);
    }

    cubesOnMap[cubeID2] = true;
    cubesScreenPosition[cubeID2] = vec(x,y);

    // Detect other cubes
    for(int j = TOP; j < NUM_SIDES; ++j) {
        CubeID id = n.cubeAt((Side)j);
        if (id != CubeID()) {
            int nposx = x;
            int nposy = y;

            int virt = (NUM_SIDES + j - cubesRotation[cubeID2]) % NUM_SIDES;
            if (virt == TOP) nposy--;
            if (virt == LEFT) nposx--;
            if (virt == BOTTOM) nposy++;
            if (virt == RIGHT) nposx++;
           
            checkNeighbors(cubeID2, id, (virt + BOTTOM) % NUM_SIDES,
                           nposx, nposy);
        }
    }
}

bool Game::cubeInMap(int cubeID)
{
    if (cubesScreenPosition[cubeID].x < 0) return false;
    if (cubesScreenPosition[cubeID].x >= Map::screensPerMap) return false;
    if (cubesScreenPosition[cubeID].y < 0) return false;
    if (cubesScreenPosition[cubeID].y >= Map::screensPerMap) return false;
    return true;
}

bool Game::cubeConnectedWithPlayersCube(int cubeID)
{
    return map.isScreenConnected(cubesScreenPosition[cubeID].x,
                                 cubesScreenPosition[cubeID].y,
                                 cubesScreenPosition);
}

void Game::drawStaticTile(int cubeID, Vector2<int> scr, Vector2<int> pos, 
                          int tileNr)
{
   vid[cubeID].bg0.image(scr, Map1, tileNr + map.at(pos.x, pos.y).id % 4);
}

void Game::drawAnimatedTile(int cubeID, Sifteo::Vector2<int> scr, int tileNr)
{
   vid[cubeID].bg0.image(scr, Map1, tileNr + frame % 4);
}

void Game::drawMap(int cubeID)
{
    // Initialize mode specific for map drawing
    vid[cubeID].initMode(BG0_SPR_BG1);
    vid[cubeID].setOrientation(cubesRotation[cubeID]);

    // Get starting tile position
    Vector2<int> tpos;
    tpos.x = cubesScreenPosition[cubeID].x * Map::tilesPerScreen;
    tpos.y = cubesScreenPosition[cubeID].y * Map::tilesPerScreen;

    // Screen tile position
    Vector2<int> scr = vec(0,0);

    // For each tile
    for(int x = tpos.x; x < tpos.x + Map::tilesPerScreen; ++x) {
        for(int y = tpos.y; y < tpos.y + Map::tilesPerScreen; ++y) {
            switch(map.at(x,y).type) {
                case Tile::FLOOR:
                    drawStaticTile(cubeID, scr, vec(x,y), 0);
                    break;
                case Tile::WALL:
                    drawStaticTile(cubeID, scr, vec(x,y), 4);
                    break;
                case Tile::STAIRS:
                    drawStaticTile(cubeID, scr, vec(x,y), 12);
                    break;
                case Tile::MP:
                    drawAnimatedTile(cubeID, scr, 16);
                    break;
                case Tile::HP:
                    drawAnimatedTile(cubeID, scr, 20);
                    break;
                case Tile::EXP:
                    drawAnimatedTile(cubeID, scr, 24);
                    break;
                case Tile::KEY:
                    drawAnimatedTile(cubeID, scr, 28);
                    break;
            }
            scr.y += 2;
        }
        scr.x += 2;
        scr.y = 0;
    }
}

void Game::drawStars(int cubeID)
{
    // Init stars mode
    vid[cubeID].initMode(BG0);
    vid[cubeID].setOrientation(cubesRotation[cubeID]);
    vid[cubeID].bg0.image(vec(0,0), Stars, 0);
    vid[cubeID].bg0.setPanning(vec((int)(time*8),(int)(time*8)));
}

void Game::drawFog(int cubeID)
{
    // Init fog mode
    vid[cubeID].initMode(BG0);
    vid[cubeID].setOrientation(cubesRotation[cubeID]);
    vid[cubeID].bg0.image(vec(0,0), Fog, 0);
    vid[cubeID].bg0.setPanning(vec((int)(time*30), 0));
}

void Game::drawMiniMap(int cubeID)
{
    // Init minimap mode
    vid[cubeID].initMode(BG0_BG1);
    vid[cubeID].setOrientation(cubesRotation[cubeID]);

    // Clear map
    for (int x = 0; x < 16; ++x)
        for(int y = 0; y < 16; ++y)
            vid[cubeID].bg0.image(vec(x,y), Minimap, 0);

    // Minimap offset:
    int mX = 7, mY = 7;

    // Draw minimap border
    for(int x = mX; x < mX + 8; x++) {
        vid[cubeID].bg0.image(vec(x, mY - 1), Minimap, 37);
        vid[cubeID].bg0.image(vec(x, mY + 8), Minimap, 36);
    }
    for(int y = mY; y < mY + 8; y++) {
        vid[cubeID].bg0.image(vec(mX - 1, y), Minimap, 38);
        vid[cubeID].bg0.image(vec(mX + 8, y), Minimap, 39);
    }
    vid[cubeID].bg0.image(vec(mX - 1, mY - 1), Minimap, 32);
    vid[cubeID].bg0.image(vec(mX + 8, mY - 1), Minimap, 33);
    vid[cubeID].bg0.image(vec(mX - 1, mY + 8), Minimap, 34);
    vid[cubeID].bg0.image(vec(mX + 8, mY + 8), Minimap, 35);

    // Draw minimap content
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char info = map.atScreen(x,y);
            int s;
            for(s = 0; s < screensNumber; ++s)
                if (cubesOnMap[s] && 
                    cubesScreenPosition[s].x == x &&
                    cubesScreenPosition[s].y == y) break;
            info &= 0xF;
            if (s != screensNumber)
                info += 16;
            vid[cubeID].bg0.image(vec(mX + x, mY + y), Minimap, info);
        }
    }
}
