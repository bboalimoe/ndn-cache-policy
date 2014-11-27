# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

with open('README') as stream:
  long_desc = stream.read()

requires = ['Sphinx>=0.6', 'xlrd']

setup(
    name='sphinxcontrib-exceltable',
    version='0.1',
    url='http://packages.python.org/sphinxcontrib-exceltable',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-exceltable',
    license='BSD',
    author='Juha Mustonen',
    author_email='juha.p.mustonen@gmail.com',
    description='Support for including Excel spreadsheets into Sphinx documents',
    long_description=long_desc,
    zip_safe=False,
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Environment :: Web Environment',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
    test_suite= 'nose.collector'
)
