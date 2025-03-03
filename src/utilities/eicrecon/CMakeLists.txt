cmake_minimum_required(VERSION 3.16)

print_header(">>>>   E X E C U T A B L E :   eicrecon    <<<<")       # Fancy printing

project(eicrecon_project)

# Find dependencies
find_package(JANA    REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt     REQUIRED)
find_package(ROOT REQUIRED COMPONENTS Core Tree Hist RIO EG)

# Compile all sources into executable
file(GLOB SOURCES *.cpp *.cc *.c  *.hpp *.hh *.h)

set( INCLUDE_DIRS  ${PROJECT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${PROJECT_SOURCE_DIR} ${JANA_INCLUDE_DIR} ${ROOT_INCLUDE_DIRS})
set( LINK_LIBRARIES ${JANA_LIB} ${ROOT_LIBRARIES} ${CMAKE_DL_LIBS} Threads::Threads )

# Define executable
add_executable( eicrecon ${SOURCES} )
target_include_directories( eicrecon PUBLIC ${INCLUDE_DIRS})
target_link_libraries(eicrecon ${LINK_LIBRARIES} )

# Install executable
install(TARGETS eicrecon DESTINATION bin)


