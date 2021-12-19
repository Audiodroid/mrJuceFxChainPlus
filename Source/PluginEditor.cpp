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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // these define the parameters of our slider object
    _sliderDelay.setName(STR_DELAY);
    _sliderDelay.setSliderStyle(juce::Slider::LinearHorizontal);
    _sliderDelay.setRange(0.0, 2000.0, 1.0);
    _sliderDelay.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    _sliderDelay.setPopupDisplayEnabled(true, false, this);
    _sliderDelay.setTextValueSuffix(juce::String(" " + STR_DELAY));
    
    auto delayInMs = audioProcessor.getDelayInMs();
    _sliderDelay.setValue(delayInMs);

    // this function adds the slider to the editor
    addAndMakeVisible(&_sliderDelay);

    // add the listener to the slider
    _sliderDelay.addListener(this);
}

MrJuceFxChainPlusAudioProcessorEditor::~MrJuceFxChainPlusAudioProcessorEditor()
{
}

//==============================================================================
void MrJuceFxChainPlusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    
    std::string str = "Delay Time in ms";
    
    g.drawFittedText (str, getLocalBounds(), juce::Justification::top, 1);
}

void MrJuceFxChainPlusAudioProcessorEditor::resized()
{
    // sets the position and size of the slider with arguments (x, y, width, height)
    _sliderDelay.setBounds(10, 200, getWidth() - 20, 20);
}

void MrJuceFxChainPlusAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{    
    auto name = slider->getName();
    
    if(name.compare(STR_DELAY) == 0)
        audioProcessor.setDelayInMs(slider->getValue());
}

