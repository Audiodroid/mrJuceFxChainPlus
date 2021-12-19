#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "JuceFxChainWrapperMock.h"

class PluginProcessorTests : public juce::UnitTest
{
public:

    PluginProcessorTests() : juce::UnitTest("PluginProcessor testing") {}

    void runTest() override
    {
        beginTest("When prepareToPlay is called then all functions to setup the chain are being called.");
        {
            /// prepare
            std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
                std::shared_ptr<IJuceFxChainWrapper>(new JuceFxChainWrapperMock());

            MrJuceFxChainPlusAudioProcessor pluginProcessor(juceFxChainWrapper);

            /// exercise
            double sampleRate = 48000;
            int samplesPerBlock = 265;
            pluginProcessor.prepareToPlay(sampleRate, samplesPerBlock);

            /// evaluate
            std::shared_ptr<JuceFxChainWrapperMock> mock =
                std::static_pointer_cast<JuceFxChainWrapperMock>(juceFxChainWrapper);

            expect(mock->atLeastOneCallToFunction("setupDelay"));
            expect(mock->atLeastOneCallToFunction("setupFilter"));
            expect(mock->atLeastOneCallToFunction("setupReverb"));
            expect(mock->atLeastOneCallToFunction("prepare"));
        }

        beginTest("When processesBlock is called then process of the chain is called.");
        {
            /// prepare
            std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
                std::shared_ptr<IJuceFxChainWrapper>(new JuceFxChainWrapperMock());

            MrJuceFxChainPlusAudioProcessor pluginProcessor(juceFxChainWrapper);

            /// exercise            
            juce::AudioSampleBuffer audioSampleBuffer;
            juce::MidiBuffer midiBuffer;
            pluginProcessor.processBlock(audioSampleBuffer, midiBuffer);
            
            /// evaluate
            std::shared_ptr<JuceFxChainWrapperMock> mock =
                std::static_pointer_cast<JuceFxChainWrapperMock>(juceFxChainWrapper);

            expect(mock->atLeastOneCallToFunction("process"));
        }
    }
};

static PluginProcessorTests pluginProcessorTests;