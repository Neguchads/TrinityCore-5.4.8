# Copyright (C) 2011-2016 Project SkyFire <http://www.projectskyfire.org/
# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if(NOT BUILDDIR)
  set(BUILDDIR ${CMAKE_BINARY_DIR})
endif()

if(WITHOUT_GIT)
  set(rev_date "1970-01-01 00:00:00 +0000")
  set(rev_hash "unknown")
  set(rev_branch "Archived")
  string(TIMESTAMP rev_date_fallback "%Y-%m-%d %H:%M:%S" UTC)
else()
  if(GIT_EXECUTABLE)
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" rev-parse --short=12 HEAD
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE rev_hash
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    if(rev_hash)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" diff-index --quiet HEAD --
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE is_dirty
        ERROR_QUIET
      )

      if(is_dirty)
        set(rev_hash "${rev_hash}+")
      endif()

      execute_process(
        COMMAND "${GIT_EXECUTABLE}" show -s --format=%ci
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE rev_date
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )

      execute_process(
        COMMAND "${GIT_EXECUTABLE}" symbolic-ref -q --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE rev_branch
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )

      if(NOT rev_branch)
        execute_process(
          COMMAND "${GIT_EXECUTABLE}" for-each-ref --points-at=HEAD refs/heads "--format=%(refname:short)"
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
          OUTPUT_VARIABLE rev_branch
          OUTPUT_STRIP_TRAILING_WHITESPACE
          ERROR_QUIET
        )
      endif()

      if(NOT rev_branch)
        execute_process(
          COMMAND "${GIT_EXECUTABLE}" for-each-ref --points-at=HEAD refs/remotes "--format=%(refname:short)"
          WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
          OUTPUT_VARIABLE rev_branch
          OUTPUT_STRIP_TRAILING_WHITESPACE
          ERROR_QUIET
        )
      endif()

      if(NOT rev_branch)
        set(rev_branch ${rev_hash})
      endif()

      string(REGEX MATCH "^[^ \t\r\n]+" rev_branch ${rev_branch})
    endif()
  endif()

  if(NOT rev_hash)
    message(STATUS "
    Could not find a proper repository signature (hash) - you may need to pull tags with git fetch -t
    Continuing anyway - note that the versionstring will be set to \"unknown 1970-01-01 00:00:00 (Archived)\"")
    set(rev_date "1970-01-01 00:00:00 +0000")
    set(rev_hash "unknown")
    set(rev_branch "Archived")
    string(TIMESTAMP rev_date_fallback "%Y-%m-%d %H:%M:%S" UTC)
  else()
    set(rev_date_fallback ${rev_date})
  endif()
endif()

string(REGEX MATCH "([0-9]+)-([0-9]+)-([0-9]+)" rev_date_fallback_match ${rev_date_fallback})
set(rev_year ${CMAKE_MATCH_1})

cmake_host_system_information(RESULT TRINITY_BUILD_HOST_SYSTEM QUERY OS_NAME)
cmake_host_system_information(RESULT TRINITY_BUILD_HOST_DISTRO QUERY DISTRIB_INFO)
cmake_host_system_information(RESULT TRINITY_BUILD_HOST_SYSTEM_RELEASE QUERY OS_RELEASE)

if(WIN32)
  execute_process(
    COMMAND powershell -NoProfile -Command "$v=(Get-CimInstance -ClassName Win32_OperatingSystem); '{0} ({1})' -f $v.Caption, $v.Version"
    OUTPUT_VARIABLE TRINITY_BUILD_HOST_SYSTEM_RELEASE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(REGEX REPLACE "^.* Windows " "" TRINITY_BUILD_HOST_SYSTEM_RELEASE ${TRINITY_BUILD_HOST_SYSTEM_RELEASE})
endif()

if(CMAKE_SCRIPT_MODE_FILE)
  set(CMAKE_PLATFORM_INFO_DIR ${BUILDDIR}${CMAKE_FILES_DIRECTORY})
  include(${CMAKE_ROOT}/Modules/CMakeDetermineSystem.cmake)
endif()

configure_file(
  "${CMAKE_SOURCE_DIR}/revision_data.h.in.cmake"
  "${BUILDDIR}/revision_data.h"
  @ONLY
)
