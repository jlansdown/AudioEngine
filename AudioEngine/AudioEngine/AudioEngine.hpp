//
//  AudioEngine.hpp
//  AudioEngine
//
//  Created by James Lansdown on 4/10/21.
//

#ifndef AudioEngine_hpp
#define AudioEngine_hpp

#include <stdio.h>
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Implementation {
    Implementation();
    ~Implementation();
    
    void Update();
    
    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;
    
    int mnNextChannelID;
    
    typedef map<string, FMOD::Sound*> SoundMap;
    typedef map<int, FMOD::Channel*> ChannelMap;
    typedef map<string, FMOD::Studio::EventInstance*> EventMap;
    typedef map<string, FMOD::Studio::Bank*> BankMap;
    
    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

#endif /* AudioEngine_hpp */
