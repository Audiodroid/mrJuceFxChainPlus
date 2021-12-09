#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        _pJuceFxChain->prepare(spec);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float> context)
    {
        _pJuceFxChain->process(context);
    }

private:

    std::shared_ptr<juce::dsp::ProcessorChain<>> _pJuceFxChain;
};