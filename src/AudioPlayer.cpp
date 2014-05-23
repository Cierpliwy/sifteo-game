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

#include "AudioPlayer.h"
#include "../assets.gen.h"
using namespace Sifteo;

void AudioPlayer::init() {
    for (int i = 0; i < AudioChannel::NUM_CHANNELS; ++i) {
        channel[i].init(i);
    }
}

void AudioPlayer::playMusic() {
    AudioTracker::setVolume(AudioChannel::MAX_VOLUME/8);
    AudioTracker::play(Music);
}

void AudioPlayer::playSound(SoundType type) {
    // Find free channel
    int i;
    for (i = 0; i < AudioChannel::NUM_CHANNELS; ++i) {
        if (!channel[i].isPlaying()) break; 
    }

    // Don't play if we don't have free channel
    if (i == AudioChannel::NUM_CHANNELS) return;
   
    switch(type) {
        case DEAD: 
            channel[i].play(DeadSound, AudioChannel::ONCE);
            break;
        case DEAD_ENEMY:
            channel[i].play(DeadEnemySound, AudioChannel::ONCE);
            break;
        case HIT: {
           int j = rnd.raw() % 3;
           if (j == 0)
               channel[i].play(Hit1Sound, AudioChannel::ONCE);
           if (j == 1)
               channel[i].play(Hit2Sound, AudioChannel::ONCE);
           if (j == 2)
               channel[i].play(Hit3Sound, AudioChannel::ONCE);
           break;}
        case KEY:
            channel[i].play(KeySound, AudioChannel::ONCE);
            break;
        case LEVEL:
            channel[i].play(LevelSound, AudioChannel::ONCE);
            break;
        case OK:
            channel[i].play(OkSound, AudioChannel::ONCE);
            break;
        case ORB:
            channel[i].play(OrbSound, AudioChannel::ONCE);
            break;
        case STEP:
            channel[i].play(StepSound, AudioChannel::ONCE);
            break;
    }
}
