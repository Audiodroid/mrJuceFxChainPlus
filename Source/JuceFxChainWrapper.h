#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"
#include "MrGain.h"
#include "MrDelay.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    const float CUT_OFF_IN_HZ = 500.0f;
    const size_t DELAY_IN_SMPLS = 36000; // 750ms if smprate = 48000

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using FxChain = juce::dsp::ProcessorChain<MrDelay<float>,
                                                juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                                                    juce::dsp::Reverb>;

    JuceFxChainWrapper() : 
        _cutoffInHz(CUT_OFF_IN_HZ), 
        _delayInSmpls(DELAY_IN_SMPLS)
    {
        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());
    }

    void setupFilter(double sampleRate)
    {
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        filter.state = FilterCoefs::makeFirstOrderHighPass(sampleRate, _cutoffInHz);
    }

    void setupDelay(juce::dsp::ProcessSpec& spec)
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        delay.prepare(spec);
        delay.setDelayInSmpls(_delayInSmpls);
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
        idxDelay,
        idxFilter,
        idxReverb
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
    
    float _cutoffInHz;
    size_t _delayInSmpls;
};
