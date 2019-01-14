cmake_minimum_required(VERSION 2.8.11)
set(SOLUTION_NAME "ISSofaGui")
project(${SOLUTION_NAME})

set( SofaGui_ACTUAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sofa/gui )
set(SOFAGUI_SOURCE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

##
## SofaGui version
##
set( SOFAGUI_MAJOR_VERSION 0 )
set( SOFAGUI_MINOR_VERSION 0 )
set( SOFAGUI_BUILD_VERSION 1 )

set( SOFAGUI_VERSION ${SOFAGUI_MAJOR_VERSION}.${SOFAGUI_MINOR_VERSION}.${SOFAGUI_BUILD_VERSION} CACHE INTERNAL "Set to internal so it can be changed by sub directories.")

set(SOFAGUI_BUILD_LIB_DIR     ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(SOFAGUI_BUILD_BIN_DIR     ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(SOFAGUI_BUILD_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(SOFAGUI_BUILD_CMAKE_DIR   ${CMAKE_BINARY_DIR}/SofaGui)

# Offer the user the choice of overriding the installation directories
set(SOFAGUI_INSTALL_LIB_DIR     lib     					  CACHE PATH "Installation directory for libraries")
set(SOFAGUI_INSTALL_BIN_DIR     bin     					  CACHE PATH "Installation directory for executables")
set(SOFAGUI_INSTALL_INCLUDE_DIR include 					  CACHE PATH "Installation directory for header files")
set(SOFAGUI_INSTALL_CMAKE_DIR   ${SOFAGUI_INSTALL_LIB_DIR}/cmake/SofaGui)

## SofaGui dependencies
##
find_package(SofaGeneral REQUIRED)
find_package(SofaMisc REQUIRED)
set(SOFAGUI_DEPENDENCY_DIR ${SofaGeneral_INCLUDE_DIRS})

set(QT_USE_IMPORTED_TARGETS 1)
set(QT4_NO_LINK_QTMAIN 1)
set(QT_COMPONENTS_LIST QtCore QtGui QtOpenGL QtXml Qt3Support)
find_package(Qt4 COMPONENTS ${QT_COMPONENTS_LIST})
# Qt3Support needs QtSql and QtNetwork, so we will embed the associated DLLs too
set(QT_LIBS_LIST ${QT_COMPONENTS_LIST} QtNetwork QtSql)

find_package(Qwt)

set(SOFA_HAVE_QWT ${Qwt_FOUND} )
set(SOFA_HAVE_QT ${Qt4_FOUND} )
set(SOFA_QT4 ${Qt4_FOUND} )

set(SOFAGUI_TARGETS SofaGuiCommon SofaGuiBatch)

add_subdirectory(${SofaGui_ACTUAL_SOURCE_DIR}/SofaGuiCommon ${CMAKE_CURRENT_BINARY_DIR}/SofaGuiCommon)
add_subdirectory(${SofaGui_ACTUAL_SOURCE_DIR}/SofaGuiBatch ${CMAKE_CURRENT_BINARY_DIR}/SofaGuiBatch)

if(SOFA_HAVE_GLUT)
    set(SOFA_GUI_GLUT 1)
    list(APPEND SOFAGUI_TARGETS SofaGuiGlut)
    add_subdirectory(${SofaGui_ACTUAL_SOURCE_DIR}/glut          ${CMAKE_CURRENT_BINARY_DIR}/SofaGuiGlut)
endif()

if(Qt4_FOUND)
    if (WIN32)
        # Install the Qt DLLs directly in the binaries dir
        get_filename_component(QT_DLL_DIR ${QT_QMAKE_EXECUTABLE} PATH)
        foreach(qtlib ${QT_LIBS_LIST})
            install(FILES ${QT_DLL_DIR}/${qtlib}d4.dll
                    DESTINATION ${DEFAULT_BIN_INSTALL} CONFIGURATIONS Debug)
            install(FILES ${QT_DLL_DIR}/${qtlib}4.dll
                    DESTINATION ${DEFAULT_BIN_INSTALL} CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endforeach(qtlib)
    else()
        # Install the Qt DLLs in the lib dir.
        foreach(qtlib ${QT_LIBS_LIST})
            string(TOUPPER ${qtlib} upper_qtlib)
            # Example for QtCore: QT_QTCORE_LIBRARY_DEBUG/RELEASE should be like
            # "/usr/lib/x86_64-linux-gnu/libQtCore.so", which is a symlink.
            # Let's search all the files related to the corresponding lib
            # (actual lib file + symlinks) with a simple GLOB pattern, to be able
            # to install them all together (just the .so symlink and its target
            # is not enough... for example libQtCore.so points to libQtCore.so.4.8.7,
            # but libQt3Support.so.4.8.7 needs the libQtCore.so.4 symlink).
            file(GLOB lib_file_and_symlinks_debug "" "${QT_${upper_qtlib}_LIBRARY_DEBUG}*")
            install(FILES ${lib_file_and_symlinks_debug}
                    DESTINATION ${DEFAULT_LIB_INSTALL} CONFIGURATIONS Debug)
            file(GLOB lib_file_and_symlinks_release "" "${QT_${upper_qtlib}_LIBRARY_RELEASE}*")
            install(FILES ${lib_file_and_symlinks_release}
                    DESTINATION ${DEFAULT_LIB_INSTALL} CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        endforeach(qtlib)
    endif()

    set(SOFA_GUI_QT 1)

    list(APPEND SOFAGUI_TARGETS SofaGuiQt)
    add_subdirectory(${SofaGui_ACTUAL_SOURCE_DIR}/qt            ${CMAKE_CURRENT_BINARY_DIR}/SofaGuiQt)
    set(SOFA_GUI_QTVIEWER 1)
    
endif(Qt4_FOUND)

foreach(TARGET ${SOFAGUI_TARGETS})
  # set IDE project filter
  if(MSVC)
    set_target_properties(${TARGET} PROPERTIES FOLDER "SofaGui")
  endif()
endforeach()

configure_file( ${ISSofaGui_SOURCE_DIR}/SofaGui.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/sofa/SofaGui.h )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/sofa/SofaGui.h
         DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa")

include(CMakePackageConfigHelpers)

# SofaGuiConfigVersion.cmake file (same for build tree and install)
write_basic_package_version_file(${SOFAGUI_BUILD_CMAKE_DIR}/SofaGuiConfigVersion.cmake
                                 VERSION ${SOFAGUI_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${SOFAGUI_BUILD_CMAKE_DIR}/SofaGuiConfigVersion.cmake
        DESTINATION ${SOFAGUI_INSTALL_CMAKE_DIR})

# SofaGuiConfig.cmake (build tree)
set(SOFAGUI_INCLUDE_DIR "${SOFAGUI_BUILD_INCLUDE_DIR}")
configure_package_config_file(SofaGuiConfig.cmake.in
                              ${SOFAGUI_BUILD_CMAKE_DIR}/SofaGuiConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
                              PATH_VARS SOFAGUI_INCLUDE_DIR)

# SofaGuiConfig.cmake (installed)
set(SOFAGUI_INCLUDE_DIR "${SOFAGUI_INSTALL_INCLUDE_DIR}")
configure_package_config_file(SofaGuiConfig.cmake.in
                              ${SOFAGUI_BUILD_CMAKE_DIR}/SofaGuiConfigInstall.cmake
                              INSTALL_DESTINATION ${SOFAGUI_INSTALL_CMAKE_DIR}
                              PATH_VARS SOFAGUI_INCLUDE_DIR )

install(FILES ${SOFAGUI_BUILD_CMAKE_DIR}/SofaGuiConfigInstall.cmake
        DESTINATION ${SOFAGUI_INSTALL_CMAKE_DIR}
        RENAME SofaGuiConfig.cmake)

# SofaGuiTargets.cmake (installed)
install(EXPORT SofaGuiTargets
        DESTINATION ${SOFAGUI_INSTALL_CMAKE_DIR} )
