find_package(PackageHandleStandardArgs)

include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES
    EP_PREFIX ${EP_PREFIX}
    )

if(CMAKE_GENERATOR MATCHES Xcode)
  set(XCODE_EXT "/Debug")
else()
  set(XCODE_EXT "")
endif()

# Project dependencies.
find_package(Threads REQUIRED)

################################
#           protobuf           #
################################
option(FIND_PROTOBUF "Try to find protobuf in system" ON)
find_package(protobuf)

#############################
#         optional          #
#############################
find_package(optional)

##########################
#         boost          #
##########################
find_package(Boost 1.65.0 REQUIRED
    COMPONENTS
    )
add_library(boost INTERFACE IMPORTED)
set_target_properties(boost PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS}
#    INTERFACE_LINK_LIBRARIES "${Boost_LIBRARIES}"
    )

if(ENABLE_LIBS_PACKAGING)
  foreach (library ${Boost_LIBRARIES})
    add_install_step_for_lib(${library})
  endforeach(library)
endif()

###################################
#          ed25519/sha3           #
###################################
find_package(ed25519)
