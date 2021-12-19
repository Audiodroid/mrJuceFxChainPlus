/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MrJuceFxChainPlusAudioProcessorEditor::MrJuceFxChainPlusAudioProcessorEditor (MrJuceFxChainPlusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 170);

    createSlider(_sliderCutOffInHz, STR_CUT_OFF_IN_HZ, 100.0, 20000.0, 50.0);
    auto cutoffInHz = audioProcessor.getCutOffInHz();
    _sliderCutOffInHz.setValue(cutoffInHz);

    createSlider(_sliderDelay, STR_DELAY, 0.0, 2000.0, 1.0);
    auto delayInMs = audioProcessor.getDelayInMs();
    _sliderDelay.setValue(delayInMs);

    createSlider(_sliderFeedback, STR_FEEDBACK, 0.0, 1.0, 0.01);
    auto feedback = audioProcessor.getFeedback();
    _sliderFeedback.setValue(feedback);

    createSlider(_sliderRoomSize, STR_ROOM_SIZE, 0.0, 1.0, 0.01);
    auto roomsize = audioProcessor.getRoomSize();
    _sliderRoomSize.setValue(roomsize);
}

MrJuceFxChainPlusAudioProcessorEditor::~MrJuceFxChainPlusAudioProcessorEditor()
{
}

void MrJuceFxChainPlusAudioProcessorEditor::createSlider(juce::Slider &slider, const std::string &name, double min, double max, double step)
{
    slider.setName(name);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(min, max, step);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextValueSuffix(juce::String(" " + name));

    addAndMakeVisible(&slider);

    slider.addListener(this);
}

//==============================================================================
void MrJuceFxChainPlusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    g.drawFittedText("LP Cutoff [Hz]", 10, 10, 100, 20, juce::Justification::top, 1);
    g.drawFittedText("Delay Time [ms]", 10, 30, 100, 20, juce::Justification::top, 1);
    g.drawFittedText("Feedback [0..1]", 10, 50, 100, 20, juce::Justification::top, 1);
    g.drawFittedText("Roomsize [0..1]", 10, 70, 100, 20, juce::Justification::top, 1);
}

void MrJuceFxChainPlusAudioProcessorEditor::resized()
{
    _sliderCutOffInHz.setBounds(130, 10, getWidth() - 150, 20);
    _sliderDelay.setBounds(130, 30, getWidth() - 150, 20);
    _sliderFeedback.setBounds(130, 50, getWidth() - 150, 20);
    _sliderRoomSize.setBounds(130, 70, getWidth() - 150, 20);
}

void MrJuceFxChainPlusAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{    
    auto name = slider->getName();
    
    if (name.compare(STR_CUT_OFF_IN_HZ) == 0)
        audioProcessor.setCutOffInHz((float)slider->getValue());
    else if(name.compare(STR_DELAY) == 0)
        audioProcessor.setDelayInMs(slider->getValue());
    else if(name.compare(STR_FEEDBACK) == 0)
        audioProcessor.setFeedback((float) slider->getValue());
    else if (name.compare(STR_ROOM_SIZE) == 0)
        audioProcessor.setRoomSize((float)slider->getValue());
}

