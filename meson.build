project('wpebackend-direct', 'cpp',
        version: '1.0',
        default_options: ['warning_level=3',
                          'werror=true',
                          'cpp_std=c++20'])

subdir('backend')

wpewebkit_dep = dependency('wpe-webkit-1.0', version: '>=2.40', required: true)

executable('webview', 'main.cpp',
           build_rpath: '$ORIGIN/backend',
           dependencies: [wpewebkit_dep, wpebackend_dep])
