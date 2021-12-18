/*
  ==============================================================================

  We use this so we don't get all the testfile headers in the production code

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

/* add more test files down here*/
#include "MrDelayTests.h"

class MrUnitTestRunner : public juce::UnitTestRunner {

};