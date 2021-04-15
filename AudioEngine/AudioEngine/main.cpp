//
//  main.cpp
//  AudioEngine
//
//  Created by James Lansdown on 4/8/21.
//

#include <iostream>
#include "AudioEngine.hpp"

int main(int argc, const char * argv[])
{
    // insert code here...
    std::cout << "test" << std::endl;
    
    CAudioEngine engine;
    
    engine.Init();
    
    engine.LoadSound("/Users/james/Desktop/AudioEngine/AudioEngine/AudioEngine/sound.wav");
    
    engine.SetChannelVolume(16, 1.005f);
    
    int test = engine.PlaySounds("/Users/james/Desktop/AudioEngine/AudioEngine/AudioEngine/sound.wav", {1,1,1}, 1.005f);
    
    while (engine.isPlaying(test)) {
        engine.Update();
    }
    
    
    engine.Shutdown();
    
    cout << "Test channel: " << test << endl;
    
    
    return 0;
}
