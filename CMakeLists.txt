cmake_minimum_required(VERSION 3.12)
project(ExchangeClient)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

option(EXECUTABLE_NAME "Executable name" "ExchangeClient")

set(EXCHANGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Find Protobuf and gRPC
find_package(Protobuf REQUIRED CONFIG)
find_package(gRPC REQUIRED)

find_program(Protobuf_PROTOC_EXECUTABLE NAMES protoc)
find_program(gRPC_CPP_PLUGIN_EXECUTABLE NAMES grpc_cpp_plugin)

# Set variables for source generation
set(PROTO_ROOT_DIR ${CMAKE_SOURCE_DIR}/semtex-grpc)
set(PROTO_SRC_DIR ${PROTO_ROOT_DIR}/protos)
set(PROTO_GEN_DIR ${PROTO_ROOT_DIR}/generated)
file(GLOB PROTO_FILES "${PROTO_SRC_DIR}/*.proto")
foreach(PROTO_FILE ${PROTO_FILES})
  get_filename_component(PROTO_FILE_NAME ${PROTO_FILE} NAME_WE)
  set(CURRENT_PROTO_FILE ${PROTO_SRC_DIR}/${PROTO_FILE_NAME}.proto)
  set(CURRENT_PROTO_SRC "${PROTO_GEN_DIR}/${PROTO_FILE_NAME}/${PROTO_FILE_NAME}.pb.cc")
  set(CURRENT_PROTO_HDR "${PROTO_GEN_DIR}/${PROTO_FILE_NAME}/${PROTO_FILE_NAME}.pb.h")
  set(CURRENT_GRPC_SRC "${PROTO_GEN_DIR}/${PROTO_FILE_NAME}/${PROTO_FILE_NAME}.grpc.pb.cc")
  set(CURRENT_GRPC_HDR "${PROTO_GEN_DIR}/${PROTO_FILE_NAME}/${PROTO_FILE_NAME}.grpc.pb.h")

  file(MAKE_DIRECTORY "${PROTO_GEN_DIR}/${PROTO_FILE_NAME}") # Create protos/PROTO_FILE_NAME directory

  message("Generating: ${CURRENT_PROTO_SRC} ${CURRENT_PROTO_HDR} ${CURRENT_GRPC_SRC} ${CURRENT_GRPC_HDR}")
  # Custom command to generate protobuf and gRPC sources
  add_custom_command(
    OUTPUT "${CURRENT_PROTO_SRC}" "${CURRENT_PROTO_HDR}" "${CURRENT_GRPC_SRC}" "${CURRENT_GRPC_HDR}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
      ARGS --grpc_out="${PROTO_GEN_DIR}/${PROTO_FILE_NAME}"
          --cpp_out="${PROTO_GEN_DIR}/${PROTO_FILE_NAME}"
          -I "${PROTO_SRC_DIR}"
          --plugin=protoc-gen-grpc="${gRPC_CPP_PLUGIN_EXECUTABLE}"
          "${CURRENT_PROTO_FILE}"
    DEPENDS "${CURRENT_PROTO_FILE}")

  # Collect sources for later use
  set(PROTO_FILES_ALL ${PROTO_FILES_ALL} ${CURRENT_PROTO_SRC} ${CURRENT_PROTO_HDR} ${CURRENT_GRPC_SRC} ${CURRENT_GRPC_HDR})
endforeach()

add_custom_target(protos ALL DEPENDS ${PROTO_FILES_ALL}) # Create custom target for executable to depend on

# Include project files
include_directories("${EXCHANGE_SOURCE_DIR}")
# Include generated files
include_directories("${PROTO_ROOT_DIR}")

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Find all source files
file(GLOB_RECURSE EXCHANGE_SOURCE_FILES "${EXCHANGE_SOURCE_DIR}/*.cpp")

# Add the executable
add_executable(ExchangeClient 
    ${EXCHANGE_SOURCE_FILES}
    ${PROTO_FILES_ALL}
)

add_dependencies(ExchangeClient protos) # Make sure protos are generated before building executable

# Link libraries
target_link_libraries(ExchangeClient 
    ${Protobuf_LIBRARIES}
    gRPC::grpc++ 
    gRPC::grpc++_reflection
)

# Set executable name
set_target_properties(ExchangeClient PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})