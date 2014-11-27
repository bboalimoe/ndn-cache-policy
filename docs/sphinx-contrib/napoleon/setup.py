# -*- coding: utf-8 -*-
# Copyright 2014 Rob Ruana
# Licensed under the BSD License, see LICENSE file for details.

"""Sphinx "napoleon" extension."""

import sys
from setuptools import setup, find_packages

reqs = open('requirements.txt', 'r').read().strip().splitlines()
reqs_test = open('requirements_test.txt', 'r').read().strip().splitlines()

extra = {}
if sys.version_info[0] >= 3:
    extra['use_2to3'] = True
    extra['use_2to3_on_doctests'] = True

setup(
    name='sphinxcontrib-napoleon',
    version='0.2.8',
    url='https://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-napoleon',
    license='BSD',
    author='Rob Ruana',
    author_email='rob@relentlessidiot.com',
    description=__doc__,
    long_description=open('README.rst', 'r').read(),
    zip_safe=False,
    classifiers=[
        'Development Status :: 4 - Beta',
        'Environment :: Console',
        'Environment :: Web Environment',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=reqs,
    test_suite='nose.collector',
    tests_require=reqs_test,
    namespace_packages=['sphinxcontrib'],
    **extra
)
