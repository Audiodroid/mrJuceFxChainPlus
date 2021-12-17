#pragma once

#include <vector>
#include <JuceHeader.h>
#include "MrGain.h"

class MrGainTests : public juce::UnitTest
{
public:

    MrGainTests() : juce::UnitTest("mrGain testing") {}

    void createImpulseSignal(const juce::AudioSourceChannelInfo& audioSrcChnlInfo, float ampImpulse)
    {
        const int numSamples = audioSrcChnlInfo.numSamples;

        std::vector<float> signal(numSamples, 0);
        signal.at(0) = ampImpulse;

        /// copy signal to each channel
        for (int chnlNum = 0; chnlNum < audioSrcChnlInfo.buffer->getNumChannels(); ++chnlNum)
        {
            float* const audioSmplBuf = 
                audioSrcChnlInfo.buffer->getWritePointer(chnlNum, audioSrcChnlInfo.startSample);
            
            auto iter = signal.begin();
            for (int sample = 0; sample < numSamples; ++sample, ++iter) {
                audioSmplBuf[sample] = *iter;
            }            
        }
    }

    void runTest() override
    {
        beginTest("when signal at -6dB with 6dB gain output is at 0dB (1.0f linear)");
        {
            const int numChnls = 2;
            const int numSamples = 16;

            const float minus6dbImpulse = (float) pow(10, -6.0f / 20.0f);
            const float gain_dB = 6.0f;
            const float outExpected = 1.0f;

            /// prepare... create signal
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo bufferToFill(audioBuffer);

            createImpulseSignal(bufferToFill, minus6dbImpulse);

            /// execute...
            std::shared_ptr<MrGain<float>> gain =
                std::shared_ptr<MrGain<float>>(new MrGain<float>());
                        
            gain->setGainDecibels(gain_dB);

            juce::dsp::AudioBlock<float> block(audioBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            gain->process(context);

            /// evaluate...
            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {
                const float* buffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
                expect(buffer[0] == outExpected);
            }
        }            
    }
};

static MrGainTests gainTests;
