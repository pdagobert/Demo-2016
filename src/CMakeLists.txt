file( GLOB_RECURSE source_files *.cpp )

include_directories( Library )

add_executable( ${app_name} ${source_files} )

if( WIN32 )
	target_link_libraries( ${app_name} SOIL glfw3dll opengl32 glew32 assimp libAntTweakBar SDL2_mixer )
else()
	target_link_libraries( ${app_name} SOIL glfw GL GLEW assimp AntTweakBar SDL2_mixer )
endif()