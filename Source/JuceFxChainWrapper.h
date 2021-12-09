#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    using FxChain = juce::dsp::ProcessorChain<juce::dsp::Reverb>;

    JuceFxChainWrapper() {

        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());
    }

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        _pJuceFxChain->prepare(spec);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float> context)
    {
        _pJuceFxChain->process(context);
    }

private:

    std::shared_ptr<FxChain> _pJuceFxChain;
};