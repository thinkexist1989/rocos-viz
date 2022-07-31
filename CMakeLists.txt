cmake_minimum_required(VERSION 3.10)

#project(rocos-viz LANGUAGES CXX)
project(rocos-viz)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

################
##    gRPC    ##
################

if (UNIX AND NOT APPLE)
    set(CMAKE_PREFIX_PATH "/opt/grpc")
endif ()

include(cmake/common.cmake) # gRPC

file(GLOB PROTO_FILES protos/*.proto) # protobuf
file(GLOB GRPC_FILES protos/*_service.proto) # gRPC

foreach (PFIL ${PROTO_FILES}) #protobuf gen
    # Proto file
    get_filename_component(PROTO_ABS ${PFIL} ABSOLUTE)
    get_filename_component(PROTO_PATH ${PFIL} PATH)
    get_filename_component(PROTO_NAME ${PFIL} NAME_WE)

    # Generated sources
    set(PROTO_SRC "${CMAKE_BINARY_DIR}/${PROTO_NAME}.pb.cc")
    set(PROTO_HDR "${CMAKE_BINARY_DIR}/${PROTO_NAME}.pb.h")
    add_custom_command(
            OUTPUT "${PROTO_SRC}" "${PROTO_HDR}"
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --cpp_out "${CMAKE_BINARY_DIR}"
            -I "${PROTO_PATH}"
            "${PROTO_ABS}"
            DEPENDS "${PROTO_ABS}"
    )
    list(APPEND PROTO_SRCS ${PROTO_SRC})
    list(APPEND PROTO_HDRS ${PROTO_HDR})
endforeach ()

foreach (GFIL ${GRPC_FILES}) #grpc gen
    # Proto file
    get_filename_component(GRPC_ABS ${GFIL} ABSOLUTE)
    get_filename_component(GRPC_PATH ${GFIL} PATH)
    get_filename_component(GRPC_NAME ${GFIL} NAME_WE)

    # Generated sources
    set(GRPC_SRC "${CMAKE_BINARY_DIR}/${GRPC_NAME}.grpc.pb.cc")
    set(GRPC_HDR "${CMAKE_BINARY_DIR}/${GRPC_NAME}.grpc.pb.h")
    add_custom_command(
            OUTPUT "${GRPC_SRC}" "${GRPC_HDR}"
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --grpc_out "${CMAKE_BINARY_DIR}"
            -I "${GRPC_PATH}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            "${GRPC_ABS}"
            DEPENDS "${GRPC_ABS}"
    )
    list(APPEND GRPC_SRCS ${GRPC_SRC})
    list(APPEND GRPC_HDRS ${GRPC_HDR})
endforeach ()

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(protocol
        STATIC
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        ${GRPC_SRCS}
        ${GRPC_HRDS}
        )
target_link_libraries(protocol
        PUBLIC
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        )
add_library(rocos::protocol ALIAS protocol)


# Qt
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Gamepad)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools Network Gamepad REQUIRED)

# VTK
find_package(VTK 8.2 REQUIRED)
include(${VTK_USE_FILE})

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
        src/ConnectDialog.cpp
        src/ScriptDialog.cpp
        src/PlotDialog.cpp
        src/AboutDialog.cpp
        src/SplashScreen.cpp
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
        src/ConnectDialog.h
        src/ScriptDialog.h
        src/PlotDialog.h
        src/AboutDialog.h
        src/SplashScreen.h
        )
set(UI_FILES
        src/RocosMainWindow.ui
        src/JointPositionWidget.ui
        src/CartesianPositionWidget.ui
        src/ConnectDialog.ui
        src/ScriptDialog.ui
        src/PlotDialog.ui
        src/AboutDialog.ui
        )

set(RC_FILES
        res.qrc)

add_executable(${PROJECT_NAME} WIN32
        ${SRC_FILES}
        ${HDR_FILES}
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
        Qt${QT_VERSION_MAJOR}::Gamepad
        ${VTK_LIBRARIES}
        Eigen3::Eigen
        yaml-cpp
        ${orocos_kdl_LIBRARIES}
        rocos::protocol
        )

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#configure_file(${MODELS} ./ COPYONLY)
file(COPY models/ DESTINATION models/)
