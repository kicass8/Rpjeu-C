# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = C:\Users\alexa\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\212.5457.51\bin\cmake\win\bin\cmake.exe

# The command to remove a file.
RM = C:\Users\alexa\AppData\Local\JetBrains\Toolbox\apps\CLion\ch-0\212.5457.51\bin\cmake\win\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\alexa\Desktop\Rpjeu-C

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Projet_C.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/Projet_C.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Projet_C.dir/flags.make

CMakeFiles/Projet_C.dir/main.c.obj: CMakeFiles/Projet_C.dir/flags.make
CMakeFiles/Projet_C.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/Projet_C.dir/main.c.obj"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-W\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\Projet_C.dir\main.c.obj -c C:\Users\alexa\Desktop\Rpjeu-C\main.c

CMakeFiles/Projet_C.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/Projet_C.dir/main.c.i"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-W\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\alexa\Desktop\Rpjeu-C\main.c > CMakeFiles\Projet_C.dir\main.c.i

CMakeFiles/Projet_C.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/Projet_C.dir/main.c.s"
	C:\PROGRA~1\MINGW-~1\X86_64~1.0-W\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\alexa\Desktop\Rpjeu-C\main.c -o CMakeFiles\Projet_C.dir\main.c.s

# Object files for target Projet_C
Projet_C_OBJECTS = \
"CMakeFiles/Projet_C.dir/main.c.obj"

# External object files for target Projet_C
Projet_C_EXTERNAL_OBJECTS =

Projet_C.exe: CMakeFiles/Projet_C.dir/main.c.obj
Projet_C.exe: CMakeFiles/Projet_C.dir/build.make
Projet_C.exe: CMakeFiles/Projet_C.dir/linklibs.rsp
Projet_C.exe: CMakeFiles/Projet_C.dir/objects1.rsp
Projet_C.exe: CMakeFiles/Projet_C.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable Projet_C.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\Projet_C.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Projet_C.dir/build: Projet_C.exe
.PHONY : CMakeFiles/Projet_C.dir/build

CMakeFiles/Projet_C.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\Projet_C.dir\cmake_clean.cmake
.PHONY : CMakeFiles/Projet_C.dir/clean

CMakeFiles/Projet_C.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\alexa\Desktop\Rpjeu-C C:\Users\alexa\Desktop\Rpjeu-C C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug C:\Users\alexa\Desktop\Rpjeu-C\cmake-build-debug\CMakeFiles\Projet_C.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Projet_C.dir/depend

