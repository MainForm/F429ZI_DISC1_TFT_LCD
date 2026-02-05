
add_library(stmpe811 
    STATIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/STMPE811.cpp
)

target_include_directories(stmpe811
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(stmpe811 
    PUBLIC
        stm32cubemx
)

target_compile_options(stmpe811 PRIVATE
    # Add user defined compile options

    $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti -fno-exceptions>   
)