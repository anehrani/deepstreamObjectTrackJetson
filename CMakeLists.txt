cmake_minimum_required(VERSION 3.10)
project(speedEstimation_v3)

set(CMAKE_CXX_STANDARD 14) #setting C++ 14 standard


# Set the build type to Release
set(CMAKE_BUILD_TYPE Release)

# Set the C++ flags for release mode to enable code optimization
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


find_package(PkgConfig) #finding pkg-config is a helper tool


pkg_search_module(GLIB REQUIRED glib-2.0)

#using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# link cuda
include(FindCUDA)
find_package(CUDA REQUIRED)
set(CUDA_NVCC_PLAGS ${CUDA_NVCC_PLAGS};-std=c++14;-g;-G;-gencode;arch=compute_30;code=sm_30)
#
include_directories(
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        /usr/include/glib-2.0
        /usr/include/libsoup-2.4
)

# include deepstream directory
include_directories(
        /opt/nvidia/deepstream/deepstream-6.0/sources/includes/ # path to deepstream (locally to be installed)
        include/rapidjson
)
#including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
        ${CUDA_INCLUDE_DIRS}
)

#
#linking GStreamer library directory
link_directories(
        ${GSTREAMER_LIBRARY_DIRS}
        ${CUDA_LIBRARIES}
)

#
set( nvanalytics
        include/ds_speed.cpp
        include/ds_speed.h
        include/dsanalytics.cpp
        include/dsanalytics.h
        include/post_processing.cpp
        include/post_processing.h
        include/configurations.h
        include/configurations.cpp
        include/metadata.h
        include/metadata.cpp
        include/configurations.cpp
        include/calculateSpeed.cpp
        include/calculateSpeed.h)

#building target executable
add_executable(
        ${PROJECT_NAME}
        main.cpp
        ${nvanalytics}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLIB_INCLUDE_DIRS})
#linking Gstreamer library with target executable

target_link_libraries(${PROJECT_NAME}
        ${GSTREAMER_LIBRARIES}
        ${GLIB_LDFLAGS}
        ${GLIB_LIBRARIES}
        nvonnxparser
        nvinfer
        nvinfer_plugin
        )



set(DSLIBDIR /opt/nvidia/deepstream/deepstream-6.0/lib/)
set(PROJECT_LINK_LIBS
        ${DSLIBDIR}/libnvds_inferutils.so
        ${DSLIBDIR}/libnvds_nvtxhelper.so
        ${DSLIBDIR}/libnvds_nvmultiobjecttracker.so
        ${DSLIBDIR}/libnvds_msgconv.so
        ${DSLIBDIR}/libnvds_msgbroker.so
        ${DSLIBDIR}/libnvds_meta.so
        ${DSLIBDIR}/libnvds_logger.so
        ${DSLIBDIR}/libnvds_kafka_proto.so
        ${DSLIBDIR}/libnvds_inferutils.so
        ${DSLIBDIR}/libnvds_infer.so
        ${DSLIBDIR}/libnvds_infer_server.so
        ${DSLIBDIR}/libnvds_inferlogger.so
        ${DSLIBDIR}/libnvds_infercustomparser.so
        ${DSLIBDIR}/libnvdsgst_tensor.so
        ${DSLIBDIR}/libnvdsgst_smartrecord.so
        ${DSLIBDIR}/libnvdsgst_meta.so
        ${DSLIBDIR}/libnvdsgst_inferbase.so
        ${DSLIBDIR}/libnvdsgst_helper.so
        ${DSLIBDIR}/libnvds_dsanalytics.so
        ${DSLIBDIR}/libnvds_dewarper.so
        ${DSLIBDIR}/libnvds_custom_sequence_preprocess.so
        ${DSLIBDIR}/libnvds_csvparser.so
        ${DSLIBDIR}/libnvds_batch_jpegenc.so
        ${DSLIBDIR}/libnvds_amqp_proto.so
        ${DSLIBDIR}/libnvbufsurftransform.so
        ${DSLIBDIR}/libnvbufsurface.so
        ${DSLIBDIR}/libcustom_videoimpl.so
        ${DSLIBDIR}/libiothub_client.so
        /usr/local/cuda/lib64/libcudart.so
        )

target_link_libraries( ${PROJECT_NAME}
        ${PROJECT_LINK_LIBS}
        ${GLIB_LIBRARIES}
        )




#

