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
    
    std::string str = "*** mrJuceFxChainPlus ***\n\n";
    str += "Reverb + LowPass@200Hz + 6dB-Gain\n\n";
    str += "made by Marcel Roth aka mr";

    g.drawFittedText (str, getLocalBounds(), juce::Justification::centred, 1);
}

void MrJuceFxChainPlusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
