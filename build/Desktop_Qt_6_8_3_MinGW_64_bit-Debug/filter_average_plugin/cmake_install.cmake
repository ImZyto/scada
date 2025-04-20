# Install script for directory: C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/filter_average_plugin

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SCADA_ALL")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Qt/Tools/mingw1310_64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/scada/plugins/libfilter_average_plugin.dll")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/scada/plugins" TYPE SHARED_LIBRARY FILES "C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/build/Desktop_Qt_6_8_3_MinGW_64_bit-Debug/filter_average_plugin/libfilter_average_plugin.dll")
  if(EXISTS "$ENV{DESTDIR}/C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/scada/plugins/libfilter_average_plugin.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/scada/plugins/libfilter_average_plugin.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Qt/Tools/mingw1310_64/bin/strip.exe" "$ENV{DESTDIR}/C:/Users/micha/Desktop/magisterka/1_rok/3/zaawansowane_C++_2025L/scada/scada/plugins/libfilter_average_plugin.dll")
    endif()
  endif()
endif()

