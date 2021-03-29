file(GLOB sources src/*.cpp)
file(GLOB headers src/*.hpp)

add_library(nauLightning SHARED
	${sources} ${headers})
	
add_dependencies(nauLightning nau)
	
include_directories(
	../../contrib/glm
	../../nau/src
	../../nau/src/nau
)	

if (WIN32)
	add_definitions(-D_WINDLL)
endif(WIN32)
	
target_link_libraries(nauLightning $<TARGET_LINKER_FILE:nau> glbinding ${CUDA_LIBRARIES})
include_directories(${CUDA_INCLUDE_DIRS})


install (TARGETS nauLightning DESTINATION lib)
install(FILES ${headers} DESTINATION include)	

add_custom_command(
	TARGET nauLightning POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
	$<TARGET_FILE:nauLightning>
    ${CMAKE_BINARY_DIR}/bin/nauSettings/plugins/$<CONFIGURATION>/pass/$<TARGET_FILE_NAME:nauLightning>)
					   
if (WIN32)
	set_target_properties (nauLightning
		PROPERTIES FOLDER plugins)	
endif(WIN32)		
