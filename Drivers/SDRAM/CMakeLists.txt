
add_library(sdramIS42S16400J 
    STATIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/IS42S16400J_7TL.c
)

target_include_directories(sdramIS42S16400J
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(sdramIS42S16400J 
    PUBLIC
        stm32cubemx
)

target_compile_options(sdramIS42S16400J PRIVATE
    # Add user defined compile options

    $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti -fno-exceptions>   
)