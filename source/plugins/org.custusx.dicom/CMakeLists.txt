project(org_custusx_dicom)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

include_directories(
    ${CX_RESOURCE_WIDGETS_INCLUDE_DIRECTORIES}
    ${CustusX3_SOURCE_DIR}/source/modules/GUIExtender
    )

set(PLUGIN_SRCS
  cxDicomPluginActivator.cpp
  cxDicomGUIExtenderService.cpp
  cxDicomWidget.cpp
  cxDicomConverter.h
  cxDicomConverter.cpp
  cxDicomImageReader.h
  cxDicomImageReader.cpp
  cxDICOMAppWidget.cpp
  cxDICOMModel.cpp
  cxDicomImporter.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxDICOMAppWidget.h
  cxDICOMModel.h
  cxDicomPluginActivator.h
  cxDicomGUIExtenderService.h
  cxDicomWidget.h
  cxDicomImporter.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)


#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}   
    cxGUIExtenderService
    cxResourceWidgets
    CTKDICOMWidgets
    )

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

add_subdirectory(testing)
