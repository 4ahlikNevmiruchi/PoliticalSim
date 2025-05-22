# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\PoliticalSim_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\PoliticalSim_autogen.dir\\ParseCache.txt"
  "PoliticalSim_autogen"
  )
endif()
