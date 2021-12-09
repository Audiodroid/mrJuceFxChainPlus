#pragma once

#include <JuceHeader.h>
#include "gmock/gmock.h"

#include "IJuceFxChainWrapper.h"

class JuceFxChainWrapperMock : public IJuceFxChainWrapper {

public:
    MOCK_METHOD(void, setupFilter, (double sampleRate), (override));
    MOCK_METHOD(void, setupGain, (juce::dsp::ProcessSpec& spec), (override));
    MOCK_METHOD(void, prepare, (juce::dsp::ProcessSpec& spec), (override));
    MOCK_METHOD(void, process, (juce::dsp::ProcessContextReplacing<float> context), (override));
};