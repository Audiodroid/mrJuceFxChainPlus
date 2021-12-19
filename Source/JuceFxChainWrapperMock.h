/*
  ==============================================================================

   This file is code written by Marcel Roth.

   THE CODE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#pragma once

#include <vector>
#include <string>

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

/**
	This is a quick hack to have somewhat of a mock for unittesting
*/
class JuceFxChainWrapperMock : public IJuceFxChainWrapper {

public:

	void setupDelay(juce::dsp::ProcessSpec& spec) { _log.push_back(__func__); }
	void setupFilter(double sampleRate) { _log.push_back(__func__); }
	void setupReverb() { _log.push_back(__func__); }
	void prepare(juce::dsp::ProcessSpec& spec) { _log.push_back(__func__); }
	void process(juce::dsp::ProcessContextReplacing<float> context) { _log.push_back(__func__); }

	bool atLeastOneCallToFunction(char *cfunc)
	{
		std::string func(cfunc);
		for (std::string entry : _log)
		{
			if (entry.compare(func) == 0)
				return true;
		}

		return false;
	}

private:

	std::vector<std::string> _log;
};