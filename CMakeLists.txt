cmake_minimum_required (VERSION 3.12)

project (discord-asm VERSION 1.0.0 DESCRIPTION "Discord Assembly")

execute_process(
    COMMAND bash -c "pkg-config dpp --libs"
    OUTPUT_VARIABLE DPP_LIBS
)
list(TRANSFORM DPP_LIBS REPLACE "\n" "")

set(D_ASM_SOURCES
    discord-asm.cpp
    discord-asm.h
    botclient.h
    botclient.cpp
    util.h
    util.cpp
    instruction_base.h
    instructions/mov.h
    instructions/mov.cpp
    mymemory.h
    definitions.h
    processor.h
    processor.cpp
    program_environment.h
    program_environment.cpp
    instructions/cmp.h
    instructions/cmp.cpp
    instructions/jne.h
    instructions/jne.cpp
    instructions/jmp.h
    instructions/jmp.cpp
    instructions/call.h
    instructions/call.cpp
    instructions/ret.h
    instructions/ret.cpp
    instructions/prtc.h
    instructions/prtc.cpp
    instructions/add.h
    instructions/add.cpp
    client_tasks.h
    client_tasks.cpp
    instructions/pusha.h
    instructions/pusha.cpp
    instructions/popa.h
    instructions/popa.cpp
    user_settings.h
    database_handler.h
    environment_settings.h
)
list (TRANSFORM D_ASM_SOURCES PREPEND "src/")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DPP_LIBS}") 

# Add source to this project's executable.
add_executable(discord-asm ${D_ASM_SOURCES})
