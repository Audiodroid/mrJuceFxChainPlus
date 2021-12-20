﻿# mrJuceFxChainPlus

# build
This implementation was made using VS2022 community. an exporter to linux makefile was added and the software compiled and ran under linux (ubuntu) as well. Adding a xcode exporter is work in progress.

# paths
The projects currently point to paths as they can be found on my local machine, e.g. the juce modules.
Open the projucer project to change these paths to what are the appropriate ones on your machine.

# tests
All tests have been changed and are now using the juce unittesting library. When in debug mode test run before the start of the application.

# todos
The application has been extended and now runs with a gui interface allowing for parameters to be changed. Changes are being applied inbetween processing blocks for thread-safty reasons. This part of the implementation has not been covered by unittests so far. Ideally this should be the case due to a TDD approach but unfamilarity with the JUCE libraries lead to a few twists and turns during the implementation and this would have been difficult to juggle with unittests. Clearly this is the next step to take.

The "handmade" delay implementation could to with further improvement to smoothing parameter changes.

Run on a ubuntu system the sound quality is not the same as on windows. This may need another look at.

As the time of writting the code has not been run on a MacOs system. This is at the top of the todo list.
