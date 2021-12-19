#pragma once

#include <JuceHeader.h>

class IJuceFxChainWrapper {

public:
	
	virtual void setupDelay(juce::dsp::ProcessSpec& spec) = 0;
	virtual void setupFilter(double sampleRate) = 0;
	virtual void setupReverb() = 0;
	virtual void prepare(juce::dsp::ProcessSpec& spec) = 0;
	virtual void process(juce::dsp::ProcessContextReplacing<float> context) = 0;

	virtual void setDelayInMs(double delayInMs) = 0;
	virtual double getDelayInMs() = 0;
};