# Sumeet's guide to build test cases

# Step 1
Create test file under the test folder
Make sure the test file has a .cpp extension to it

# Step 2
Make sure to have these includes at the top of the test file: 
- #include "gtest/gtest.h"
- #include "gmock/gmock.h"
- #include the header file that you are testing at the top

# Step 3
- Write your tests

# Step 4
In the CMakeLists.txt that is in the test directory (test/CMakeLists.txt) do the following:
- under 'add_executable', add in your new test.cpp file
- under target_link_libraries, add the folder name where the .cpp/.h files are located in the lib directory (web-socket-networking/lib). Example: I added networking folder to the target_link_libraries as the 2 files I was testing at the time (Client.h and Server.h) reside in the networking folder

# Step 5
To run test cases: 
 - Navigate to build folder
 - Type in make
 - Type in ./bin/runAllTests