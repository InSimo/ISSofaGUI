cmake_minimum_required(VERSION 2.8)
project(SofaGuiQt)

if( POLICY CMP0020 )
	cmake_policy( SET CMP0020 NEW )
endif()

set(MOC_HEADER_FILES

	GenGraphForm.h
	AddObject.h
	RealGUI.h
	LinkWidget.h
	DataWidget.h
	DataFilenameWidget.h
	DisplayFlagsDataWidget.h
    EnumFlagsDataWidget.h
	MaterialDataWidget.h
	ModifyObject.h
	SimpleDataWidget.h
	WDoubleLineEdit.h
	SofaPluginManager.h
	SofaMouseManager.h
	SofaVideoRecorderManager.h
	QDataDescriptionWidget.h
	QDisplayPropertyWidget.h
	QDisplayLinkWidget.h     
	QDisplayDataWidget.h
	QTabulationModifyObject.h
	QTransformationWidget.h
	QMouseOperations.h
	QSofaListView.h
	QSofaRecorder.h
	QSofaStatWidget.h
	QModelViewTableUpdater.h
	)

# these header files do not need MOCcing
set(HEADER_FILES

	viewer/VisualModelPolicy.h
	viewer/SofaViewer.h
	GraphListenerQListView.h
	SofaGUIQt.h
	StructDataWidget.h
	TableDataWidget.h
	FileManagement.h
	PickHandlerCallBacks.h
	QMenuFilesRecentlyOpened.h
	ImageQt.h
	initPlugin.h
	QModelViewTableDataContainer.h
	)

set(SOURCE_FILES

	viewer/SofaViewer.cpp
	GraphListenerQListView.cpp
	GenGraphForm.cpp
	AddObject.cpp
	RealGUI.cpp
	LinkWidget.cpp 
	DataWidget.cpp 
	DataFilenameWidget.cpp
	DisplayFlagsDataWidget.cpp
    EnumFlagsDataWidget.cpp
	MaterialDataWidget.cpp
	ModifyObject.cpp
	SimpleDataWidget.cpp
	StructDataWidget.cpp
	TableDataWidget.cpp
	WDoubleLineEdit.cpp
	FileManagement.cpp
	SofaPluginManager.cpp
	SofaVideoRecorderManager.cpp
	SofaMouseManager.cpp
	QDataDescriptionWidget.cpp
	PickHandlerCallBacks.cpp
	QDisplayPropertyWidget.cpp
	QDisplayLinkWidget.cpp
	QDisplayDataWidget.cpp
	QMouseOperations.cpp               
	QTabulationModifyObject.cpp
	QTransformationWidget.cpp
	QSofaListView.cpp
	QSofaRecorder.cpp
	QSofaStatWidget.cpp
	QMenuFilesRecentlyOpened.cpp
	ImageQt.cpp 
	initPlugin.cpp
	)
	
set(UI_FILES

	GUI.ui
	BaseGenGraphForm.ui
	DialogAddObject.ui
	PluginManager.ui
	MouseManager.ui
	VideoRecorderManager.ui
	)

set( SOFAGUIQT_COMPILER_FLAGS "-DSOFA_BUILD_SOFAGUIQT")
set( SOFAGUIQT_LINK_LIBRARIES SofaGuiCommon SofaBaseVisual SofaExporter SofaLoader SofaOpenglVisual)

if(SOFA_OPTIONAL)
	set( SOFAGUIQT_LINK_LIBRARIES ${SOFAGUIQT_LINK_LIBRARIES} SofaMiscForceField)
endif()

if(SOFA_DUMP_VISITOR_INFO)
	list(APPEND MOC_HEADER_FILES

		 WindowVisitor.h
		 QVisitorControlPanel.h
		 PieWidget.h
		 )

    list(APPEND HEADER_FILES

         GraphVisitor.h
         )
		
	list(APPEND SOURCE_FILES
	
		 GraphVisitor.cpp
		 WindowVisitor.cpp
		 QVisitorControlPanel.cpp
		 PieWidget.cpp
		 )
	
	list(APPEND UI_FILES VisitorGUI.ui)
endif()


