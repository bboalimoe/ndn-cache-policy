#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
    sphinxcontrib.feed unit test driver
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This script runs the sphinxcontrib.feed unit test suite.

    :copyright: Copyright 2007-2009 by the sphinxcontrib.feed team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import sys
from os import path
import unittest

def run(bonus_args=[]):
    # always test the sphinxcontrib.feed package from this directory
    sys.path.insert(0, path.join(path.dirname(__file__), path.pardir))
    sys.path.insert(1, path.abspath(
      path.join(path.dirname(__file__),
      path.pardir,
      'sphinxcontrib', 'feed'))
    )

    try:
        import sphinx
    except ImportError:
        print "The sphinx package is needed to run the sphinxcontrib.feed test suite."
    
    import test_feed
    
    print "Running sphinxcontrib.feed test suite..."
    
    suite = unittest.TestLoader().loadTestsFromTestCase(
      test_feed.TestFeedStructure
    )
    unittest.TextTestRunner(verbosity=2).run(suite)

if __name__ == '__main__':
    run()