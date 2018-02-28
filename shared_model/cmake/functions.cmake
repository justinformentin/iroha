function(compile_proto_to_cpp PROTO)
  string(REGEX REPLACE "\\.proto$" ".pb.h" GEN_PB_HEADER ${PROTO})
  string(REGEX REPLACE "\\.proto$" ".pb.cc" GEN_PB ${PROTO})
  add_custom_command(
      OUTPUT ${IROHA_SCHEMA_DIR}/${GEN_PB_HEADER} ${IROHA_SCHEMA_DIR}/${GEN_PB}
      COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${protobuf_LIBRARY_DIR}:$ENV{LD_LIBRARY_PATH} "${protoc_EXECUTABLE}"
      ARGS -I${protobuf_INCLUDE_DIR} -I. --cpp_out=${IROHA_SCHEMA_DIR} ${PROTO}
      DEPENDS protoc
      WORKING_DIRECTORY ${IROHA_SCHEMA_DIR}
      )
endfunction()

macro(set_target_description target description url commit)
  set_package_properties(${target}
      PROPERTIES
      URL ${url}
      DESCRIPTION ${description}
      PURPOSE "commit: ${commit}"
      )
endmacro()

function(iroha_get_lib_name out lib type)
  if(type STREQUAL "STATIC")
    set(${out} ${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX} PARENT_SCOPE)
  elseif(type STREQUAL "SHARED")
    set(${out} ${CMAKE_SHARED_LIBRARY_PREFIX}${lib}${CMAKE_SHARED_LIBRARY_SUFFIX} PARENT_SCOPE)
  else()
    message(FATAL_ERROR "type can be either STATIC or SHARED")
  endif()
endfunction()
