cmake_minimum_required( VERSION 3.2.0 )

set( app_name "project" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin )
set( CMAKE_CXX_STANDARD 11 )

project( project )
subdirs( src )
