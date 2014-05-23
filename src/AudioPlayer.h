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

#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <sifteo.h>

class AudioPlayer {
public:
    enum SoundType {
        DEAD,
        DEAD_ENEMY,
        HIT,
        KEY,
        LEVEL,
        OK,
        ORB,
        STEP
    };

    void init();
    void playMusic();
    void playSound(SoundType type);

private:
    Sifteo::AudioChannel channel[Sifteo::AudioChannel::NUM_CHANNELS];
    Sifteo::Random rnd;
};

#endif //AUDIO_PLAYER_H
