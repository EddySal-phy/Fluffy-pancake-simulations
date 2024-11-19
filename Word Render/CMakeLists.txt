cmake_minimum_required(VERSION 3.10)
project(ParticleTextAnimation)

# Specify C++ version
set(CMAKE_CXX_STANDARD 11)

# Optionally download SFML automatically
include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.5.1 # Use a stable version tag
)
FetchContent_MakeAvailable(SFML)

# Add executables for main.cpp and main2.cpp
add_executable(ParticleTextAnimation main.cpp)
add_executable(ParticleTextAnimation2 main2.cpp)

# Include SFML for both executables
target_link_libraries(ParticleTextAnimation PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(ParticleTextAnimation2 PRIVATE sfml-graphics sfml-window sfml-system)

# Copy heart.png and ariali.ttf to the build directory
foreach(EXEC ParticleTextAnimation ParticleTextAnimation2)
  add_custom_command(
    TARGET ${EXEC} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/heart.png
            $<TARGET_FILE_DIR:${EXEC}>/heart.png
  )
  add_custom_command(
    TARGET ${EXEC} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/ariali.ttf
            $<TARGET_FILE_DIR:${EXEC}>/ariali.ttf
  )
endforeach()
