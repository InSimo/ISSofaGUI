cmake_minimum_required(VERSION 2.8)
project(SofaGuiGlut)

set(HEADER_FILES

	SimpleGUI.h
	)

set(SOURCE_FILES

	SimpleGUI.cpp
	)

if(SOFA_HAVE_BOOST)
	list(APPEND HEADER_FILES MultithreadGUI.h)
	
	list(APPEND SOURCE_FILES MultithreadGUI.cpp)
endif()



include( CMakeDependentOption )
cmake_dependent_option(UNITY_BUILD_${PROJECT_NAME} "Decides if a the generated project files should build through a unity build instead of a normal build. A unity builds packs all .cpp files into a UnityBuild.cpp file and then only include this in the project. This greatly reduces build times." ON  SOFA_ALLOW_UNITY_BUILDS OFF)

if( UNITY_BUILD_${PROJECT_NAME} )
  # Generate a unity build, by creating the UnityBuild.cpp and only including the required 
  # source files.
  set( UNITYBUILD_INCLUDES "" )
  foreach(filename ${SOURCE_FILES})
    SET( UNITYBUILD_INCLUDES "${UNITYBUILD_INCLUDES}#include \"${filename}\"\n" )
  endforeach(filename)

  file(WRITE UnityBuild.cpp ${UNITYBUILD_INCLUDES} )

  # overwrite the source files to only include the generated unity build file.
  set( SOURCE_FILES UnityBuild.cpp )
endif()

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC SofaGuiCommon)

set( SOFAGUIGLUT_COMPILER_FLAGS "-DSOFA_BUILD_SOFAGUIGLUT")

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d" )
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${SOFAGUI_VERSION} )
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${SOFAGUIGLUT_COMPILER_FLAGS}"  )
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_SOURCE_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_BUILD_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_DEPENDENCY_DIR}>")

install(TARGETS ${PROJECT_NAME}
        COMPONENT SofaGui_libraries
        EXPORT  SofaGuiTargets
        RUNTIME DESTINATION "${SOFAGUI_INSTALL_BIN_DIR}"
        LIBRARY DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}"
        )

if(NOT SOFA_INSTALL_HEADERS_GENERATED_ONLY)
install(DIRECTORY ${SofaGuiGlut_SOURCE_DIR}/ DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/SofaGuiGlut"
        COMPONENT SofaGui_headers
        FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.inl"
            PATTERN "*.hxx"
            PATTERN "*.hpp"
        )
endif()
