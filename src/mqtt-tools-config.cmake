# - Config file for the glRender package
# It defines the following variables
#  GLRENDER_INCLUDE_DIRS - include directories for SimpleLib
#  GLRENDER_LIBRARIES    - libraries to link against

set(WBMQTT_INCLUDE_DIRS "/usr/include/wbmqtt")

find_library(WBMQTT_LIBRARIES "mqtt-tools" HINTS "/usr/lib")

set(WBMQTT_FOUND ON)
