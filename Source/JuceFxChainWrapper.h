#pragma once

#include <JuceHeader.h>
#include "IJuceFxChainWrapper.h"
#include "MrDelay.h"

class JuceFxChainWrapper : public IJuceFxChainWrapper {

public:

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using FxChain = juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>, 
                                                MrDelay<float>,                                                
                                                    juce::dsp::Reverb>;
    ///delay
    const double DELAY_IN_MS = 750;
    const float FEEDBACK = 0.5f;

    ///filter
    const float CUT_OFF_IN_HZ = 200.0f;

    ///reverb
    const float ROOMSIZE = 0.3f;

    JuceFxChainWrapper()
    {
        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());                
    }

    void setupFilter(juce::dsp::ProcessSpec& spec)
    {
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        filter.state = FilterCoefs::makeFirstOrderLowPass(spec.sampleRate, CUT_OFF_IN_HZ);

        filter.prepare(spec);
        filter.reset();
    }

    void setupDelay(juce::dsp::ProcessSpec& spec)
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        delay.prepare(spec);

        delay.setDelayInMs(DELAY_IN_MS);
        delay.setFeedback(FEEDBACK);
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

    void setFeedback(float feedback)
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        delay.setFeedback(feedback);
    }

    float getFeedback()
    {
        auto& delay = _pJuceFxChain->template get<idxDelay>();
        return delay.getFeedback();
    }

private:
    
    enum
    {
        idxFilter,
        idxDelay,
        idxReverb
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
};
