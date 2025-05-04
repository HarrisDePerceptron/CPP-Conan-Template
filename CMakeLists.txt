cmake_minimum_required(VERSION 3.20)
project(HttpClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -fdiagnostics-color=always
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Applying secure Release flags")

  # Compiler optimization and hardening flags
  set(CMAKE_CXX_FLAGS_RELEASE
    "-O3 -march=native -flto=auto -DNDEBUG \
     -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
     -fno-omit-frame-pointer -fno-common"
    CACHE STRING "Release compiler flags" FORCE)

  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
    "-flto=auto -Wl,-z,relro,-z,now"
    CACHE STRING "Release linker flags" FORCE)


  message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
  message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")



endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Applying debug development flags")

  set(CMAKE_CXX_FLAGS_DEBUG
      "-O0 -g3 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -fsanitize=address,undefined -fno-omit-frame-pointer"
      CACHE STRING "Debug compiler flags" FORCE)

  set(CMAKE_EXE_LINKER_FLAGS_DEBUG
      "-fsanitize=address,undefined"
      CACHE STRING "Debug linker flags" FORCE)



  message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
  message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")


endif()

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O2 -g -march=native -DNDEBUG -fstack-protector-strong -fno-omit-frame-pointer"
    CACHE STRING "RelWithDebInfo flags" FORCE)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
  message(STATUS ">> Coverage flags enabled")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
  else()
    message(WARNING "Coverage is only supported with GCC or Clang")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)


# Source files (excluding test and mock)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
foreach(f ${SRC_FILES})
  if (f MATCHES ".*/(test|tests|mock)/.*")
    list(REMOVE_ITEM SRC_FILES ${f})
  endif()
endforeach()
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.hpp src/*.hpp)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBS})

enable_testing()
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(tests ${TEST_FILES})
target_link_libraries(tests PRIVATE ${PROJECT_LIBS} ${TEST_LIBS})
target_include_directories(tests PRIVATE include)
include(CTest)
include(Catch)
catch_discover_tests(tests)

