import os
import sys
import sysconfig

from setuptools import setup, find_packages
from setuptools.extension import Extension


def path_to_build_folder():
    """Returns the name of a distutils build directory"""
    f = "{dirname}.{platform}-{version[0]}.{version[1]}"
    dir_name = f.format(dirname='lib',
                        platform=sysconfig.get_platform(),
                        version=sys.version_info)
    return os.path.join('build', dir_name, 'grumbo')


module1 = Extension(name="xgui.xgui", sources=['src/xgui/xguimodule.c'], libraries=['User32'],
                    library_dirs=[os.path.join(path_to_build_folder()), 'lib'],
                    extra_link_args=[])
setup(
    include_package_data=True,
    package_data={"xgui": ["lib/*.lib", '*.dll']},
    name='xgui',
    ext_modules=[module1],
    version='0.1.0',
    packages=find_packages(where='src'),
    package_dir={"": "src"},
    author='Gabriel Howe',
)
