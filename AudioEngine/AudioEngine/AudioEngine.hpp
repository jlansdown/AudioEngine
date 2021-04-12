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

class CAudioEngine {
public:
    static void Init();
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    void LoadBank(const string& strBank, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const string& strEventName);
    void LoadSound(const string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const string& strSoundName);
    void Set3dListenerAndOrientation(const Vector3& vPos = Vector3{0, 0, 0}, float fVolumedB = 0.0f);
    void PlaySound(const string$ strSoundName, const vector3& vPos = Vector3{0, 0, 0})
    void PlayEvent(const string& strEventName);
    void StopChannel(int nChannelId);
    void StopEvent(const string& strEventName, bool bImmediate = false);
    void GetEventParameter(const string& strEventName, const string& strEventParameter, float* parameter);
    void SetEventParameter(const string& strEventName, const string& strParameter, float fValue);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    bool isPlaying(int nChannelId) const;
    bool IsEventPlaying(const string& strEventName) const;
    float dbToVolume(float db);
    float VolumeTodb(float volume);
    FMOD_VECTOR VectorToFmod(const Vector& vPosition);
};

#endif /* AudioEngine_hpp */