if(SOFA_HAVE_QWT)
	list(APPEND MOC_HEADER_FILES
		 GraphDataWidget.h
		 QGraphStatWidget.h
		 QEnergyStatWidget.h              
		 QMomentumStatWidget.h 
		 )
		
	list(APPEND SOURCE_FILES
		 GraphDataWidget.cpp  
		 )
	list(APPEND SOFAGUIQT_LINK_LIBRARIES qwt)
endif()

#if(SOFA-LIB_GUI_QTVIEWER)
	list(APPEND MOC_HEADER_FILES viewer/qt/QtViewer.h)
	list(APPEND SOURCE_FILES viewer/qt/QtViewer.cpp)
#endif()

if(SOFA_HAVE_QGLVIEWER)
	list(APPEND MOC_HEADER_FILES viewer/qgl/QtGLViewer.h)
	list(APPEND SOURCE_FILES viewer/qgl/QtGLViewer.cpp)
	list(APPEND SOFAGUIQT_LINK_LIBRARIES QGLViewer)
endif()

# make mocced header / form files path from absolute to relative
#ToRelativePath(MOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}" "${MOC_FILES}")
#ToRelativePath(FORM_FILES "${CMAKE_CURRENT_SOURCE_DIR}" "${FORM_FILES}")

if(SOFA_DUMP_VISITOR_INFO)
endif()

set(QT_USE_IMPORTED_TARGETS 1)
set(QT4_NO_LINK_QTMAIN 1)
find_package(Qt4 COMPONENTS qtcore qtgui qtopengl qt3support qtxml REQUIRED)

list(APPEND SOFAGUIQT_LINK_LIBRARIES
     ${QT_QTCORE_LIBRARY} 
     ${QT_QTGUI_LIBRARY} 
     ${QT_QTOPENGL_LIBRARY}
     ${QT_QT3SUPPORT_LIBRARY}     
     ${QT_QTXML_LIBRARY} )
     
QT4_WRAP_CPP(MOC_FILES ${MOC_HEADER_FILES})
QT4_WRAP_UI(FORM_FILES ${UI_FILES})

set(MOC_FILES_GROUP "Generated")
set(FORM_FILES_GROUP "Generated")



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

add_library(${PROJECT_NAME} SHARED ${MOC_HEADER_FILES} ${HEADER_FILES} ${MOC_FILES} ${FORM_FILES} ${SOURCE_FILES})

target_link_libraries( ${PROJECT_NAME} PUBLIC ${SOFAGUIQT_LINK_LIBRARIES})
target_compile_definitions( ${PROJECT_NAME} PUBLIC "SOFA_QT4" )
#target_compile_definitions( ${PROJECT_NAME} PUBLIC "${QT_DEFINITIONS}" )
target_compile_definitions( ${PROJECT_NAME} PUBLIC "QT3_SUPPORT" )
#target_compile_options( ${PROJECT_NAME} PUBLIC "-DSOFA_QT4")
set_target_properties( ${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_d" )
set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION ${SOFAGUI_VERSION} )
set_target_properties( ${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${SOFAGUIQT_COMPILER_FLAGS}"  )
# set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES} {MOC_HEADER_FILES}")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_SOURCE_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_BUILD_INCLUDE_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
#target_include_directories(${PROJECT_NAME} PUBLIC "${QT_INCLUDES}")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${SOFAGUI_DEPENDENCY_DIR}>")

install(TARGETS ${PROJECT_NAME}
        EXPORT  SofaGuiTargets
        RUNTIME DESTINATION "${SOFAGUI_INSTALL_BIN_DIR}" COMPONENT SofaGui_libraries
        LIBRARY DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}" COMPONENT SofaGui_libraries
        ARCHIVE DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}" COMPONENT SofaGui_libraries
#        PUBLIC_HEADER DESTINATION "${SOFAGUI_INSTALL_LIB_DIR}/sofa/gui/qt" COMPONENT SofaGui_headers
        )

install(FILES ${FORM_FILES} DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa/gui/qt"
        COMPONENT SofaGui_headers
        )
if(NOT SOFA_INSTALL_HEADERS_GENERATED_ONLY)
install(FILES ${HEADER_FILES} DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa/gui/qt"
        COMPONENT SofaGui_headers
        )
install(FILES ${MOC_HEADER_FILES} DESTINATION "${SOFAGUI_INSTALL_INCLUDE_DIR}/sofa/gui/qt"
        COMPONENT SofaGui_headers
        )
endif()
