
#include "gtest/gtest.h"
#include "PluginProcessor.h"
#include "IJuceFxChainWrapper.h"
#include "JuceFxChainWrapper.h"

const int NUM_CHNLS = 2;
const int NUM_SMPLS = 2048;

TEST(MrJuceFxChainPlusAudioProcessor_IntegratedTests, WhenProcessingAudioData_ThenItChanges) {

	auto diracImpulse = 1.0f;
	auto minSamplesNotZeroBeforeProcessingExpected = 1;
	auto minSamplesNotZeroAfterProcessingExpected = 10;

	/// prepare
	std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
		std::shared_ptr<IJuceFxChainWrapper>(new JuceFxChainWrapper());

	/// create data with a "simple" dirac impulse
	float* dataAllChnls[NUM_CHNLS] = {};
	for (int i = 0; i < NUM_CHNLS; ++i)
	{
		float dataForChnl[NUM_SMPLS] = {};
		dataForChnl[0] = diracImpulse;
		dataAllChnls[i] = dataForChnl;
	}
	float* const* dataToReferTo = &dataAllChnls[0];
	juce::AudioBuffer<float> audioBuffer(dataToReferTo,
		NUM_CHNLS,
		NUM_SMPLS);

	for(int i = 0; i < NUM_CHNLS; ++i)
	{
		ASSERT_EQ(dataToReferTo[i][0], diracImpulse);

		auto cntSamplesNotZeroBefore = 0;
		for (int j = 0; j < NUM_SMPLS; ++j)
		{
			if (dataToReferTo[i][j] != 0.0f)
				++cntSamplesNotZeroBefore;			
		}
		ASSERT_EQ(cntSamplesNotZeroBefore, minSamplesNotZeroBeforeProcessingExpected);
	}

	/// exercise
	MrJuceFxChainPlusAudioProcessor mrJuceFxChainPlusAudioProcessor(juceFxChainWrapper);

	auto sampleRate = 48000;
	auto samplesPerBlock = 512;

	mrJuceFxChainPlusAudioProcessor.prepareToPlay(sampleRate, samplesPerBlock);
	
	juce::MidiBuffer midiBuffer;	
	mrJuceFxChainPlusAudioProcessor.processBlock(audioBuffer, midiBuffer);

	/// evaluate
	for (int i = 0; i < NUM_CHNLS; ++i)
	{
		/// dirac impulse "weakened" due to effect(s)
		ASSERT_LT(dataToReferTo[i][0], diracImpulse);

		auto cntSamplesNotZeroAfter = 0;
		for (int j = 0; j < NUM_SMPLS; ++j)
		{
			/// we expect at least 10 samples to be not zero due to added effect(s)
			if (dataToReferTo[i][j] != 0.0f)
				++cntSamplesNotZeroAfter;			
		}
		ASSERT_GT(cntSamplesNotZeroAfter, minSamplesNotZeroAfterProcessingExpected);
	}
}
