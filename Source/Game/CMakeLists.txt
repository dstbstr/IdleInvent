project(IdleGame LANGUAGES CXX)

if(WIN32)
    add_executable(${PROJECT_NAME})
elseif(ANDROID)
    APK_ADD_EXECUTABLE(${PROJECT_NAME})
    #add_library(${PROJECT_NAME} SHARED)
endif()

target_link_libraries(${PROJECT_NAME} 
    Engine
    InventLib
    IdlePlatform
)

target_sources(${PROJECT_NAME} PRIVATE
    src/Main.cpp
)

# TODO: Copy all assets to the build directory
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
elseif(ANDROID)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/assets ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_apk/src/main/assets
    )
endif()