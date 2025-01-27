# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/simplebrowser_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/simplebrowser_autogen.dir/ParseCache.txt"
  "simplebrowser_autogen"
  )
endif()
