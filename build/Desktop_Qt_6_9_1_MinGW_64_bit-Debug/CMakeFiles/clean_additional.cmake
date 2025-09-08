# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\DeepRock_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DeepRock_autogen.dir\\ParseCache.txt"
  "DeepRock_autogen"
  )
endif()
