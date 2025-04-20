# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "filter_average_plugin\\CMakeFiles\\filter_average_plugin_autogen.dir\\AutogenUsed.txt"
  "filter_average_plugin\\CMakeFiles\\filter_average_plugin_autogen.dir\\ParseCache.txt"
  "filter_average_plugin\\filter_average_plugin_autogen"
  "scada\\CMakeFiles\\scada_autogen.dir\\AutogenUsed.txt"
  "scada\\CMakeFiles\\scada_autogen.dir\\ParseCache.txt"
  "scada\\CMakeFiles\\scada_core_autogen.dir\\AutogenUsed.txt"
  "scada\\CMakeFiles\\scada_core_autogen.dir\\ParseCache.txt"
  "scada\\scada_autogen"
  "scada\\scada_core_autogen"
  )
endif()
