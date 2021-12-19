#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"
#include "MrDelay.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using FxChain = juce::dsp::ProcessorChain<MrDelay<float>,
                                                juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                                                    juce::dsp::Reverb>;
    ///delay
    const double DELAY_IN_MS = 750;
    const float FEEDBACK = 0.5f;

    ///filter
    const float CUT_OFF_IN_HZ = 500.0f;

    ///reverb
    const float ROOMSIZE = 0.3f;

    JuceFxChainWrapper()
    {
        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());                
    }

    void setupDelay(juce::dsp::ProcessSpec& spec)
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        delay.prepare(spec);

        delay.setDelayInMs(DELAY_IN_MS);
        delay.setFeedback(FEEDBACK);
    }

    void setupFilter(double sampleRate)
    {
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        filter.state = FilterCoefs::makeFirstOrderHighPass(sampleRate, CUT_OFF_IN_HZ);
    }
    
    void setupReverb()
    {
        auto& reverb = _pJuceFxChain->template get<idxReverb>();
        auto params = reverb.getParameters();
        
        params.roomSize = ROOMSIZE;
        reverb.setParameters(params);
    }

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        _pJuceFxChain->prepare(spec);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float> context)
    {
        _pJuceFxChain->process(context);
    }

    void setDelayInMs(double delayInMs)
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        delay.setDelayInMs(delayInMs);
    }

    double getDelayInMs()
    { 
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        return delay.getDelayInMs();
    };


private:
    
    enum
    {
        idxDelay,
        idxFilter,
        idxReverb
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
};
