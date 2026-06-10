project(IdleGame LANGUAGES CXX)

set(GAME "GhostHunter" CACHE STRING "Which game to build")
set_property(CACHE GAME PROPERTY STRINGS GhostHunter TechMaze Invent) # for GUI dropdowns

if(GAME STREQUAL "GhostHunter")
    set(GAME_LIB GhostHunterLib)
    set(GAME_HEADER "GhostHunter/GhostHunterGame.h")
    set(GAME_TYPE GhostHunter::GhostHunterGame)
elseif(GAME STREQUAL "TechMaze")
    set(GAME_LIB TechMazeLib)
    set(GAME_HEADER "TechMaze/TechMazeGame.h")
    set(GAME_TYPE TechMaze::TechMazeGame)
elseif(GAME STREQUAL "Invent")
    set(GAME_LIB InventLib)
    set(GAME_HEADER "Invent/InventGame.h")
    set(GAME_TYPE Invent::InventGame)
else()
    message(FATAL_ERROR "Invalid game selected: ${GAME}")
endif()

set(GAME_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/${GAME})
set(SHARED_ASSET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/Shared)
if(NOT EXISTS ${GAME_ASSET_DIR})
    message(FATAL_ERROR "Game asset directory does not exist: ${GAME_ASSET_DIR}")
endif()
message(STATUS "Building game: ${GAME} (lib=${GAME_LIB})")

if(WIN32)
    add_executable(${PROJECT_NAME})
elseif(ANDROID)
    APK_ADD_EXECUTABLE(${PROJECT_NAME})
endif()

target_link_libraries(${PROJECT_NAME} 
    Engine
    ${GAME_LIB}
    IdlePlatform
)

target_sources(${PROJECT_NAME} PRIVATE
    src/Main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GAME_HEADER="${GAME_HEADER}" # quoted for #include
    GAME_TYPE=${GAME_TYPE} # bare token for using alias
)

if(WIN32)
    set(ASSET_DEST ${CMAKE_CURRENT_BINARY_DIR}/assets)
elseif(ANDROID)
    set(ASSET_DEST ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_apk/src/main/assets)
endif()

add_custom_target(SyncAssets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${SHARED_ASSET_DIR} ${ASSET_DEST}
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${GAME_ASSET_DIR} ${ASSET_DEST}
    VERBATIM
)

add_dependencies(${PROJECT_NAME} SyncAssets)
# Clear out assets on a Clean to avoid stale assets
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${ASSET_DEST})