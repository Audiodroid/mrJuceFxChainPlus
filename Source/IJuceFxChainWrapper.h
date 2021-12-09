#pragma once

#include <JuceHeader.h>

class IJuceFxChainWrapper {

public:

	virtual void prepare(juce::dsp::ProcessSpec& spec) = 0;
	virtual void process(juce::dsp::ProcessContextReplacing<float> context) = 0;
};