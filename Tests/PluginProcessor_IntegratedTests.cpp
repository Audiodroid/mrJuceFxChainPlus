
#include "gtest/gtest.h"
#include "PluginProcessor.h"
#include "IJuceFxChainWrapper.h"
#include "JuceFxChainWrapper.h"

const int NUM_CHNLS = 2;
const int NUM_SMPLS = 2048;

TEST(MrJuceFxChainPlusAudioProcessor_IntegratedTests, WhenProcessingAudioData_ThenItChanges) {

	/// prepare
	std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
		std::shared_ptr<IJuceFxChainWrapper>(new JuceFxChainWrapper());

	/// create data with a "simple" dirac impulse
	float* dataAllChnls[NUM_CHNLS] = {};
	for (int i = 0; i < NUM_CHNLS; ++i)
	{
		float dataForChnl[NUM_SMPLS] = {};
		dataForChnl[0] = 1.0f;
		dataAllChnls[i] = dataForChnl;
	}
	float* const* dataToReferTo = &dataAllChnls[0];
	juce::AudioBuffer<float> audioBuffer(dataToReferTo,
		NUM_CHNLS,
		NUM_SMPLS);

	for(int i = 0; i < NUM_CHNLS; ++i)
	{
		ASSERT_EQ(dataToReferTo[i][0], 1.0f);

		auto cntSamplesNotZeroBefore = 0;
		for (int j = 1; j < NUM_SMPLS; ++j)
		{
			if (dataToReferTo[i][j] != 0.0f)
				++cntSamplesNotZeroBefore;			
		}
		ASSERT_EQ(cntSamplesNotZeroBefore, 0);
	}

	/// exercise
	MrJuceFxChainPlusAudioProcessor mrJuceFxChainPlusAudioProcessor(juceFxChainWrapper);

	auto sampleRate = 48000;
	auto samplesPerBlock = 512;
	auto minSamplesNotZeroAfterExpected = 10;

	mrJuceFxChainPlusAudioProcessor.prepareToPlay(sampleRate, samplesPerBlock);
	
	juce::MidiBuffer midiBuffer;	
	mrJuceFxChainPlusAudioProcessor.processBlock(audioBuffer, midiBuffer);

	/// evaluate
	for (int i = 0; i < NUM_CHNLS; ++i)
	{
		/// dirac impulse "weakened" due to effect(s)
		ASSERT_LT(dataToReferTo[i][0], 1.0f);

		auto cntSamplesNotZeroAfter = 0;
		for (int j = 1; j < NUM_SMPLS; ++j)
		{
			/// we expect at least 10 samples to be not zero due to added effect(s)
			if (dataToReferTo[i][j] != 0.0f)
				++cntSamplesNotZeroAfter;			
		}
		ASSERT_GT(cntSamplesNotZeroAfter, minSamplesNotZeroAfterExpected);
	}
}
