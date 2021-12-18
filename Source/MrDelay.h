#pragma once

#include <cmath>
#include <vector>
#include <iterator>
#include <JuceHeader.h>

/**
    Applies a simple delay to AudioBlocks.
*/
class MrDelay
{
public:
    const float FEEDBACK_LEVEL = 0.5f;

    MrDelay() noexcept = default;
    
    //==============================================================================
    void setFeedback(float feedback) { _feedback.setTargetValue(feedback); }

    /** Applies new delay as number of samples. */
    void setDelayInSmpls(size_t delayInSmpls) noexcept
    {
        _delayInSmpls = delayInSmpls;
        
        for (int i = 0; i < _numChnls; ++i)
        {
            _dlyBufs[i].resize(_delayInSmpls + 1);

            _iterRs[i] = _dlyBufs[i].begin();
            _iterWs[i] = _dlyBufs[i].begin() + delayInSmpls;
        }
    }

    /** Applies new delay as a millisecond value. */
    void setDelayInMs(double delayInMs) noexcept { setDelayInSmpls(msToSmpls(delayInMs)); }

    /** Returns the current delay in a number of samples. */
    size_t getDelayInSmpls() const noexcept { return _delayInSmpls; }

    /** Returns the current delay as a millisecond value. */
    double getDelayInMs() const noexcept { return smplsToMs(getDelayInSmpls()); }

    /** Converts time in ms to samples */
    size_t msToSmpls(double ms) const noexcept { return (size_t) (std::round(ms * _sampleRate) / 1000.0f); }

    /** Converts samples to time in ms*/
    double smplsToMs(size_t smpls) const noexcept { return smpls * 1000 / _sampleRate; }

    //==============================================================================
    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        /// if we already have a delay value set, reset it to fit the new sampleRate
        if ((_delayInSmpls > 0) && (_sampleRate > 0) && spec.sampleRate != _sampleRate)
        {
            auto delayInSmpls = (size_t) (std::round((_delayInSmpls * spec.sampleRate) / _sampleRate));
            setDelayInSmpls(delayInSmpls);
        }

        _sampleRate = spec.sampleRate;
        _numChnls = spec.numChannels;
        
        _dlyBufs.resize(_numChnls);
        _iterRs.resize(_numChnls);
        _iterWs.resize(_numChnls);
    }

    //==============================================================================
    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto&& inBlock = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
        jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

        auto len = inBlock.getNumSamples();
        auto numChannels = inBlock.getNumChannels();

        if (context.isBypassed)
        {            
            if (context.usesSeparateInputAndOutputBlocks())
                outBlock.copyFrom(inBlock);

            return;
        }
               
        for (size_t c = 0; c < numChannels; ++c)
        {
            auto* src = inBlock.getChannelPointer(c);
            auto* dst = outBlock.getChannelPointer(c);

            for (size_t i = 0; i < len; ++i, ++_iterRs[c], ++_iterWs[c])
            {                
                if (_iterRs[c] == _dlyBufs[c].end())
                    _iterRs[c] = _dlyBufs[c].begin();
                
                const float feedback = _feedback.getNextValue();
                dst[i] = src[i] + (feedback * (*_iterRs[c]));

                if (_iterWs[c] == _dlyBufs[c].end())
                    _iterWs[c] = _dlyBufs[c].begin();

                *_iterWs[c] = dst[i];
            }
        }                    
    }

private:
    //==============================================================================
    size_t _delayInSmpls = 0;
    double _sampleRate = 0;
    juce::SmoothedValue<float> _feedback = FEEDBACK_LEVEL;

    int _numChnls = 0;
    std::vector < std::vector<float>> _dlyBufs;
    std::vector < std::vector<float>::const_iterator> _iterRs;
    std::vector < std::vector<float>::iterator> _iterWs;
};


