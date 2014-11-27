# -*- coding: utf-8 -*-

from setuptools import setup, find_packages

long_desc = '''
This package contains the actdiag Sphinx extension.

.. _Sphinx: http://sphinx.pocoo.org/
.. _actdiag: http://blockdiag.com/en/actdiag/

This extension enable you to insert activity diagrams in your Sphinx document.
Following code is sample::

   .. actdiag::

      diagram {
        A -> B -> C -> D;

        lane {
          A; B;
        }
        lane {
          C; D;
        }
      }


This module needs actdiag_.
'''

requires = ['actdiag>=0.5.3', 'Sphinx>=0.6', 'setuptools']

setup(
    name='sphinxcontrib-actdiag',
    version='0.7.2',
    url='http://bitbucket.org/birkenfeld/sphinx-contrib',
    download_url='http://pypi.python.org/pypi/sphinxcontrib-actdiag',
    license='BSD',
    author='Takeshi Komiya',
    author_email='i.tkomiya@gmail.com',
    description='Sphinx "actdiag" extension',
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
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Topic :: Documentation',
        'Topic :: Utilities',
    ],
    platforms='any',
    packages=find_packages(),
    include_package_data=True,
    install_requires=requires,
    namespace_packages=['sphinxcontrib'],
)
