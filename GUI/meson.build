project('Gerber2PdfGui', 'cpp', default_options : ['cpp_std=c++17'])
incdirs = include_directories('imgui', 'imgui/examples', 'imgui/misc/cpp')
GL = dependency('GL')
cxx = meson.get_compiler('cpp')
if build_machine.system() == 'windows'
    glfw = cxx.find_library('glfw3')
    boost_fs = [
        cxx.find_library('boost_system-mt'),
        cxx.find_library('boost_filesystem-mt'),
    ]
else
    glfw = dependency('glfw3')
    boost_fs = [
        cxx.find_library('boost_system'),
        cxx.find_library('boost_filesystem'),
    ]
endif

executable('Gerber2PdfGui', 
           'main.cpp', 
           'gui_main.cpp',
           'logic.cpp',
           'gerber_list_box.cpp',
           'file_chooser.cpp',
           'imgui/imgui.cpp',
           'imgui/misc/cpp/imgui_stdlib.cpp',
           'imgui/examples/imgui_impl_glfw.cpp',
           'imgui/examples/imgui_impl_opengl2.cpp',
           'imgui/imgui_draw.cpp',
           'imgui/imgui_widgets.cpp',
           include_directories: incdirs,
           dependencies: [GL, glfw, boost_fs],
           cpp_pch: 'pch/pch.hpp',
           override_options : ['cpp_std=c++17'])