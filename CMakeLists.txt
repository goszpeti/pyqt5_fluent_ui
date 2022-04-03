project("FluentWindow")
cmake_minimum_required(VERSION 3.22)
if ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo.cmake)
elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
elseif ( EXISTS ${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/../conanbuildinfo_multi.cmake)
else()
    message(FATAL_ERROR "Conan build info can't be found.")
endif()
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

# C++ standard settings:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)#
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core)
set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/autogen)

set(LIB_NAME "FluentWindow")

set(SOURCES
    src/fluent_window.cpp 
    src/fluent_window.h
    src/fluent.qrc
)

add_library(${LIB_NAME} SHARED
    ${SOURCES}
)

set_target_properties(${LIB_NAME} PROPERTIES VERSION 1.0.0)

# for export symbols
target_compile_definitions(${LIB_NAME}
    PRIVATE FLW_LIBRARY
)

# Make all non-exported symbols hidden by default
set_target_properties(${LIB_NAME}
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}
)

# Export the library's public header path to dependent targets
target_include_directories(${LIB_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${LIB_NAME}
    PUBLIC
    # Qt5::Core
    # Qt5::Widgets
    # Qt5::Gui
    CONAN_PKG::qt

)

install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

install(TARGETS ${_target} EXPORT dev_essential_targets)

add_executable(${LIB_NAME}_EXAMPLE
    src/main.cpp
)

target_include_directories(${LIB_NAME}_EXAMPLE PRIVATE
    $<$<CONFIG:RELWITHDEBINFO>:${CMAKE_CURRENT_BINARY_DIR}/include_RelWithDebInfo>
    $<$<CONFIG:RELEASE>:${CMAKE_CURRENT_BINARY_DIR}/include_Release>

)

target_link_libraries(${LIB_NAME}_EXAMPLE
    ${LIB_NAME}
    CONAN_PKG::qt
)

add_custom_command( TARGET ${LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CONAN_QT_ROOT}/bin/Qt5Core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CONAN_QT_ROOT}/bin/Qt5Gui${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CONAN_QT_ROOT}/bin/Qt5Widgets${CMAKE_SHARED_LIBRARY_SUFFIX}
            $<TARGET_FILE_DIR:${LIB_NAME}>
        )

if (MSVC)
add_custom_command(TARGET ${LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory() 
    "$<TARGET_FILE_DIR:${LIB_NAME}>/platforms"
)
add_custom_command( TARGET ${LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CONAN_QT_ROOT}/bin/archdatadir/plugins/platforms/qwindows.dll
            $<TARGET_FILE_DIR:${LIB_NAME}>/platforms/
        )
endif()