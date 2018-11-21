project('Gerber2PdfGui', 'cpp', default_options : ['cpp_std=c++17'])
gtkdep = dependency('gtk+-3.0')
gtkmmdep = dependency('gtkmm-3.0')
executable('Gerber2PdfGui', 'main.cpp', 'logic.cpp', 'view.cpp',
           dependencies : [gtkdep, gtkmmdep], override_options : ['cpp_std=c++17'],
           cpp_pch: 'pch/pch.hpp')