/******************************************************************************
 * DUNGEON CRAWLER
 * Copyright by Przemysław Lenart
 *****************************************************************************/

#include "Game.h"

void main()
{
    static Game game;
    game.init();
    while (!game.finished()) game.run();
    game.cleanup();
}
