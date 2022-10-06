cmake_minimum_required (VERSION 3.12)

project (discord-asm VERSION 1.0.0 DESCRIPTION "Discord Assembly")

execute_process(
    COMMAND bash -c "pkg-config dpp --libs"
    OUTPUT_VARIABLE DPP_LIBS
)
list(TRANSFORM DPP_LIBS REPLACE "\n" "")

set(D_ASM_SOURCES
    discord-asm.hpp
    discord-asm.cpp
    botclient.hpp
    botclient.cpp
    util.hpp
    util.cpp
    instruction_base.hpp
    instructions/mov.hpp
    instructions/mov.cpp
    mymemory.hpp
    definitions.hpp
    processor.hpp
    processor.cpp
    program_environment.hpp
    program_environment.cpp
    instructions/cmp.hpp
    instructions/cmp.cpp
    instructions/jne.hpp
    instructions/jne.cpp
    instructions/jmp.hpp
    instructions/jmp.cpp
    instructions/call.hpp
    instructions/call.cpp
    instructions/ret.hpp
    instructions/ret.cpp
    instructions/prtc.hpp
    instructions/prtc.cpp
    instructions/add.hpp
    instructions/add.cpp
    client_tasks.hpp
    client_tasks.cpp
    instructions/pusha.hpp
    instructions/pusha.cpp
    instructions/popa.hpp
    instructions/popa.cpp
    instructions/prtn.hpp
    instructions/prtn.cpp
    user_settings.hpp
    database_handler.hpp
    environment_settings.hpp
)
list (TRANSFORM D_ASM_SOURCES PREPEND "src/")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DPP_LIBS}") 

# Add source to this project's executable.
add_executable(discord-asm ${D_ASM_SOURCES})
