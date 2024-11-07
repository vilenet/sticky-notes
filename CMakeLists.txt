cmake_minimum_required(VERSION 3.5)
project(sticky-notes)

set(FLTK_INCLUDE_DIR "C:/libs/fltk/include")
set(FLTK_LIBRARY_DIR "C:/libs/fltk/lib")

find_package(FLTK REQUIRED PATHS "C:/libs/fltk" NO_DEFAULT_PATH)

include_directories(${FLTK_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(sticky-notes src/main.cpp src/app.cpp)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

target_link_libraries(sticky-notes
    ${FLTK_LIBRARY_DIR}/libfltk.a
    ${FLTK_LIBRARY_DIR}/libfltk_forms.a
    ${FLTK_LIBRARY_DIR}/libfltk_images.a
    ${FLTK_LIBRARY_DIR}/libfltk_jpeg.a
    ${FLTK_LIBRARY_DIR}/libfltk_png.a
    ${FLTK_LIBRARY_DIR}/libfltk_z.a
    gdi32
    comdlg32
    comctl32
    ws2_32
    ole32
    gdiplus
)
