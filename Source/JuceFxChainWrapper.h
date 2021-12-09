#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using FxChain = juce::dsp::ProcessorChain<juce::dsp::Reverb,
                        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>>;

    JuceFxChainWrapper() : 
        _cutoffInHz(200.0f)
    {

        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());
    }

    void setupFilter(double sampleRate)
    {
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        filter.state = FilterCoefs::makeFirstOrderLowPass(sampleRate, _cutoffInHz);
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
    
    enum
    {
        idxReverb,
        idxFilter,
        idxChorus
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
    
    float _cutoffInHz;
};