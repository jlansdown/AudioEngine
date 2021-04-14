//
//  AudioEngine.cpp
//  AudioEngine
//
//  Created by James Lansdown on 4/10/21.
//

#include "AudioEngine.hpp"

Implementation::Implementation()
{
    mpStudioSystem = NULL;
    CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
    
    mpSystem = NULL;
    
    //Studio::System::getLowLevelSystem is now Studio::System::getCoreSystem
    CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation()
{
    CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
    CAudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
    vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    CAudioEngine::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;

void CAudioEngine::Init()
{
    sgpImplementation = new Implementation;
}

void CAudioEngine::Update()
{
    sgpImplementation->Update();
}

void CAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;
    
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
    
    FMOD::Sound* pSound = nullptr;
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    
    if (pSound)
    {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }
    
}

void CAudioEngine::UnLoadSound(const string &strSoundName)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;
    
    CAudioEngine::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->mSounds.erase(tFoundIt);
}

int CAudioEngine::PlaySounds(const string &strSoundName, const Vector3& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelID++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode &FMOD_3D)
        {
            FMOD_VECTOR position = VectorToFmod(vPosition);
            CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        CAudioEngine::ErrorCheck(pChannel->setVolume((dbToVolume(fVolumedB))));
        CAudioEngine::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void CAudioEngine::SetChannel3dPosition(int nChannelId, const Vector3 &vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;
    
    FMOD_VECTOR position = VectorToFmod(vPosition);
    CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void CAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;
    
    CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

void CAudioEngine::LoadBank(const string &strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end())
        return;
    
    FMOD::Studio::Bank* pBank;
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank)
    {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void CAudioEngine::LoadEvent(const string &strEventName)
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt != sgpImplementation->mEvents.end())
        return;
    
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription)
    {
        FMOD::Studio::EventInstance* pEventInstnce = NULL;
        CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstnce));
        if (pEventInstnce)
        {
            sgpImplementation->mEvents[strEventName] = pEventInstnce;
        }
    }
}

void CAudioEngine::PlayEvent(const string &strEventName)
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
    {
        LoadEvent(strEventName);
        tFoundIt = sgpImplementation->mEvents.find(strEventName);
        if (tFoundIt == sgpImplementation->mEvents.end())
            return;
    }
    tFoundIt->second->start();
}

void CAudioEngine::StopEvent(const string &strEventName, bool bImmediate)
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    
    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool CAudioEngine::IsEventPlaying(const string &strEventName) const
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;
    
    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    //Casting FMOD_RESULT as FMOD_STUDIO_PLAYBACK_STATE
    if ((FMOD_STUDIO_PLAYBACK_STATE)tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
    {
        return true;
    }
    return false;
}

void CAudioEngine::GetEventParameter(const string &strEventName, const string &strEventParameterName, float *parameter)
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    
    CAudioEngine::ErrorCheck(tFoundIt->second->getParameterByName(strEventParameterName.c_str(), parameter));
}

void CAudioEngine::SetEventParameter(const string &strEventName, const string &strParameterName, float fValue)
{
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;
    
    CAudioEngine::ErrorCheck(tFoundIt->second->setParameterByName(strParameterName.c_str(), fValue));
}

FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector3 &vPosition)
{
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

float CAudioEngine::dbToVolume(float db)
{
    return powf(10.0f, 0.05f * db);
}

float CAudioEngine::VolumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}

int CAudioEngine::ErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        cout << "FMOD ERROR " << result << endl;
        return 1;
    }
    return 0;
}

void CAudioEngine::Shutdown()
{
    delete sgpImplementation;
}
