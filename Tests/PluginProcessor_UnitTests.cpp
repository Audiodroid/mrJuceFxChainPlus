
#include "gtest/gtest.h"
#include "PluginProcessor.h"
#include "IJuceFxChainWrapper.h"
#include "JuceFxChainWrapperMock.h"

using ::testing::AtLeast;
TEST(MrJuceFxChainPlusAudioProcessor_Tests, WhenProcessCalled_ThenProcessOfWrapperCalled) {

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