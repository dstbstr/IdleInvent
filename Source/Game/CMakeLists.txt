project(IdleGame LANGUAGES CXX)

function(add_game_exe target game_lib game_header game_type)
    if(WIN32)
        add_executable(${target})
        set(asset_dest "${CMAKE_CURRENT_BINARY_DIR}/assets")
    elseif(ANDROID)
        APK_ADD_EXECUTABLE(${target})
        set(asset_dest "${CMAKE_CURRENT_BINARY_DIR}/${target}_apk/src/main/assets")
    endif()

    target_sources(${target} PRIVATE src/Main.cpp)
    target_link_libraries(${target} PRIVATE
        Engine
        ${game_lib}
        IdlePlatform
    )

    target_compile_definitions(${target} PRIVATE
        ${compiler_definitions}
        GAME_HEADER="${game_header}"
        GAME_TYPE=${game_type}
    )

    set(game_asset_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/${target}")
    set(shared_asset_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/Shared")
    set(sync_target "${target}SyncAssets")
    if(NOT EXISTS "${game_asset_dir}")
        message(FATAL_ERROR "Game asset directory does not exist: ${game_asset_dir}")
    endif()

    add_custom_target(${sync_target}
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${shared_asset_dir} ${asset_dest}
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${game_asset_dir} ${asset_dest}
        VERBATIM
    )

    add_dependencies(${target} ${sync_target})
    set_property(TARGET ${target} APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${asset_dest})
endfunction()

add_game_exe(GhostHunter GhostHunterLib "GhostHunter/GhostHunterGame.h" GhostHunter::GhostHunterGame)
add_game_exe(TechMaze TechMazeLib "TechMaze/TechMazeGame.h" TechMaze::TechMazeGame)
add_game_exe(Invent InventLib "Invent/InventGame.h" Invent::InventGame)
add_game_exe(PetPyramid PetPyramidLib "Pets/PetPyramidGame.h" Pets::PetPyramidGame)