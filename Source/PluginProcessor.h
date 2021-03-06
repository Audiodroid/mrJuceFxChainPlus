/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

//==============================================================================
/**
*/
class MrJuceFxChainPlusAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MrJuceFxChainPlusAudioProcessor();
    MrJuceFxChainPlusAudioProcessor(std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper);
    ~MrJuceFxChainPlusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void setDelayInMs(double delayInMs);
    double getDelayInMs();

    void setFeedback(float feedback);
    float getFeedback();

    void setCutOffInHz(float cutOffInHz);
    float getCutOffInHz();

    void setRoomSize(float roomSize);
    float getRoomSize();

private:
    
    std::shared_ptr<IJuceFxChainWrapper> _juceFxChainWrapper;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MrJuceFxChainPlusAudioProcessor)
};
