/*
  ==============================================================================

   This file is code written by Marcel Roth.   
   
   THE CODE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#pragma once

#include <cmath>
#include <vector>
#include <iterator>
#include <JuceHeader.h>

/**
    Applies a simple delay to AudioBlocks.
*/
template <typename FloatType>
class MrDelay
{
public:
    const FloatType SAMPLERATE_DEFAULT = 48000;
    const FloatType FEEDBACK_DEFAULT = 0.5f;

    MrDelay() noexcept = default;
    
    //==============================================================================
    /* Applies a new feedback value to delay. */
    void setFeedback(FloatType feedback) { _feedback.setTargetValue(feedback); }

    /* Returns the current feedback value. */
    FloatType getFeedback() { return _feedback.getTargetValue(); }

    /** Applies new delay as number of samples. */
    void setDelayInSmpls(size_t delayInSmpls) noexcept
    {
        _delayInSmpls = delayInSmpls;

        if (_dlyBufs == nullptr)
            return;

        _dlyBufs->setSize(_numChnls, _delayInSmpls + 1);
        _dlyBufs->clear();

        _posR = 0;
        _posW = delayInSmpls;
    }

    /** Applies new delay as a millisecond value. */
    void setDelayInMs(FloatType delayInMs) noexcept { setDelayInSmpls(msToSmpls(delayInMs)); }

    /** Returns the current delay in a number of samples. */
    size_t getDelayInSmpls() const noexcept { return _delayInSmpls; }

    /** Returns the current delay as a millisecond value. */
    FloatType getDelayInMs() const noexcept { return smplsToMs(getDelayInSmpls()); }

    /** Converts time in ms to samples */
    size_t msToSmpls(FloatType ms) const noexcept { return (size_t) (std::round(ms * _sampleRate) / 1000.0f); }

    /** Converts samples to time in ms*/
    FloatType smplsToMs(size_t smpls) const noexcept { return (_sampleRate != 0) ? (smpls * 1000 / _sampleRate) : 0.0; }

    //==============================================================================
    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        if (_sampleRate <= 0)
            return;
        
        auto delayInSmpls = (size_t) (std::round((_delayInSmpls * spec.sampleRate) / _sampleRate));
        setDelayInSmpls(delayInSmpls);
       
        _sampleRate = spec.sampleRate;
        _numChnls = spec.numChannels;
        
        _dlyBufs = std::unique_ptr< juce::AudioBuffer<FloatType>>(new juce::AudioBuffer<FloatType>(_numChnls, (int) delayInSmpls+1));
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

        int dlyBufsLen = _dlyBufs->getNumSamples();
        for (size_t pos = 0; pos < len; ++pos, ++_posR, ++_posW)
        {
            if (_posR >= dlyBufsLen)
                _posR = 0;

            if (_posW >= dlyBufsLen)
                _posW = 0;

            for (size_t c = 0; c < numChannels; ++c)
            {
                auto* src = inBlock.getChannelPointer(c);
                auto* dst = outBlock.getChannelPointer(c);
                            
                const FloatType feedback = _feedback.getNextValue();
                auto dly = _dlyBufs->getSample(c, _posR);
                dst[pos] = src[pos] + (feedback * dly);

                _dlyBufs->setSample(c, _posW, dst[pos]);
            }            
        }                    
    }

private:

    //==============================================================================
    size_t _delayInSmpls = 0;
    FloatType _sampleRate = SAMPLERATE_DEFAULT;
    juce::SmoothedValue<FloatType> _feedback = FEEDBACK_DEFAULT;

    int _numChnls = 0;

    /// These vectors need to be replaced byt something that excepts FloatType
    std::unique_ptr<juce::AudioBuffer<FloatType>> _dlyBufs;
    
    int _posR;
    int _posW;
};
