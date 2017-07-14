from distutils.core import setup, Extension
import sys

major_version = '4'
minor_version = '0'

cpplmodule = Extension('cppl_cpp_python_bridge',
                    define_macros = [('MAJOR_VERSION', major_version),
                                     ('MINOR_VERSION', minor_version)],
                    include_dirs = [],
                    libraries = ['cppl'],
                    library_dirs = ['../policy-decision-point'],
                    sources = ['cpplmodule.cc'],
                    #extra_compile_args = ['-std=c++11', '-Wall', '-Werror',],
                    extra_compile_args = ['-std=c++11', '-Wall',],
                    # extra_objects are included _before_ library_dirs and libraries
                    extra_objects = [],
                    # extra_link_args are included _after_ library_dirs and libraries
                    extra_link_args = [])

setup (name = 'cppl',
       version = major_version + '.' + minor_version,
       description = 'A C++ - Python bridge for CPPL',
       author = 'Jens Hiller',
       author_email = 'jens.hiller@comsys.rwth-aachen.de',
       url = '',
       long_description = '''This package provides a C++-Python bridge for cppl (compact privacy policy language) functionality''',
       py_modules = ['cppl'],
       ext_modules = [cpplmodule])
