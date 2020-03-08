import os, platform, subprocess
import tdbuild.tdbuild as tdbuild

build_options = {
    'source_dir': os.path.join('..', 'src'),
    'include_dirs': [
        os.path.join('..', 'include'),
        os.path.join('..', 'include', 'lua')
    ],
    'lib_dir': os.path.join('..', 'lib'),
    'build_dir': 'build',
    'source_files': [
        os.path.join("..", "include", "imgui", "imgui.cpp"),
        os.path.join("..", "include", "imgui", "imgui_demo.cpp"),
        os.path.join("..", "include", "imgui", "imgui_draw.cpp"),
        os.path.join("..", "include", "imgui", "imgui_widgets.cpp"),
        os.path.join("..", "include", "glad", "glad.c"),
        "win_main.cpp"
    ],
    'debug': True,
    'cpp': True,
    'cpp_standard': '17',
    'Windows': {
        'system_libs': [
            'user32.lib',
            'opengl32.lib',
            'gdi32.lib',
            'Shell32.lib',
            'Kernel32.lib'
        ],
        'user_libs': [
            'freetyped.lib',
            'glfw3.lib',
            'lua51.lib'
        ],
        'dlls': [
            'lua51.dll'
        ],
        'ignore': [
            '4099',
            '4068'   # unknown pragma
        ],
        'machine': 'X86',
        'out': 'tdengine.exe',
        'runtime_library': 'MDd',
        'warnings': [
            '4530',
            '4201',
            '4577',
            '4310',
            '4624'
        ],
        'extras': [
            '/nologo',
            '/D_CRT_SECURE_NO_WARNINGS',
            '/W2'
        ]
    },
    'Darwin': {
        'compiler': 'g++-9',
        'user_libs': [
            'libfreetype.a',
            'libglfw3.a'
        ],
        'system_libs': [
            'objc',
            'bz2',
            'z',
        ],
        'frameworks': [
            'Cocoa',
            'OpenGL',
            'CoreVideo',
            'IOKit'
        ],
        'out': 'tdengine',
        'extras': [
            '-Wall',
            '-fpermissive'
        ]
    },
    'Linux': {
        'compiler': 'g++',
        'user_libs': [
            'freetype',
            'glfw3',
            'lua51'
        ],
        'system_libs':[
            'z',
            'GL',
            'X11',
            'c',
            'dl',
            'stdc++fs'
        ],
        'extras': [
            '-pthread'
        ],
        'out': 'tdengine'
    }
}

class Builder(tdbuild.base_builder):
    def __init__(self):
        super().__init__()

    def build(self):
        if platform.system() == 'Windows':
            subprocess.run(['setup_devenv.bat'], stdout=subprocess.PIPE)
        super().build()
        
    def run(self):
        super().run()
        
    def setup(self):
        super().setup()
        
    def prebuild(self):
        pass
    
