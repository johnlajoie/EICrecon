cmake_minimum_required(VERSION 3.16)

set(PLUGIN_NAME "algorithms_tracking")
print_header(">>>>   P L U G I N :   ${PLUGIN_NAME}    <<<<")       # Fancy printing

# Function creates ${PLUGIN_NAME}_plugin and ${PLUGIN_NAME}_library targets
# Setting default includes, libraries and installation paths
plugin_add(${PLUGIN_NAME} WITH_STATIC_LIBRARY)

# The macro grabs sources as *.cc *.cpp *.c and headers as *.h *.hh *.hpp
# Then correctly sets sources for ${_name}_plugin and ${_name}_library targets
# Adds headers to the correct installation directory
plugin_glob_all(${PLUGIN_NAME})

plugin_sources(${PLUGIN_NAME} JugBase/BField/DD4hepBField.cc)

# Find dependencies
plugin_add_dd4hep(${PLUGIN_NAME})
plugin_add_acts(${PLUGIN_NAME})
plugin_add_cern_root(${PLUGIN_NAME})
plugin_add_event_model(${PLUGIN_NAME})
find_package(Eigen3 REQUIRED)

# The macro grabs sources as *.cc *.cpp *.c and headers as *.h *.hh *.hpp
# Then correctly sets sources for ${_name}_plugin and ${_name}_library targets
# Adds headers to the correct installation directory
plugin_glob_all(${PLUGIN_NAME})

# Add include directories (works same as target_include_directories)
# plugin_include_directories(${PLUGIN_NAME} SYSTEM PUBLIC ... )

# Add libraries
# (same as target_include_directories but for both plugin and library)
plugin_link_libraries(${PLUGIN_NAME} Eigen3::Eigen)

#
## Find dependencies
#find_package(EDM4HEP REQUIRED)
#find_package(podio REQUIRED)
#find_package(ROOT REQUIRED COMPONENTS Core Tree Hist RIO EG)
#find_package(EDM4EIC REQUIRED)
#find_package(spdlog REQUIRED)
#find_package(Eigen3 REQUIRED)
#
#plugin_add_dd4hep(${PLUGIN_NAME})
#plugin_add_acts(${PLUGIN_NAME})
#
## Add include directories (works same as target_include_directories)
#plugin_include_directories(${PLUGIN_NAME}
#            PUBLIC
#                ${CMAKE_CURRENT_LIST_DIR}
#            SYSTEM PUBLIC
#                ${podio_INCLUDE_DIR}
#                ${EDM4HEP_INCLUDE_DIR}
#                ${ROOT_INCLUDE_DIRS}
#                ${EDM4EIC_INCLUDE_DIRS})
#
## Add libraries (works same as target_include_directories)
#plugin_link_libraries(${PLUGIN_NAME}
#        ${ROOT_LIBRARIES}
#        dd4hep_library
#        EDM4HEP::edm4hep
#        EDM4EIC::edm4eic
#        spdlog::spdlog
#        Boost::boost
#        Eigen3::Eigen)
#
#target_link_libraries(${PLUGIN_NAME}_plugin
#        ${ROOT_LIBRARIES}
#        dd4hep_library
#        EDM4HEP::edm4hep
#        EDM4EIC::edm4eic
#        spdlog::spdlog
#        Boost::boost)