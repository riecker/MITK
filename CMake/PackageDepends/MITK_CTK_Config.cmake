# message("In package CTK config")
find_package(CTK REQUIRED)
if(CTK_REQUIRED_COMPONENTS_BY_MODULE)
  foreach(_ctk_component ${CTK_REQUIRED_COMPONENTS_BY_MODULE})
    list(APPEND ALL_INCLUDE_DIRECTORIES ${${_ctk_component}_INCLUDE_DIRS})
    list(APPEND ALL_LIBRARIES ${_ctk_component})
    list(APPEND ALL_LIBRARY_DIRS ${${_ctk_component}_LIBRARY_DIRS})
  endforeach()
else()
  list(APPEND ALL_INCLUDE_DIRECTORIES ${CTK_INCLUDE_DIRS})
  list(APPEND ALL_LIBRARIES ${CTK_LIBRARIES})
  link_directories(${CTK_LIBRARY_DIRS})
  foreach(_ctk_library ${CTK_LIBRARIES})
      if(${_ctk_library}_LIBRARY_DIRS)
        list(APPEND ALL_LIBRARY_DIRS "${${_ctk_library}_LIBRARY_DIRS}")
      endif()
    endforeach()
endif()
