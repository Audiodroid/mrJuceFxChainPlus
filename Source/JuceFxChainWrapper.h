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
    const float CUT_OFF_IN_HZ = 500.0f;

    ///reverb
    const float ROOMSIZE = 0.3f;

    JuceFxChainWrapper()
    {
        _pJuceFxChain = std::shared_ptr<FxChain>(new FxChain());                
    }

    void setupFilter(juce::dsp::ProcessSpec& spec)
    {
        _sampleRate = spec.sampleRate;

        setCutOffInHz(CUT_OFF_IN_HZ);
        
        auto& filter = _pJuceFxChain->template get<idxFilter>();
        *filter.state = *FilterCoefs::makeLowPass(_sampleRate, _cutOffInHz, 5.0);

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
        setRoomSize(ROOMSIZE);
    }

    void prepare(juce::dsp::ProcessSpec& spec)
    {
        _pJuceFxChain->prepare(spec);
    }
    
    void process(juce::dsp::ProcessContextReplacing<float> context)
    {
        _pJuceFxChain->process(context);
    }

    void setCutOffInHz(float cutOffInHz)
    {
        _cutOffInHz = cutOffInHz;
        _updateFilterFlag = true;
    }

    float getCutOffInHz()
    {
        return _cutOffInHz;
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

    void setRoomSize(float roomSize)
    {
        _roomSize = roomSize;
        _updateReverbFlag = true;
    }

    float getRoomSize()
    {        
        return _roomSize;
    }

    void updateFilter()
    {
        if (!_updateFilterFlag)
            return;

        auto& filter = _pJuceFxChain->template get<idxFilter>();
        *filter.state = *FilterCoefs::makeLowPass(_sampleRate, _cutOffInHz, 5.0);

        _updateFilterFlag = false;
    }

    void updateReverb()
    {
        if (!_updateReverbFlag)
            return;

        auto& reverb = _pJuceFxChain->template get<idxReverb>();
        auto params = reverb.getParameters();

        params.roomSize = _roomSize;
        reverb.setParameters(params);

        _updateReverbFlag = false;
    }

private:
    
    enum
    {
        idxFilter,
        idxDelay,
        idxReverb
    };

    std::shared_ptr<FxChain> _pJuceFxChain;
    double _sampleRate;
    
    bool _updateFilterFlag = false;
    float _cutOffInHz;

    bool _updateReverbFlag = false;
    float _roomSize;
};
