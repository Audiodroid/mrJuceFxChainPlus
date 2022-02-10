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
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

            delay->setFeedback(feedbackExpected);
            auto feedbackActual = delay->getFeedback();

            /// evaluate...
            expect(feedbackActual == feedbackExpected);
        }

        beginTest("When delay is set in samples then delay returns the newly set value.");
        {
            size_t delayInSmplsExpected = 24000;

            /// execute...
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

            delay->setDelayInSmpls(delayInSmplsExpected);
            auto delayInSmplsActual = delay->getDelayInSmpls();

            /// evaluate...
            expect(delayInSmplsActual == delayInSmplsExpected);
        }

        beginTest("When delay is set in ms then delay returns the newly set value.");
        {
            double delayInMsExpected = 500;

            /// execute...
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

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
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

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

        beginTest("When stereo delay with 2-samples-delay-time then as of 3rd sample delay comes in on both channels");
        {
            const int numChnls = 2;
            const int numSamples = 4;

            const size_t delayInSmpls = 2;
            const float feedback = 0.5f;

            const std::vector<float> outExpected = { 0.1f, 0.2f, 0.35f, 0.5f }; ///input being (0.1, 0.2, 0.3, 0.4)
            const auto deltaExpected = 0.0000001f;

            /// prepare... 
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo audioSrcChnlInfo(audioBuffer);

            /* creates ramp i.e. (0.1, 0.2, 0.3, 0.4) */
            MrSignal::ramp(audioSrcChnlInfo);

            /// execute...
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

            juce::dsp::ProcessSpec spec;
            spec.numChannels = numChnls;
            spec.maximumBlockSize = numSamples;

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

                for (int i = 0; i < numSamples; ++i)
                {
                    auto deltaActual = outActual[i] - outExpected[i];
                    expect(deltaActual < deltaExpected);
                }
            }
        }

        beginTest("When processing 2 blocks then delay still works as expected");
        {
            const int numChnls = 2;
            const int numSamplesPerBlock = 2;
            const int numSamples = 4;
            const int numBlocks = numSamples / numSamplesPerBlock;
            const size_t delayInSmpls = 2;
            const float feedback = 0.5f;

            const std::vector<float> outExpected = { 0.1f, 0.2f, 0.35f, 0.5f }; ///input being (0.1, 0.2, 0.3, 0.4)
            const auto deltaExpected = 0.0000001f;

            /// prepare... 
            juce::AudioBuffer<float> audioBuffer(numChnls, numSamples);
            juce::AudioSourceChannelInfo audioSrcChnlInfo(audioBuffer);

            /* creates ramp i.e. (0.1, 0.2, 0.3, 0.4) */
            MrSignal::ramp(audioSrcChnlInfo);

            /// execute...
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

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

                for (int i = 0; i < numSamples; ++i)
                {
                    auto deltaActual = outActual[i] - outExpected[i];
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
            std::shared_ptr<MrDelay<float>> delay =
                std::shared_ptr<MrDelay<float>>(new MrDelay<float>());

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
                    auto deltaActual = outActual[i] - outExpected[i];
                    expect(deltaActual < deltaExpected);
                }
            }
        }
    }       
};

static MrDelayTests delayTests;
