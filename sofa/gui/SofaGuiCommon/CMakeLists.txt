cmake_minimum_required(VERSION 2.8)
project(SofaGuiCommon)

set(HEADER_FILES
    ../BaseGUI.h
    ../BaseViewer.h
    ../BatchGUI.h
    ../ColourPickingVisitor.h
    ../MouseOperations.h
    ../OperationFactory.h
    ../PickHandler.h
    ../FilesRecentlyOpenedManager.h
    ../SofaGUI.h
    ../ViewerFactory.h
    ../GUIManager.h
    )

set(SOURCE_FILES
    ../BaseGUI.cpp
    ../BaseViewer.cpp
    ../BatchGUI.cpp
    ../ColourPickingVisitor.cpp
    ../FilesRecentlyOpenedManager.cpp
    ../MouseOperations.cpp
    ../PickHandler.cpp
    ../GUIManager.cpp
    ../ViewerFactory.cpp
    )

set(GROUP_BASE_DIR "..")



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

target_link_libraries( ${PROJECT_NAME} PUBLIC SofaGraphComponent SofaBaseCollision SofaUserInteraction SofaBaseVisual SofaComponentBase SofaComponentCommon SofaComponentGeneral)

set( SOFAGUICOMMON_COMPILER_FLAGS "-DSOFA_BUILD_SOFAGUI")


set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d" )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${SOFAGUI_VERSION} )
set_target_properties( ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${SOFAGUICOMMON_COMPILER_FLAGS}"  )
# set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${HEADER_FILES} )

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_SOURCE_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_BUILD_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_DEPENDENCY_DIR}>")

install(TARGETS ${PROJECT_NAME}
        EXPORT  SofaGuiTargets
        RUNTIME DESTINATION "${SOFAGUI_INSTALL_BIN_DIR}" COMPONENT SofaGui_libraries
        LIBRARY DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}" COMPONENT SofaGui_libraries
        ARCHIVE DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}" COMPONENT SofaGui_libraries
#        PUBLIC_HEADER DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa/gui" COMPONENT SofaGui_headers
        )
        
if(NOT SOFA_INSTALL_HEADERS_GENERATED_ONLY)
install(FILES ${HEADER_FILES} DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa/gui"
        COMPONENT SofaGui_headers
        )
endif()
