#pragma once

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
    void setDelayInSmpls(size_t delayInSmpls) noexcept { _delayInSmapls = delayInSmpls; }

    ///** Applies new delay as a millisecond value. */
    //void setDelayInMs(FloatType newDelayTime) noexcept { setDelayInSmpls(msToSmpls(newDelatTime)); }

    /** Returns the current delay as a number of samples. */
    size_t getDelayInSmpls() const noexcept { return delayInSmpls; }

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
            setDelayInSmpls(delayInSmpls);
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

        std::vector<float> tmp(len, 0);

        for (size_t chan = 0; chan < numChannels; ++chan)
        {
            auto* src = inBlock.getChannelPointer(chan);
            auto* dst = outBlock.getChannelPointer(chan);

            for (size_t i = 0; i < len; ++i)
            {
                tmp[i] = src[i];

                if (i < _delayInSmapls)
                    dst[i] = 0.0f;
                else
                    dst[i] = tmp[i - _delayInSmapls];
            }
        }                    
    }

private:
    //==============================================================================
    size_t _delayInSmapls = 0;
    double _sampleRate = 0;
};

