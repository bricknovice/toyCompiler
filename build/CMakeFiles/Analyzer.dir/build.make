# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/joshua/toyCompiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/joshua/toyCompiler/build

# Include any dependencies generated for this target.
include CMakeFiles/Analyzer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Analyzer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Analyzer.dir/flags.make

CMakeFiles/Analyzer.dir/analyzer.cpp.o: CMakeFiles/Analyzer.dir/flags.make
CMakeFiles/Analyzer.dir/analyzer.cpp.o: ../analyzer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/joshua/toyCompiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Analyzer.dir/analyzer.cpp.o"
	/usr/local/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Analyzer.dir/analyzer.cpp.o -c /home/joshua/toyCompiler/analyzer.cpp

CMakeFiles/Analyzer.dir/analyzer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Analyzer.dir/analyzer.cpp.i"
	/usr/local/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/joshua/toyCompiler/analyzer.cpp > CMakeFiles/Analyzer.dir/analyzer.cpp.i

CMakeFiles/Analyzer.dir/analyzer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Analyzer.dir/analyzer.cpp.s"
	/usr/local/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/joshua/toyCompiler/analyzer.cpp -o CMakeFiles/Analyzer.dir/analyzer.cpp.s

# Object files for target Analyzer
Analyzer_OBJECTS = \
"CMakeFiles/Analyzer.dir/analyzer.cpp.o"

# External object files for target Analyzer
Analyzer_EXTERNAL_OBJECTS =

libAnalyzer.so: CMakeFiles/Analyzer.dir/analyzer.cpp.o
libAnalyzer.so: CMakeFiles/Analyzer.dir/build.make
libAnalyzer.so: CMakeFiles/Analyzer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/joshua/toyCompiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libAnalyzer.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Analyzer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Analyzer.dir/build: libAnalyzer.so

.PHONY : CMakeFiles/Analyzer.dir/build

CMakeFiles/Analyzer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Analyzer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Analyzer.dir/clean

CMakeFiles/Analyzer.dir/depend:
	cd /home/joshua/toyCompiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/joshua/toyCompiler /home/joshua/toyCompiler /home/joshua/toyCompiler/build /home/joshua/toyCompiler/build /home/joshua/toyCompiler/build/CMakeFiles/Analyzer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Analyzer.dir/depend

