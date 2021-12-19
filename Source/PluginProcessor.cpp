/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "JuceFxChainWrapper.h"

#include "mrUnitTestRunner.h"

//==============================================================================
MrJuceFxChainPlusAudioProcessor::MrJuceFxChainPlusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

#if _DEBUG
    MrUnitTestRunner unitTestRunner;
    unitTestRunner.runAllTests();
#endif

    _juceFxChainWrapper = std::shared_ptr<IJuceFxChainWrapper>(new JuceFxChainWrapper());   
}

MrJuceFxChainPlusAudioProcessor::MrJuceFxChainPlusAudioProcessor(std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper)
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    _juceFxChainWrapper = juceFxChainWrapper;
}

MrJuceFxChainPlusAudioProcessor::~MrJuceFxChainPlusAudioProcessor()
{
}

//==============================================================================
const juce::String MrJuceFxChainPlusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MrJuceFxChainPlusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MrJuceFxChainPlusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MrJuceFxChainPlusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MrJuceFxChainPlusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MrJuceFxChainPlusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MrJuceFxChainPlusAudioProcessor::getCurrentProgram()
{
    return 0;
}

JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
void MrJuceFxChainPlusAudioProcessor::setCurrentProgram (int index)
{
}
JUCE_END_IGNORE_WARNINGS_MSVC

JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
const juce::String MrJuceFxChainPlusAudioProcessor::getProgramName (int index)
{
    return {};
}
JUCE_END_IGNORE_WARNINGS_MSVC

JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
void MrJuceFxChainPlusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}
JUCE_END_IGNORE_WARNINGS_MSVC

//==============================================================================
void MrJuceFxChainPlusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    _juceFxChainWrapper->setupFilter(spec);
    _juceFxChainWrapper->setupDelay(spec);
    _juceFxChainWrapper->setupReverb();

    _juceFxChainWrapper->prepare(spec);
}

void MrJuceFxChainPlusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MrJuceFxChainPlusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
void MrJuceFxChainPlusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    _juceFxChainWrapper->updateFilter();
    _juceFxChainWrapper->updateReverb();
    _juceFxChainWrapper->updateDelay();

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    _juceFxChainWrapper->process(context);
}
JUCE_END_IGNORE_WARNINGS_MSVC

//==============================================================================
bool MrJuceFxChainPlusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MrJuceFxChainPlusAudioProcessor::createEditor()
{
    return new MrJuceFxChainPlusAudioProcessorEditor (*this);
}

//==============================================================================
JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
void MrJuceFxChainPlusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}
JUCE_END_IGNORE_WARNINGS_MSVC

JUCE_BEGIN_IGNORE_WARNINGS_MSVC(4100)
void MrJuceFxChainPlusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
JUCE_END_IGNORE_WARNINGS_MSVC

void MrJuceFxChainPlusAudioProcessor::setDelayInMs(double delayInMs)
{
    _juceFxChainWrapper->setDelayInMs(delayInMs);
}

double MrJuceFxChainPlusAudioProcessor::getDelayInMs()
{
    return _juceFxChainWrapper->getDelayInMs();
}

void MrJuceFxChainPlusAudioProcessor::setFeedback(float feedback)
{
    _juceFxChainWrapper->setFeedback(feedback);
}

float MrJuceFxChainPlusAudioProcessor::getFeedback()
{
    return _juceFxChainWrapper->getFeedback();
}

void MrJuceFxChainPlusAudioProcessor::setCutOffInHz(float cutOffInHz)
{        
    _juceFxChainWrapper->setCutOffInHz(cutOffInHz);
}

float MrJuceFxChainPlusAudioProcessor::getCutOffInHz()
{
    return _juceFxChainWrapper->getCutOffInHz();
}

void MrJuceFxChainPlusAudioProcessor::setRoomSize(float roomSize)
{
    _juceFxChainWrapper->setRoomSize(roomSize);
}

float MrJuceFxChainPlusAudioProcessor::getRoomSize()
{
    return _juceFxChainWrapper->getRoomSize();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MrJuceFxChainPlusAudioProcessor();
}

