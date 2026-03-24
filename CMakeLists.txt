CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE_SOURCES
)

if (SC_UNI_BUILD)
    set(SCRIPTS_CURRENT_LOADER "${CMAKE_CURRENT_SOURCE_DIR}/src/mod_lfg_all_dungeons.cpp")
endif()

CU_ADD_HOOK(GAME_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_sources(game PRIVATE ${PRIVATE_SOURCES})
