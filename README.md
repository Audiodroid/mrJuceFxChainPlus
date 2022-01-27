# mrJuceFxChainPlus

# Build / Platforms

## Windows
This implementation was mainly made using VS2022 community. A projucer exporter for Visual Studio 2019 was used and worked successfully.

## Linux
Using the LinuxMakefile exporter this project also build on Linux (Ubuntu), however there were sound quality issues.

## MacOS
An XCode exporter has also been added to the projucer project. This project has also been build and tested on MacOS Monterey and no issues have been found.

## Note
The last commit has been made from a MacOS system. From (short time) experience...switching to another platform might need a revisit of the projucer project and saving the desired exporter for that system/platform again.

# Paths
The projects currently point to paths as they can be found on my local machine(s), e.g. the juce modules.
Open the projucer project to change these paths to the appropriate ones on your machine.

# Tests
All tests are now using the juce unittesting library. When in debug mode tests run before the start of the application.

# Todos
The application has been extended and now runs with a gui interface allowing for parameters to be changed. Changes are being applied inbetween processing blocks for thread-safty reasons. This part of the implementation has not been covered by unittests so far. Ideally this should be the case due to a TDD approach but unfamilarity with the JUCE libraries lead to a few twists and turns during the implementation and this would have been difficult to juggle with unittests. Clearly adding test to cover this added code is the next step to take.

The "handmade" delay implementation could to with further improvement to smoothing parameter changes.

Running on a Ubuntu/Linux system the sound quality is not the same as on Windows or MacOS. This may need another look at.

