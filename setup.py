from setuptools import setup, find_packages
from setuptools.extension import Extension

setup(
    name='xgui',
    version='0.1.0',
    packages=find_packages(include=['xgui']),
    ext_modules=[Extension('cext', ['xgui/guimodule.c'], library_dirs=['xgui/lib'], libraries=['Gdi32', 'User32'])],
    url='',
    license='',
    author='gabri',
    author_email='',
    description='',
    setup_requires=['pytest-runner'],
    tests_require=['pytest==7.2.1'],
    test_suite='tests'
)
