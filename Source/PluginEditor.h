/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MrJuceFxChainPlusAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                    private juce::Slider::Listener
{
public:
    MrJuceFxChainPlusAudioProcessorEditor (MrJuceFxChainPlusAudioProcessor&);
    ~MrJuceFxChainPlusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    const std::string STR_DELAY = "Delay";
    const std::string STR_FEEDBACK = "Feedback";

    void createSlider(juce::Slider& slider, const std::string& name, double min, double max, double step);
    void sliderValueChanged(juce::Slider* slider) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MrJuceFxChainPlusAudioProcessor& audioProcessor;

    juce::Slider _sliderDelay;
    juce::Slider _sliderFeedback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MrJuceFxChainPlusAudioProcessorEditor)
};
