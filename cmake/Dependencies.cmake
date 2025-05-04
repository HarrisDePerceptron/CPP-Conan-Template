find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Catch2 REQUIRED)
find_package(Crow REQUIRED)

# Define a global variable for shared project libraries
set(PROJECT_LIBS
    CURL::libcurl
    nlohmann_json::nlohmann_json
    Crow::Crow
)

# Catch2 is linked only to test targets
set(TEST_LIBS
    Catch2::Catch2WithMain
)

# Optional message for debug
message(STATUS "Linked libraries: ${PROJECT_LIBS}")
