/*
  ==============================================================================

   This file is code written by Marcel Roth.

   THE CODE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#pragma once

#include <algorithm>

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

		dlyBufs.setSize(numChnls, delayInSmpls + 1);
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

		if (context.isBypassed)
		{
			if (context.usesSeparateInputAndOutputBlocks())
				outBlock.copyFrom(inBlock);

			return;
		}

		posR = writeToOutput(inBlock,
			dlyBufs,
			outBlock,
			posR);

		const FloatType feedbackVal = feedback.getNextValue();
		posW = writeToDelayBuffer(inBlock, dlyBufs, posW, feedbackVal);
	}

	static int writeToOutput(
		const juce::dsp::AudioBlock<const float>& in,
		juce::AudioBuffer<float>& dly,
		juce::dsp::AudioBlock<float>& out,
		int pos)
	{
		int bufSizeDly = (int)dly.getNumSamples();
		int bufSizeIn = (int)in.getNumSamples();

		int numSamplesFromEnd = std::min((int)(bufSizeDly - pos), bufSizeIn);
		int numSamplesFromFront = bufSizeIn - numSamplesFromEnd;

		if (numSamplesFromEnd > 0)
		{
			auto in1 = in.getSubBlock(0, numSamplesFromEnd);
			const juce::dsp::AudioBlock<const float> dly1(dly, pos);
			auto out1 = out.getSubBlock(0, numSamplesFromEnd);
			out1.replaceWithSumOf(in1, dly1);
		}

		if (numSamplesFromFront > 0)
		{
			auto in2 = in.getSubBlock(numSamplesFromEnd, numSamplesFromFront);
			const juce::dsp::AudioBlock<const float> dly2(dly, 0);
			auto out2 = out.getSubBlock(numSamplesFromEnd, numSamplesFromFront);
			out2.replaceWithSumOf(in2, dly2);
		}

		pos += bufSizeIn;
		pos %= bufSizeDly;

		return pos;
	}

	static int writeToDelayBuffer(
		const juce::dsp::AudioBlock<const float>& in,
		juce::AudioBuffer<float>& dly,
		int pos,
		const FloatType feedbackVal)
	{
		int bufSizeDly = dly.getNumSamples();
		int bufSizeIn = in.getNumSamples();

		int numSamplesToEnd = std::min((int)(bufSizeDly - pos), (int)bufSizeIn);
		int numSamplesToFront = bufSizeIn - numSamplesToEnd;

		size_t numChannels = in.getNumChannels();
		for (size_t c = 0; c < numChannels; ++c)
		{
			auto* inData = in.getChannelPointer(c);

			dly.copyFromWithRamp((int)c, pos, inData, numSamplesToEnd, feedbackVal, feedbackVal);
			dly.copyFromWithRamp((int)c, 0, inData + numSamplesToEnd, numSamplesToFront, feedbackVal, feedbackVal);
		}

		pos += bufSizeIn;
		pos %= bufSizeDly;

		return pos;
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
