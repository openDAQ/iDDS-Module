macro(setup_cyclone_dds_core)
  set(CYCLONEDDS_REPO "https://github.com/eclipse-cyclonedds/cyclonedds.git")
  set(INSTALL_LOCATION "${CMAKE_BINARY_DIR}/cyclonedds-install")

  set(CLONE_DIR "${CMAKE_BINARY_DIR}/cyclonedds-src")
  set(BUILD_DIR "${CMAKE_BINARY_DIR}/cyclonedds-build")

  if(NOT EXISTS "${CLONE_DIR}")
    execute_process(
      COMMAND git clone ${CYCLONEDDS_REPO} ${CLONE_DIR}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
      message(FATAL_ERROR "Failed to clone CycloneDDS repository.")
    endif()
  endif()

  file(MAKE_DIRECTORY ${BUILD_DIR})

  execute_process(
    COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION} ${CLONE_DIR}
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_CONFIGURE_RESULT
  )
  if(NOT CMAKE_CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake configuration for CycloneDDS failed.")
  endif()

  # Build and install CycloneDDS Core library
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --parallel
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_BUILD_RESULT
  )
  if(NOT CMAKE_BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake build for CycloneDDS failed.")
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target install
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_INSTALL_RESULT
  )
  if(NOT CMAKE_INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake install for CycloneDDS failed.")
  endif()

  message(STATUS "CycloneDDS successfully built and installed at ${INSTALL_LOCATION}")
endmacro()


macro(setup_cyclone_dds_cxx_bindings)
  set(CYCLONEDDS_CXX_REPO "https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git")
  set(COREDDS_LOCATION "${CMAKE_BINARY_DIR}/cyclonedds-install")
  set(INSTALL_LOCATION "${CMAKE_BINARY_DIR}/cyclonedds-cxx-install")

  set(CLONE_DIR "${CMAKE_BINARY_DIR}/cyclonedds-cxx-src")
  set(BUILD_DIR "${CMAKE_BINARY_DIR}/cyclonedds-cxx-build")

  if(NOT EXISTS "${CLONE_DIR}")
    execute_process(
      COMMAND git clone ${CYCLONEDDS_CXX_REPO} ${CLONE_DIR}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
      message(FATAL_ERROR "Failed to clone CycloneDDS repository.")
    endif()
  endif()

  file(MAKE_DIRECTORY ${BUILD_DIR})

  execute_process(
    COMMAND ${CMAKE_COMMAND} -DCMAKE_PREFIX_PATH=${COREDDS_LOCATION} -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION} ${CLONE_DIR}
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_CONFIGURE_RESULT
  )
  if(NOT CMAKE_CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake configuration for CycloneDDS failed.")
  endif()

  # Build and install CycloneDDS-CXX bindings
  execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --parallel
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_BUILD_RESULT
  )
  if(NOT CMAKE_BUILD_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake build for CycloneDDS failed.")
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target install
    WORKING_DIRECTORY ${BUILD_DIR}
    RESULT_VARIABLE CMAKE_INSTALL_RESULT
  )
  if(NOT CMAKE_INSTALL_RESULT EQUAL 0)
    message(FATAL_ERROR "CMake install for CycloneDDS failed.")
  endif()

  message(STATUS "CycloneDDS successfully built and installed at ${INSTALL_LOCATION}")
endmacro()

macro(setup_dds_dependencies)
    setup_cyclone_dds_core()
    setup_cyclone_dds_cxx_bindings()
endmacro()