#pragma once

#include <JuceHeader.h>

class IJuceFxChainWrapper {

public:
	
	virtual void setupFilter(juce::dsp::ProcessSpec& spec) = 0;
	virtual void setupDelay(juce::dsp::ProcessSpec& spec) = 0;
	virtual void setupReverb() = 0;
	virtual void prepare(juce::dsp::ProcessSpec& spec) = 0;
	virtual void process(juce::dsp::ProcessContextReplacing<float> context) = 0;

	virtual void setDelayInMs(double delayInMs) = 0;
	virtual double getDelayInMs() = 0;

	virtual void setFeedback(float feedback) = 0;
	virtual float getFeedback() = 0;

};