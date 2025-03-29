# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\scada_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\scada_autogen.dir\\ParseCache.txt"
  "scada_autogen"
  )
endif()
