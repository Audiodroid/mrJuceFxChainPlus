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
        beginTest("when stereo delay with 2-samples-delay-time then as of 3rd sample delay comes in on both channels");
        {
            const int numChnls = 2;
            const int numSamples = 4;
            
            const size_t delayInSmpls = 2;
            const float feedback = 0.5f;

            const std::vector<float> outExpected = {0.1f, 0.2f, 0.35f, 0.5f}; ///input being (0.1, 0.2, 0.3, 0.4)
            const auto deltaExpected = 0.0000001f;

            /// prepare... create ramp i.e. (0.1, 0.2, 0.3, 0.4)
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo audioSrcChnlInfo(audioBuffer);

            MrSignal::ramp(audioSrcChnlInfo);

            /// execute...
            std::shared_ptr<MrDelay> delay =
                std::shared_ptr<MrDelay>(new MrDelay());

            juce::dsp::ProcessSpec spec;
            spec.numChannels = numChnls;
            delay->prepare(spec);

            delay->setDelayInSmpls(delayInSmpls);
            delay->setFeedback(feedback);

            juce::dsp::AudioBlock<float> block(audioBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            delay->process(context);

            /// evaluate...
            for (int channel = 0; channel < audioSrcChnlInfo.buffer->getNumChannels(); ++channel)
            {
                const float* outActual = audioSrcChnlInfo.buffer->getReadPointer(channel, audioSrcChnlInfo.startSample);
                
                for(int i = 0; i < numSamples; ++i)
                {
                    auto deltaActual = outActual[i] - outExpected[i];
                    expect(deltaActual < deltaExpected);
                }
            }
        }
    }
};

static MrDelayTests delayTests;
