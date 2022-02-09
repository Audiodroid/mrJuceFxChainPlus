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

		reset();
	}

	void reset() {

		dlyBufs.setSize(numChnls, (int)delayInSmpls + 1);
		dlyBufs.clear();

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
	size_t msToSmpls(FloatType ms) const noexcept { return (size_t)(std::round(ms * sampleRate) / 1000.0f); }

	/** Converts samples to time in ms*/
	FloatType smplsToMs(int smpls) const noexcept { return (sampleRate != 0) ? (smpls * 1000 / sampleRate) : 0.0; }

	//==============================================================================
	/** Called before processing starts. */
	void prepare(const juce::dsp::ProcessSpec& spec) noexcept
	{
		if (sampleRate <= 0)
			return;

		auto delayInSmplsNew = static_cast<size_t>(std::round((delayInSmpls * spec.sampleRate) / sampleRate));

		sampleRate = spec.sampleRate;
		numChnls = spec.numChannels;

		setDelayInSmpls(delayInSmplsNew);
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

		auto bufSizeIn = inBlock.getNumSamples();
		auto numChannels = inBlock.getNumChannels();

		if (context.isBypassed)
		{
			if (context.usesSeparateInputAndOutputBlocks())
				outBlock.copyFrom(inBlock);

			return;
		}

		int bufSizeDly = dlyBufs.getNumSamples();

		for (int c = 0; c < numChannels; ++c)
		{
			auto* src = inBlock.getChannelPointer(c);
			auto* dst = outBlock.getChannelPointer(c);
			auto* dly = dlyBufs.getReadPointer(c);

			auto tmp = posR;
			for (size_t pos = 0; pos < bufSizeIn; ++pos, ++tmp)
			{
				if (tmp >= bufSizeDly)
					tmp = 0;

				const FloatType fb = feedback.getNextValue();
				dst[pos] = src[pos] + (fb * dly[tmp]);
			}
		}

		posR += bufSizeIn;
		posR = (posR < bufSizeDly) ? posR : posR - bufSizeDly;

		for (size_t pos = 0; pos < bufSizeIn; ++pos, ++posW)
		{
			if (posW >= bufSizeDly)
				posW = 0;

			for (int c = 0; c < numChannels; ++c)
			{
				auto* src = inBlock.getChannelPointer(c);
				auto* dst = outBlock.getChannelPointer(c);											

				dlyBufs.setSample(c, posW, dst[pos]);
			}
		}
	}	

private:

	//==============================================================================
	int delayInSmpls{ 0 };
	FloatType sampleRate{ SAMPLERATE_DEFAULT };
	juce::SmoothedValue<FloatType> feedback{ FEEDBACK_DEFAULT };

	int numChnls{ 0 };

	juce::AudioBuffer<float> dlyBufs;

	int posR;
	int posW;
};
