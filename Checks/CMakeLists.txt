cmake_minimum_required(VERSION 3.16)
project(LanternJourneyChecks CXX)
set(CMAKE_CXX_STANDARD 17)
add_executable(journey_checks journey_checks.cpp ../Source/Lanternfall/Core/Journey.cpp)
target_compile_options(journey_checks PRIVATE -Wall -Wextra -Werror)

