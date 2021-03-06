#pragma once

#include <vector>
#include <JuceHeader.h>
#include "MrSignal.h"
#include "MrDelay.h"

class MrDelayTests : public juce::UnitTest
{
public:

    MrDelayTests() : juce::UnitTest("MrDelay testing") {}

    void runTest() override
    {
        beginTest("When feedback is set then delay returns the newly set feedback.");
        {
            float feedbackExpected = 0.7f;

            /// execute...
            auto delay(std::make_unique<MrDelay<float>>());

            delay->setFeedback(feedbackExpected);
            auto feedbackActual = delay->getFeedback();

            /// evaluate...
            expect(feedbackActual == feedbackExpected);
        }

        beginTest("When delay is set in samples then delay returns the newly set value.");
        {
            size_t delayInSmplsExpected = 24000;

            /// execute...
            std::unique_ptr<MrDelay<float>> delay(new MrDelay<float>());

            delay->setDelayInSmpls(delayInSmplsExpected);
            auto delayInSmplsActual = delay->getDelayInSmpls();

            /// evaluate...
            expect(delayInSmplsActual == delayInSmplsExpected);
        }

        beginTest("When delay is set in ms then delay returns the newly set value.");
        {
            double delayInMsExpected = 500;

            /// execute...
            auto delay(std::make_unique<MrDelay<float>>());

            delay->setDelayInMs(delayInMsExpected);
            auto delayInMsActual = delay->getDelayInMs();

            /// evaluate...
            expect(delayInMsActual == delayInMsExpected);
        }

        beginTest("When samplerate doubles then delay in samples doubles.");
        {
            size_t delayInSmpls1st = 24000;
            double sampleRate1st = 48000; //todo: this seems to be the default, needs to looked at again to make this a proper test not relying on defaults
            double sampleRateNew = sampleRate1st * 2;

            size_t delayInSmplsExpected = delayInSmpls1st * 2;

            /// execute...
            auto delay(std::make_unique<MrDelay<float>>());

            juce::dsp::ProcessSpec spec;
            spec.numChannels = 2;
            spec.sampleRate = sampleRate1st;
            spec.maximumBlockSize = 256;
            
            delay->setDelayInSmpls(delayInSmpls1st);
            delay->prepare(spec);
            
            spec.sampleRate = sampleRateNew;
            delay->prepare(spec);

            auto delayInSmplsActual = delay->getDelayInSmpls();

            /// evaluate...
            expect(delayInSmplsActual == delayInSmplsExpected);
        }        

        beginTest("When processing 3 blocks then delay still works as expected");
        {            
            const int numChnls = 2;
            const int numSamplesPerBlock = 2;
            const int numSamplesIn = 6;
            const int numBlocks = numSamplesIn / numSamplesPerBlock;
            const size_t delayInSmpls = 2;
            const float feedback = 0.5f;

            float source[] = { 1, 2, 3, 4, 5, 6 };
            const std::vector<float> outExpected = { 1.0, 2.0, 3.5, 5.0, 6.75, 8.5 };
            const auto deltaExpected = 0.0000001f;

            /// prepare... 
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamplesIn);
            juce::AudioSourceChannelInfo audioSrcChnlInfo(audioBuffer);

            /* creates ramp i.e. (0.1, 0.2, 0.3, 0.4) */
            int numChannels = audioSrcChnlInfo.buffer->getNumChannels();
            for (int c = 0; c < numChannels; ++c)
                audioSrcChnlInfo.buffer->copyFrom(c, 0, source, numSamplesIn);
            
            
            /// execute...
            auto delay(std::make_unique<MrDelay<float>>());

            juce::dsp::ProcessSpec spec;
            spec.numChannels = numChnls;
            spec.maximumBlockSize = numSamplesPerBlock;

            delay->prepare(spec);
            delay->setDelayInSmpls(delayInSmpls);            
            delay->setFeedback(feedback);

            juce::dsp::AudioBlock<float> block(audioBuffer);

            auto offset = 0;
            for (int i = 0; i < numBlocks; ++i, offset += numSamplesPerBlock)
            {
                auto subBlock = block.getSubBlock(offset, numSamplesPerBlock);
                juce::dsp::ProcessContextReplacing<float> context(subBlock);
                delay->process(context);
            }

            /// evaluate...
            for (int channel = 0; channel < audioSrcChnlInfo.buffer->getNumChannels(); ++channel)
            {
                const float* outActual = audioSrcChnlInfo.buffer->getReadPointer(channel, audioSrcChnlInfo.startSample);

                for (int i = 0; i < numSamplesIn; ++i)
                {
                    auto deltaActual = abs(outActual[i] - outExpected[i]);
                    expect(deltaActual < deltaExpected);
                }
            }
        }

        beginTest("When bypassing then output signal is equal to input signal");
        {
            const int numChnls = 2;
            const int numSamples = 4;

            const size_t delayInSmpls = 2;
            const float feedback = 0.5f;

            const std::vector<float> outExpected = { 0.1f, 0.2f, 0.3f, 0.4f }; ///input being (0.1, 0.2, 0.3, 0.4)
            const auto deltaExpected = 0.0000001f;
            auto isBypassed = true;

            /// prepare... 
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo audioSrcChnlInfo(audioBuffer);

            /* creates ramp i.e. (0.1, 0.2, 0.3, 0.4) */
            MrSignal::ramp(audioSrcChnlInfo);

            /// execute...
            auto delay(std::make_unique<MrDelay<float>>());

            juce::dsp::ProcessSpec spec;
            spec.numChannels = numChnls;
            spec.maximumBlockSize = numSamples;

            delay->prepare(spec);
            delay->setDelayInSmpls(delayInSmpls);            
            delay->setFeedback(feedback);

            juce::dsp::AudioBlock<float> block(audioBuffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            
            context.isBypassed = isBypassed;

            delay->process(context);

            /// evaluate...
            for (int channel = 0; channel < audioSrcChnlInfo.buffer->getNumChannels(); ++channel)
            {
                const float* outActual = audioSrcChnlInfo.buffer->getReadPointer(channel, audioSrcChnlInfo.startSample);

                for (int i = 0; i < numSamples; ++i)
                {
                    auto deltaActual = abs(outActual[i] - outExpected[i]);
                    expect(deltaActual < deltaExpected);
                }
            }
        }
    }       
};

static MrDelayTests delayTests;
