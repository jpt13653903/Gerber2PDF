project('Gerber2PdfGui', 'cpp', default_options : ['cpp_std=c++14'])
incdirs = include_directories(
    'imgui', 'imgui/examples', 'imgui/misc/cpp',
    '../Source/', 
    '../Source/Tools/Deflate/',
    '../Source/Tools/File/',
    '../Source/Tools/Gerber/',
    '../Source/Tools/PDF/',
    '../Source/Tools/String/', 
)

cxx = meson.get_compiler('cpp')
engine = cxx.find_library('Engine', dirs: meson.current_source_dir()+'/../Source/obj')

if host_machine.system() == 'windows'
    GL = cxx.find_library('opengl32')
    glfw = cxx.find_library('glfw3')
    boost_fs = [
        cxx.find_library('boost_system-mt'),
        cxx.find_library('boost_filesystem-mt'),
    ]
else
    GL = dependency('GL')
    glfw = dependency('glfw3')
    boost_fs = [
        cxx.find_library('boost_system'),
        cxx.find_library('boost_filesystem'),
    ]
endif

executable('Gerber2pdf_Gui', 
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
           dependencies: [engine, GL, glfw, boost_fs],
           cpp_pch: 'pch/pch.hpp',
           override_options : ['cpp_std=c++14'])