set(EXE_SOURCES "${EXECUTABLE_NAME}.cpp" "cycle.cpp")
set(EXE_HEADERS "cycle.hpp")
add_executable(noctoui ${EXE_SOURCES} ${EXE_HEADERS})

target_link_libraries(
    noctoui
    PRIVATE impl
            ${LIBRARY_NAME}
            fmt::fmt
            cxxopts::cxxopts
            nlohmann_json::nlohmann_json)

target_set_warnings(
    TARGET
    noctoui
    ENABLE
    ${ENABLE_WARNINGS}
    AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS})

if(${ENABLE_LTO})
    target_enable_lto(noctoui optimized)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_clang_tidy_msvc_to_target(${LIBRARY_NAME})
endif()
