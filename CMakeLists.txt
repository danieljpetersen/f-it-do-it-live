cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME "fi")
project(${PROJECT_NAME})

# some compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/")

function(configure_exe exe_name)

    # add sources
    # i'm sure there are more better ways to do this, but my hatred of cmake and refusal to learn more than the bare minimum of it prevents me from doing better. Ideally these next few if statements would just be a single line FILE(GLOB_RECURSE ${exe_name} ...), but that does not work 
    if (exe_name MATCHES "empty-app")
        FILE(GLOB_RECURSE EMPTY_SRC_FILES src/${exe_name}/*h src/${exe_name}/*.cpp src/${exe_name}/*.c src/${exe_name}/*.cc src/${exe_name}/*.hh src/${exe_name}/*.hpp src/${exe_name}/*.hp)
        add_executable(${exe_name} ${EMPTY_SRC_FILES} ${COMMON_SRC_FILES})
    elseif(exe_name MATCHES "it-always-ends-in-nuclear-war")
        FILE(GLOB_RECURSE ALWAYS_SRC_FILES src/${exe_name}/*h src/${exe_name}/*.cpp src/${exe_name}/*.c src/${exe_name}/*.cc src/${exe_name}/*.hh src/${exe_name}/*.hpp src/${exe_name}/*.hp)
        add_executable(${exe_name} ${ALWAYS_SRC_FILES} ${COMMON_SRC_FILES})
    elseif (exe_name MATCHES "it-usually-ends-in-nuclear-war")
        FILE(GLOB_RECURSE USUALLY_SRC_FILES src/${exe_name}/*h src/${exe_name}/*.cpp src/${exe_name}/*.c src/${exe_name}/*.cc src/${exe_name}/*.hh src/${exe_name}/*.hpp src/${exe_name}/*.hp)
        add_executable(${exe_name} ${USUALLY_SRC_FILES} ${COMMON_SRC_FILES})
    elseif (exe_name MATCHES "it-didnt-end-in-nuclear-war")
        FILE(GLOB_RECURSE DIDNT_SRC_FILES src/${exe_name}/*h src/${exe_name}/*.cpp src/${exe_name}/*.c src/${exe_name}/*.cc src/${exe_name}/*.hh src/${exe_name}/*.hpp src/${exe_name}/*.hp)
        add_executable(${exe_name} ${DIDNT_SRC_FILES} ${COMMON_SRC_FILES})
    elseif (exe_name MATCHES "terrible-geometric-video")
        FILE(GLOB_RECURSE TVG_SRC_FILES src/${exe_name}/*h src/${exe_name}/*.cpp src/${exe_name}/*.c src/${exe_name}/*.cc src/${exe_name}/*.hh src/${exe_name}/*.hpp src/${exe_name}/*.hp)
        add_executable(${exe_name} ${TVG_SRC_FILES} ${COMMON_SRC_FILES})
    endif()


    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/${exe_name}/")
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/${exe_name}/")
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}/bin/${exe_name}/")
    set_target_properties(${exe_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_LIST_DIR}/bin/${exe_name}/")


    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_definitions(${exe_name} PRIVATE FI_DEBUG_FLAG=1) # for debug specific #ifdef instructions in c++
    endif()


    # SFML
    find_package(SFML 2.5.1 REQUIRED COMPONENTS system window graphics audio)
    if (SFML_FOUND)
        # set(SFML_STATIC_LIBRARIES TRUE)
        include_directories(${SFML_INCLUDE_DIR})
        target_link_libraries(${exe_name} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} )
    endif()

    # OpenGL (for imgui)
    set(OpenGL_GL_PREFERENCE GLVND) # GLVND or LEGACY -- some versions of SFML give warnings if not explicitly set
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    if (OPENGL_FOUND)
        target_link_libraries(${exe_name} ${OPENGL_LIBRARIES})
    endif()


    # Thread Library
    find_package(Threads REQUIRED)
    target_link_libraries(${exe_name} Threads::Threads)


    ## GNU Compiler Flags
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-sign-compare -Wreturn-type")

        target_link_libraries(${exe_name} -lX11)

        IF(CMAKE_BUILD_TYPE MATCHES Release)
            message("Release build.")
            set(CMAKE_CXX_FLAGS "-O2 -Wreturn-type") ## Optimize + Warn about forgotten return statements
        else()
            message("Debug build.")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wreturn-type")
        endif()
    else()
        set(BUILD_ARCH "-m64")
    endif()
endfunction()


FILE(GLOB_RECURSE COMMON_SRC_FILES src/common/*h src/common/*.cpp src/common/*.c src/common/*.cc src/common/*.hh src/common/*.hpp src/common/*.hp)

configure_exe("empty-app")
configure_exe("it-always-ends-in-nuclear-war")
configure_exe("it-usually-ends-in-nuclear-war")
configure_exe("it-didnt-end-in-nuclear-war")
configure_exe("terrible-geometric-video")
