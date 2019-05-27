include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-13-0
    GIT_SHALLOW    ON
)

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    message(STATUS "Fetching asio")
    FetchContent_Populate(asio)

    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_compile_features(asio INTERFACE cxx_std_11)

    if(WIN32)
        target_link_libraries(asio INTERFACE ws2_32 wsock32) # Link to Winsock
        target_compile_definitions(asio INTERFACE _WIN32_WINNT=0x0601) # Windows 7 and up
    endif()
endif()
