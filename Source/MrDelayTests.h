#pragma once

#include <vector>
#include <JuceHeader.h>
#include "MrSignal.h"
#include "MrDelay.h"

class MrDelayTests : public juce::UnitTest
{
public:

    MrDelayTests() : juce::UnitTest("mrDelay testing") {}

    void runTest() override
    {
        beginTest("when just delaying by one then impulse moved by one");
        {
            const int numChnls = 1;
            const int numSamples = 16;

            const float impulse = 1.0f;
            const size_t delayInSmpls = 1;
            const float outExpected = impulse;

            /// prepare... create signal
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo bufferToFill(audioBuffer);

            MrSignal::impulse(bufferToFill, impulse);

            /// execute...
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

            delay->setDelayInSmpls(delayInSmpls);

            juce::dsp::AudioBlock<float> block(audioBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            delay->process(context);

            /// evaluate...
            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {
                const float* buffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
                expect(buffer[delayInSmpls] == outExpected);
            }
        }
    }
};

static MrDelayTests delayTests;
