
#include "gtest/gtest.h"
#include "PluginProcessor.h"
#include "IJuceFxChainWrapper.h"
#include "JuceFxChainWrapperMock.h"

using ::testing::AtLeast;
TEST(MrJuceFxChainPlusAudioProcessor_UnitTests, WhenPrepareToPlayIsCalled_ThenPrepareOfWrapperIsCalled) {

	/// prepare
	auto juceFxChainWrapperMock = new JuceFxChainWrapperMock();

	std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
		std::shared_ptr<IJuceFxChainWrapper>(juceFxChainWrapperMock);
	
	EXPECT_CALL(*juceFxChainWrapperMock, setupFilter)
		.Times(AtLeast(1));

	EXPECT_CALL(*juceFxChainWrapperMock, setupGain)
		.Times(AtLeast(1));

	EXPECT_CALL(*juceFxChainWrapperMock, prepare)
		.Times(AtLeast(1));
	
	/// exercise
	MrJuceFxChainPlusAudioProcessor mrJuceFxChainPlusAudioProcessor(juceFxChainWrapper);
	
	auto sampleRate = 48000;
	auto samplesPerBlock = 512;
	mrJuceFxChainPlusAudioProcessor.prepareToPlay(sampleRate, samplesPerBlock);

	/// evaluate	
	/*...see expect-calls above*/
}

TEST(MrJuceFxChainPlusAudioProcessor_UnitTests, WhenProcessBlockIsCalled_ThenProcessOfWrapperIsCalled) {

	/// prepare
	auto juceFxChainWrapperMock = new JuceFxChainWrapperMock();

	std::shared_ptr<IJuceFxChainWrapper> juceFxChainWrapper =
		std::shared_ptr<IJuceFxChainWrapper>(juceFxChainWrapperMock);

	EXPECT_CALL(*juceFxChainWrapperMock, process)
		.Times(AtLeast(1));

	/// exercise
	MrJuceFxChainPlusAudioProcessor mrJuceFxChainPlusAudioProcessor(juceFxChainWrapper);

	juce::AudioSampleBuffer buffer;
	juce::MidiBuffer midiBuffer;
	mrJuceFxChainPlusAudioProcessor.processBlock(buffer, midiBuffer);

	/// evaluate	
	/*...see expect-calls above*/
}