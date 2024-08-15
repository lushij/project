# Install script for directory: /home/lu/search_engine/include/redis-plus-plus-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/lu/search_engine/include/redis-plus-plus-master/buildf/libredis++.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libredis++.so.1.3.13"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libredis++.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/lu/search_engine/include/redis-plus-plus-master/buildf/libredis++.so.1.3.13"
    "/home/lu/search_engine/include/redis-plus-plus-master/buildf/libredis++.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libredis++.so.1.3.13"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libredis++.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/usr/local/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/lu/search_engine/include/redis-plus-plus-master/buildf/libredis++.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake"
         "/home/lu/search_engine/include/redis-plus-plus-master/buildf/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/redis++/redis++-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "/home/lu/search_engine/include/redis-plus-plus-master/buildf/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES "/home/lu/search_engine/include/redis-plus-plus-master/buildf/CMakeFiles/Export/7d81f1912f64acc9d7f7c51a1b3ceb40/redis++-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sw/redis++" TYPE FILE FILES
    "/home/lu/search_engine/include/redis-plus-plus-master/buildf/src/sw/redis++/hiredis_features.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/cmd_formatter.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/command.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/command_args.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/command_options.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/connection.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/connection_pool.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/cxx11/sw/redis++/cxx_utils.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/errors.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/no_tls/sw/redis++/tls.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/pipeline.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/queued_redis.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/queued_redis.hpp"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis++.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis.hpp"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis_cluster.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis_cluster.hpp"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/redis_uri.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/reply.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/sentinel.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/shards.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/shards_pool.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/subscriber.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/transaction.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/utils.h"
    "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/version.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sw/redis++/patterns" TYPE FILE FILES "/home/lu/search_engine/include/redis-plus-plus-master/src/sw/redis++/patterns/redlock.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/redis++" TYPE FILE FILES
    "/home/lu/search_engine/include/redis-plus-plus-master/buildf/cmake/redis++-config.cmake"
    "/home/lu/search_engine/include/redis-plus-plus-master/buildf/cmake/redis++-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/lu/search_engine/include/redis-plus-plus-master/buildf/cmake/redis++.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/lu/search_engine/include/redis-plus-plus-master/buildf/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/lu/search_engine/include/redis-plus-plus-master/buildf/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
