project(client)
cmake_minimum_required(VERSION 2.8)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g")
# cmake 常用变量和常用环境变量查表手册---整理
# https://www.cnblogs.com/xianghang123/p/3556425.html


# set(libconfig_dir lib_src/libconfig)
# ExternalProject_Add(
	# mylibconfig
	# GIT_REPOSITORY  https://github.com/hyperrealm/libconfig.git
	# #GIT_TAG 
	# PREFIX ${libconfig_dir}
	# #DOWNLOAD_COMMAND wget https://github.com/hyperrealm/libconfig/archive/master.zip && tar xvzf master.zip -C mylib --strip-components=1
	# CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${client_SOURCE_DIR} -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
# )


#当前工程共用库
aux_source_directory( ${client_SOURCE_DIR}/lib_src/pub lib_pub)
#添加依赖库 http://aigo.iteye.com/blog/2229401
#添加编译依赖库
add_library(PUB_LIB ${lib_pub})

# ADD_SUBDIRECTORY(lib_src)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   

#不同生成目标程序的独立子目录
aux_source_directory( ${client_SOURCE_DIR}/src/server src_server)
aux_source_directory( ${client_SOURCE_DIR}/src/client_loop src_client)
aux_source_directory( ${client_SOURCE_DIR}/src/client_loop_pool src_client_loop_pool)
aux_source_directory( ${client_SOURCE_DIR}/src/client_no_queue src_client_no_queue)




add_executable(c ${src_client} )
add_executable(p ${src_client_loop_pool} )
add_executable(s ${src_server} )
add_executable(cnq ${src_client_no_queue} )


INCLUDE_DIRECTORIES(
    ${client_SOURCE_DIR}/include
)

LINK_DIRECTORIES(
	${client_SOURCE_DIR}/lib
)

TARGET_LINK_LIBRARIES(
	c
	PUB_LIB
	boost_system boost_filesystem boost_thread 
)
TARGET_LINK_LIBRARIES(
	p
	PUB_LIB
	boost_system boost_filesystem boost_thread pthread
)
TARGET_LINK_LIBRARIES(
	s
	PUB_LIB
	boost_system boost_filesystem boost_thread  pthread
)
TARGET_LINK_LIBRARIES(
	cnq
	PUB_LIB
	boost_system boost_filesystem boost_thread 
)

set(client_BUILD_DIR ${client_SOURCE_DIR}/build)
set(client_OUTPUT_DIR ${client_SOURCE_DIR}/build/output)

ADD_CUSTOM_COMMAND(
	OUTPUT c s p cnq
	POST_BUILD
# execute_process(
	COMMAND mkdir -p ${client_OUTPUT_DIR}/bin
	COMMAND mkdir -p ${client_OUTPUT_DIR}/lib
	COMMAND mkdir -p ${client_OUTPUT_DIR}/conf
	COMMAND mkdir -p ${client_OUTPUT_DIR}/log

	COMMAND cp -rf ${client_SOURCE_DIR}/conf/* ${client_OUTPUT_DIR}/conf
	COMMAND cp -rf ${client_SOURCE_DIR}/lib/* ${client_OUTPUT_DIR}/lib
	COMMAND cp -rf ${client_BUILD_DIR}/c ${client_OUTPUT_DIR}/bin
	COMMAND cp -rf ${client_BUILD_DIR}/s ${client_OUTPUT_DIR}/bin
	COMMAND cp -rf ${client_BUILD_DIR}/p ${client_OUTPUT_DIR}/bin
	COMMAND cp -rf ${client_BUILD_DIR}/cnq ${client_OUTPUT_DIR}/bin

)