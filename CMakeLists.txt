# CMakeList.txt : CMake project for discord-asm, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("discord-asm")

# Add source to this project's executable.
add_executable (discord-asm "discord-asm.cpp" "discord-asm.h" "botclient.h" "botclient.cpp"  "util.h"
 "util.cpp" "instruction_base.h"  "instructions/mov.h" "instructions/mov.cpp" "mymemory.h"  "definitions.h" 
 "processor.h" "processor.cpp" "program_environment.h" "program_environment.cpp"   "instructions/cmp.h" "instructions/cmp.cpp" 
 "instructions/jne.h" "instructions/jne.cpp" "instructions/jmp.h" "instructions/jmp.cpp" "instructions/call.h" 
 "instructions/call.cpp" "instructions/ret.h" "instructions/ret.cpp" "instructions/prtc.h" "instructions/prtc.cpp" 
 "instructions/add.h" "instructions/add.cpp"   "client_tasks.h" "client_tasks.cpp" "instructions/pusha.h" 
 "instructions/pusha.cpp" "instructions/popa.h" "instructions/popa.cpp" "user_settings.h"  "database_handler.h" "environment_settings.h")

# Add tests and install targets if needed.

add_subdirectory(deps/sleepy-discord)
target_link_libraries(discord-asm sleepy-discord)

target_compile_features(discord-asm PUBLIC cxx_std_20)