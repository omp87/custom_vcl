cmake_minimum_required (VERSION 3.10)
project(special_vcl_library)
find_package( OpenCV REQUIRED )

include_directories(../vdms/include/ ../vdms/src/vcl . /usr/local/include/opencv4 /usr/include/jsoncpp)
add_library(custom_vcl SHARED SpecialCustomVCL.cc )
target_link_libraries(custom_vcl ${OpenCV_LIBS})
