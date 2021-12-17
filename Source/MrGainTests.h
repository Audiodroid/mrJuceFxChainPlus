#pragma once

#include <vector>
#include <JuceHeader.h>
#include "MrSignal.h"
#include "MrGain.h"

class MrGainTests : public juce::UnitTest
{
public:

    MrGainTests() : juce::UnitTest("mrGain testing") {}

    void runTest() override
    {
        beginTest("when signal at -6dB with 6dB gain then output is at 0dB (1.0f linear)");
        {
            const int numChnls = 2;
            const int numSamples = 16;

            const float minus6dbImpulse = (float) pow(10, -6.0f / 20.0f);
            const float gain_dB = 6.0f;
            const float outExpected = 1.0f;

            /// prepare... create signal
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo bufferToFill(audioBuffer);

            MrSignal::impulse(bufferToFill, minus6dbImpulse);

            /// execute...
            std::shared_ptr<MrGain<float>> gain =
                std::shared_ptr<MrGain<float>>(new MrGain<float>());
                        
            gain->setDelayInSmpls(gain_dB);

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
