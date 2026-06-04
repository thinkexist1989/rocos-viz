cmake_minimum_required(VERSION 3.10)

#project(rocos-viz LANGUAGES CXX)
project(rocos-viz VERSION 1.0.1 DESCRIPTION "ROCOS Visualization Application")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# rocos-viz install path
include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX /opt/rocos/viz)
message(STATUS "INSTALL PREFIX: ${CMAKE_INSTALL_PREFIX}")
#message(STATUS "INSTALL INCLUDE DIR: ${CMAKE_INSTALL_INCLUDEDIR}")
#message(STATUS "INSTALL LIB DIR: ${CMAKE_INSTALL_LIBDIR}")
#message(STATUS "INSTALL BIN DIR: ${CMAKE_INSTALL_BINDIR}")

set(${PROJECT_NAME}_INSTALL_CONFIGDIR ${CMAKE_INSTALL_PREFIX}/config)
set(${PROJECT_NAME}_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.d
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

if (UNIX AND NOT APPLE)
    set(ROCOS_VIZ_QT_PREFIX_CANDIDATES
            "/opt/Qt/6.6.2/gcc_64"
            "/opt/Qt/6.2.4/gcc_64"
            )
    file(GLOB ROCOS_VIZ_QT_PREFIX_GLOB "/opt/Qt/*/gcc_64")
    list(APPEND ROCOS_VIZ_QT_PREFIX_CANDIDATES ${ROCOS_VIZ_QT_PREFIX_GLOB})
    list(REMOVE_DUPLICATES ROCOS_VIZ_QT_PREFIX_CANDIDATES)

    foreach(ROCOS_VIZ_QT_PREFIX IN LISTS ROCOS_VIZ_QT_PREFIX_CANDIDATES)
        if (EXISTS "${ROCOS_VIZ_QT_PREFIX}/lib/cmake/Qt6/Qt6Config.cmake"
                AND EXISTS "${ROCOS_VIZ_QT_PREFIX}/lib/cmake/Qt6Charts/Qt6ChartsConfig.cmake")
            list(APPEND CMAKE_PREFIX_PATH "${ROCOS_VIZ_QT_PREFIX}")
            message(STATUS "Using Qt prefix candidate: ${ROCOS_VIZ_QT_PREFIX}")
        endif ()
    endforeach()
endif ()

# Qt
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network OpenGLWidgets REQUIRED)
#find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network OpenGLWidgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Charts REQUIRED)

if(QT_VERSION_MAJOR EQUAL 6)
        find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
        set(QT_OPENGL_TARGET Qt6::OpenGLWidgets)
else()
        find_package(Qt5 COMPONENTS OpenGL REQUIRED)
        set(QT_OPENGL_TARGET Qt5::OpenGL)
endif()

# VTK
#find_package(VTK COMPONENTS
#        WrappingTools
#        CommonColor
#        CommonCore
#        CommonComputationalGeometry
#        CommonDataModel
#        CommonExecutionModel
#        CommonMath
#        CommonMisc
#        FiltersCore
#        FiltersSources
#        InfovisCore
#        InteractionStyle
#        RenderingContextOpenGL2
#        RenderingCore
#        RenderingFreeType
#        RenderingGL2PSOpenGL2
#        RenderingOpenGL2
#        ViewsQt
#        GUISupportQt
#        RenderingQt
#        )

find_package(VTK 9 REQUIRED)

message("VTK LIBS: ${VTK_LIBRARIES}")
#include(${VTK_USE_FILE})

#if(NOT DEFINED VTK_QT_VERSION)
#    set(VTK_QT_VERSION 5)
#endif()
#
#set(qt_components Core Gui Widgets)
#if(${VTK_QT_VERSION} VERSION_GREATER_EQUAL 6)
#    list(APPEND qt_components OpenGLWidgets)
#endif()
#list(SORT qt_components)
## We have ui files, so this will also bring in the macro:
##   qt5_wrap_ui or qt_wrap_ui from Widgets.
#find_package(Qt${VTK_QT_VERSION} QUIET
#        REQUIRED COMPONENTS ${qt_components}
#        )

# Eigen3
find_package(Eigen3 REQUIRED)

# Yaml-cpp
find_package(yaml-cpp REQUIRED)

# orocos_kdl
find_package(orocos_kdl REQUIRED)

include_directories(include)
include_directories(src)

set(TS_FILES rocos_zh_CN.ts)
set(SRC_FILES
        src/main.cpp
        src/RocosMainWindow.cpp
        src/MouseInteractorChooseActor.cpp
        src/SceneWidget.cpp
        src/Model.cpp
        src/Link.cpp
        src/PositionBar.cpp
        src/JointPositionWidget.cpp
        src/CartesianPositionWidget.cpp
        src/HttpRobotClient.cpp
        src/ConnectDialog.cpp
        src/ScriptDialog.cpp
        src/PlotWidget.cpp
        src/AboutDialog.cpp
        src/SplashScreen.cpp
        src/ModelLoaderDialog.cpp
        src/PlotConfigDialog.cpp
        )
set(HRD_FILES
        src/RocosMainWindow.h
        src/MouseInteractorChooseActor.h
        src/SceneWidget.h
        src/Model.h
        src/Link.h
        src/PositionBar.h
        src/JointPositionWidget.h
        src/CartesianPositionWidget.h
        src/HttpRobotClient.h
        src/ConnectDialog.h
        src/ScriptDialog.h
        src/PlotWidget.h
        src/AboutDialog.h
        src/SplashScreen.h
        src/ModelLoaderDialog.h
        src/PlotConfigDialog.h
        )
set(UI_FILES
        src/RocosMainWindow.ui
        src/JointPositionWidget.ui
        src/CartesianPositionWidget.ui
        src/ConnectDialog.ui
        src/ScriptDialog.ui
        src/PlotWidget.ui
        src/AboutDialog.ui
        src/ModelLoaderDialog.ui
        src/PlotConfigDialog.ui
        )

set(RC_FILES
        res.qrc)

add_executable(${PROJECT_NAME} WIN32
        ${SRC_FILES}
        ${HRD_FILES}
        ${UI_FILES}
        ${RC_FILES}
        ${TS_FILES}
        app_win.rc
        )

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${orocos_kdl_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        ${QT_OPENGL_TARGET}
        Qt${QT_VERSION_MAJOR}::Charts
        ${VTK_LIBRARIES}
        Eigen3::Eigen
        yaml-cpp
        ${orocos_kdl_LIBRARIES}
        )

vtk_module_autoinit(
        TARGETS ${PROJECT_NAME}
        MODULES ${VTK_LIBRARIES}
)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#configure_file(${MODELS} ./ COPYONLY)
#file(COPY models/ DESTINATION bin/models/)
file(COPY res/landau.png DESTINATION bin/)

#############
## Install ##
#############

# Add support for installaion
include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
        EXPORT rocos_viz-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 动态库安装路径
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # 静态库安装路径
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}  # 头文件安装路径
        )

install(EXPORT rocos_viz-targets
        FILE rocos_viz-targets.cmake
        NAMESPACE rocos::
        DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
        )

install(DIRECTORY models DESTINATION .)

configure_package_config_file(
        cmake/rocos_viz-config.cmake.in
        rocos_viz-config.cmake
        INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
)

# 写入库的版本信息
write_basic_package_version_file(
        rocos_viz-config-version.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion  # 表示该函数库向下兼容
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rocos_viz-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/rocos_viz-config-version.cmake"
        DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
        )

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/icon.png"
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        )

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/rocos-viz.desktop"
        DESTINATION /usr/share/applications/
        )

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/res/landau.png"
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
