#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    const float CUT_OFF_IN_HZ = 200.0f;
    const float GAIN_IN_DB = 6.0f;

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using FxChain = juce::dsp::ProcessorChain<juce::dsp::Reverb,
                        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                            juce::dsp::Gain<float>>;

    JuceFxChainWrapper() : 
        _cutoffInHz(CUT_OFF_IN_HZ), _gain_dB(GAIN_IN_DB)
    {
        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());
    }

    void setupFilter(double sampleRate)
    {
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        filter.state = FilterCoefs::makeFirstOrderLowPass(sampleRate, _cutoffInHz);
    }

    void setupGain(juce::dsp::ProcessSpec& spec)
    {
        auto& gain = _pJuceFxChain->template get<idxGain>();
        gain.prepare(spec);
        gain.setGainDecibels(_gain_dB);
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
        idxGain
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
    
    float _cutoffInHz;
    float _gain_dB;
};