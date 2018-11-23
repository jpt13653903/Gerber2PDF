project('Gerber2PdfGui', 'cpp', default_options : ['cpp_std=c++17'])
incdirs = include_directories('imgui', 'imgui/examples')
GL = dependency('GL')
glfw = dependency('glfw3')
cxx = meson.get_compiler('cpp')
boost_fs = [
    cxx.find_library('boost_system'),
    cxx.find_library('boost_filesystem'),
]

executable('Gerber2PdfGui', 
           'main.cpp', 
           'gui_main.cpp',
           'gerber_list_box.cpp',
           'imgui/imgui.cpp',
           'imgui/examples/imgui_impl_glfw.cpp',
           'imgui/examples/imgui_impl_opengl2.cpp',
           'imgui/imgui_draw.cpp',
           'imgui/imgui_widgets.cpp',
           include_directories: incdirs,
           dependencies: [GL, glfw, boost_fs],
           cpp_pch: 'pch/pch.hpp',
           override_options : ['cpp_std=c++17'])