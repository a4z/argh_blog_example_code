
cmake_minimum_required(VERSION 3.6)
project(gitlike)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(_add_onefileer filename)

    set(options )
    set(nameArgs CXX_STANDARD)
    set(listArgs ) 

    cmake_parse_arguments(ARG 
        "${options}" "${nameArgs}" "${listArgs}" 
        ${ARGN}
    )

    if(NOT ARG_CXX_STANDARD)
        set(ARG_CXX_STANDARD 11)
    endif(NOT ARG_CXX_STANDARD)

    get_filename_component(bin_name ${filename} NAME_WE)

    add_executable(${bin_name} ${filename}) 
    set_property(TARGET ${bin_name} PROPERTY CXX_STANDARD ${ARG_CXX_STANDARD})

    set_target_properties(${EXEC_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/}
    )

endfunction(_add_onefileer)
    

_add_onefileer(gitlike.cpp CXX_STANDARD 11)

