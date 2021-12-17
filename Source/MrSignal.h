#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <JuceHeader.h>

class MrSignal
{

public:

    static void impulse(const juce::AudioSourceChannelInfo& audioSrcChnlInfo, float ampImpulse)
    {
        std::vector<float> signal(audioSrcChnlInfo.numSamples, 0);
        
        signal.at(0) = ampImpulse;

        copyToChannels(audioSrcChnlInfo, signal);
    }

    static void copyToChannels(const juce::AudioSourceChannelInfo& audioSrcChnlInfo, std::vector<float> &signal)
    {
        for (int chnlNum = 0; chnlNum < audioSrcChnlInfo.buffer->getNumChannels(); ++chnlNum)
        {
            float* const audioSmplBuf =
                audioSrcChnlInfo.buffer->getWritePointer(chnlNum, audioSrcChnlInfo.startSample);

            int i = 0;
            for (auto iter = signal.begin(); iter != signal.end(); ++i, ++iter) {
                audioSmplBuf[i] = *iter;
            }
        }
    }
};