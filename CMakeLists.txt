cmake_minimum_required(VERSION 3.25)

project(
        VrelkHttpClient
        VERSION 0.0.4
        LANGUAGES CXX)
set(PROJECT_LICENSE "MIT")
set(PROJECT_FRIENDLY_NAME ${PROJECT_NAME})
set(PROJECT_LONG_NAME "Template CommonLibSSE-NG")
set(PROJECT_DESCRIPTION ${PROJECT_LONG_NAME})
set(PROJECT_COPYRIGHT "Copyright")

# disable the warnings as errors for MSVC
add_compile_options(/WX-) # disables warnings as errors for MSVC

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(addpluginsources)
include(pluginconfig)
include(plugintarget)
include(pluginpackage)
include("${PROJECT_SOURCE_DIR}/local.cmake" OPTIONAL)

######## dependencies
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED  ON)
find_package(Boost CONFIG REQUIRED COMPONENTS)
if(Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
endif()

# Find and link CPR library
find_package(cpr CONFIG REQUIRED)
target_link_libraries(VrelkHttpClient PRIVATE cpr::cpr)

# Find and link SQLite3 library
find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(VrelkHttpClient PRIVATE SQLiteCpp)

# Configure Crow
find_package(Crow)
target_link_libraries(VrelkHttpClient PUBLIC Crow::Crow)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    USER_AGENT_NAME="${PROJECT_NAME}" 
    USER_AGENT_VERSION="${PROJECT_VERSION}"
)

# find_library(DETOURS_LIBRARY detours REQUIRED)
# target_include_directories(${PROJECT_NAME} PRIVATE ${DETOURS_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${DETOURS_LIBRARY})