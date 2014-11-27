# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the googlemaps Sphinx extension.

This extension enable you to embed maps using `Google Maps`_ .
Following code is sample::

   .. googlemaps:: Shibuya Station


.. _Google Maps: http://maps.google.com/
'''

requires = ['Sphinx>=0.6', 'setuptools']

setup(
    name='sphinxcontrib-googlemaps',
    version='0.1.0',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-googlemaps',
    license='BSD',
    author='Takeshi KOMIYA',
    author_email='i.tkomiya@gmail.com',
    description='Sphinx "googlemaps" extension',
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
)
