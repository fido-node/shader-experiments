cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME
        "gl_example"
        )

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(GLAD_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/glad-lib/include)
set(GLAD_LIBRARY_C ${PROJECT_SOURCE_DIR}/glad-lib/src/glad.c)
set(GLAD_LIBRARY_H ${PROJECT_SOURCE_DIR}/glad-lib/include/glad/glad.h)



set(SOURCE_FILES
        source/main.cpp
        ${GLAD_LIBRARY_H}
        ${GLAD_LIBRARY_C}
        )
set(CMAKE_CXX_STANDARD 11)


find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

find_package( glm REQUIRED )


include_directories(
        ${SDL2_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${GLAD_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
        ${PROJECT_NAME}
        ${SDL2_LIBRARY}
        ${CMAKE_DL_LIBS}
        glm
        #        ${OPENGL_LIBRARIES}
        ${GLAD_LIBRARY}
)
