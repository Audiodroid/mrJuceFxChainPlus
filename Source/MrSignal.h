#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <JuceHeader.h>

class MrSignal
{

public:

    /* creates a signal with an inital impulse followed by zeros as long as number of samples */
    static void impulse(const juce::AudioSourceChannelInfo& audioSrcChnlInfo, float ampImpulse)
    {
        std::vector<float> signal(audioSrcChnlInfo.numSamples, 0);
        
        signal.at(0) = ampImpulse;

        copyToChannels(audioSrcChnlInfo, signal);
    }

    /* creates ramp i.e. (0.1, 0.2, 0.3, 0.4), would need improvment for more universal purpose */
    static void ramp(const juce::AudioSourceChannelInfo& audioSrcChnlInfo)
    {
        auto numSampls = audioSrcChnlInfo.numSamples;
        std::vector<float> signal(numSampls, 0);
        
        auto fac = 0.1f;
        //auto x = numSampls;
        //while (x > 10)
        //{
        //    fac *= fac;
        //    x /= 10;
        //}

        int i = 1;
        for (auto iter = signal.begin(); iter != signal.end(); ++iter, ++i)
            *iter = fac * i;

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