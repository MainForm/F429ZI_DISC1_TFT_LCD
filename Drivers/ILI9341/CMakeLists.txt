
add_library(ili9341 
    STATIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/ILI9341.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/FrameBuffer.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Src/font/font8.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/font/font12.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/font/font16.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/font/font20.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/font/font24.cpp
)

target_include_directories(ili9341
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(ili9341 
    PUBLIC
        stm32cubemx
)

target_compile_options(ili9341 PRIVATE
    # Add user defined compile options

    $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti -fno-exceptions>   
)