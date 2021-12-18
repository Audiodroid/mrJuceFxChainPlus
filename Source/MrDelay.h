#pragma once

#include <vector>
#include <iterator>

/**
    Applies a simple delay to AudioBlocks.
*/
template <typename FloatType>
class MrDelay
{
public:

    MrDelay() noexcept = default;
    
    //==============================================================================
    /** Applies new delay as number of samples. */
    void setDelayInSmpls(size_t delayInSmpls) noexcept
    {
        _delayInSmpls = delayInSmpls;
        
        _dlyBuf.resize(_delayInSmpls+1);

        _iterR = _dlyBuf.begin();
        _iterW = _dlyBuf.begin() + delayInSmpls;
    }

    ///** Applies new delay as a millisecond value. */
    //void setDelayInMs(FloatType newDelayTime) noexcept { setDelayInSmpls(msToSmpls(newDelatTime)); }

    /** Returns the current delay as a number of samples. */
    size_t getDelayInSmpls() const noexcept
    {
        return _delayInSmpls;
    }

    ///** Returns the current delay as a millisecond value. */
    //FloatType getDelayInMs() const noexcept { return smplsToMs(getDelayInSmpls()); }

    ///** Converts time to samples */
    //size_t msToSmpls(FloatType ms) { return (ms * sampleRate) / 1000 ; }

    ///** Converts samples to time */
    //FloatType smplsToMs(size_t smpls) { return smpls * 1000 / sampleRate; }

    //==============================================================================
    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        _sampleRate = spec.sampleRate;
        reset();
    }

    /** Resets the internal state of the delay */
    void reset() noexcept
    {
        if (_sampleRate > 0)
            setDelayInSmpls(_delayInSmpls);
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
               
        for (size_t chan = 0; chan < numChannels; ++chan)
        {
            auto* src = inBlock.getChannelPointer(chan);
            auto* dst = outBlock.getChannelPointer(chan);

            for (size_t i = 0; i < len; ++i, ++_iterR, ++_iterW)
            {                
                if (_iterR == _dlyBuf.end())
                    _iterR = _dlyBuf.begin();
                
                dst[i] = src[i] + (_feedback * (*_iterR));

                if (_iterW == _dlyBuf.end())
                    _iterW = _dlyBuf.begin();

                *_iterW = dst[i];
            }
        }                    
    }

private:
    //==============================================================================
    size_t _delayInSmpls = 0;
    double _sampleRate = 0;
    FloatType _feedback = 0.5f;

    std::vector<FloatType> _dlyBuf;

    typename std::vector<FloatType>::const_iterator _iterR;
    typename std::vector<FloatType>::iterator _iterW;
};

