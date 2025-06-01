cmake_minimum_required(VERSION 3.30)
project(SyntaxHighlighter)

set(CMAKE_CXX_STANDARD 17)

# FLTK'yı FetchContent ile dahil edelim
include(FetchContent)

# FLTK'yı FetchContent ile indirip projeye dahil edelim
FetchContent_Declare(
        FLTK
        GIT_REPOSITORY https://github.com/fltk/fltk.git
        GIT_TAG        master
)

FetchContent_MakeAvailable(FLTK)

# Kaynak dosyanızı belirtin
add_executable(SyntaxHighlighter main.cpp)

# FLTK kütüphanesini bağlayalım
target_link_libraries(SyntaxHighlighter PUBLIC fltk)

# FLTK'nın başlık dosyalarını ve kütüphane dizinlerini ekleyelim
target_include_directories(SyntaxHighlighter PUBLIC ${FLTK_SOURCE_DIR})