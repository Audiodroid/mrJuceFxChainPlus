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

    The code is meant to follow the JUCE coding standard
    https://juce.com/discover/stories/coding-standards
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
    void setFeedback(FloatType feedbackNew)
    {
        feedback.setTargetValue(feedbackNew);
    }

    /* Returns the current feedback value. */
    FloatType getFeedback() { return feedback.getTargetValue(); }

    /** Applies new delay as number of samples. */
    void setDelayInSmpls(size_t delayInSmplsNew) noexcept
    {
        delayInSmpls = delayInSmplsNew;

        if (dlyBufs == nullptr)
            return;

        dlyBufs->setSize(numChnls, (int) delayInSmpls + 1);
        dlyBufs->clear();

        posR = 0;
        posW = delayInSmpls;
    }

    /** Applies new delay as a millisecond value. */
    void setDelayInMs(FloatType delayInMs) noexcept { setDelayInSmpls(msToSmpls(delayInMs)); }

    /** Returns the current delay in a number of samples. */
    size_t getDelayInSmpls() const noexcept { return delayInSmpls; }

    /** Returns the current delay as a millisecond value. */
    FloatType getDelayInMs() const noexcept { return smplsToMs(getDelayInSmpls()); }

    /** Converts time in ms to samples */
    size_t msToSmpls(FloatType ms) const noexcept { return (size_t) (std::round(ms * sampleRate) / 1000.0f); }

    /** Converts samples to time in ms*/
    FloatType smplsToMs(size_t smpls) const noexcept { return (sampleRate != 0) ? (smpls * 1000 / sampleRate) : 0.0; }

    //==============================================================================
    /** Called before processing starts. */
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        if (sampleRate <= 0)
            return;
        
        auto delayInSmplsNew = (size_t) (std::round((delayInSmpls * spec.sampleRate) / sampleRate));
        setDelayInSmpls(delayInSmplsNew);
       
        sampleRate = spec.sampleRate;
        numChnls = spec.numChannels;
        
        dlyBufs = std::unique_ptr< juce::AudioBuffer<FloatType>>(new juce::AudioBuffer<FloatType>(numChnls, (int) delayInSmpls+1));
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

        int dlyBufsLen = dlyBufs->getNumSamples();
        for (size_t pos = 0; pos < len; ++pos, ++posR, ++posW)
        {
            if (posR >= dlyBufsLen)
                posR = 0;

            if (posW >= dlyBufsLen)
                posW = 0;

            for (size_t c = 0; c < numChannels; ++c)
            {
                auto* src = inBlock.getChannelPointer(c);
                auto* dst = outBlock.getChannelPointer(c);
                            
                const FloatType fb = feedback.getNextValue();
                auto dly = dlyBufs->getSample(c, posR);
                dst[pos] = src[pos] + (fb * dly);

                dlyBufs->setSample(c, posW, dst[pos]);
            }            
        }                    
    }

private:

    //==============================================================================
    size_t delayInSmpls = 0;
    FloatType sampleRate = SAMPLERATE_DEFAULT;
    juce::SmoothedValue<FloatType> feedback = FEEDBACK_DEFAULT;

    int numChnls = 0;

    /// These vectors need to be replaced byt something that excepts FloatType
    std::unique_ptr<juce::AudioBuffer<FloatType>> dlyBufs;
    
    int posR;
    int posW;
};
