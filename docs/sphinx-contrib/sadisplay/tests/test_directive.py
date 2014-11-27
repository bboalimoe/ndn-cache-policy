# -*- coding: utf-8 -*-
from docutils.parsers.rst import Parser
from docutils.utils import new_document
from docutils.frontend import OptionParser
from sphinx.application import Sphinx
from sphinx.errors import SphinxWarning
from sphinx.config import Config

from nose.tools import raises

from sphinxcontrib import sadisp


_parser, _settings = None, []


class FakeSphinx(Sphinx):
    def __init__(self):
        self.config = Config(None, None, None, None)


def setup():
    global _parser, _settings
    _parser = Parser()
    _settings = OptionParser().get_default_values()
    _settings.tab_width = 8
    _settings.pep_references = False
    _settings.rfc_references = False
    app = FakeSphinx()
    sadisp.setup(app)


def with_parsed(func):
    def test():
        doc = new_document('<test>', _settings)
        src = '\n'.join(l[4:] for l in func.__doc__.splitlines()[2:])
        _parser.parse(src, doc)
        func(doc.children)
    #test.func_name = func.func_name
    test.__name__ = func.__name__
    return test


@with_parsed
def test_simple(nodes):
    """Simple sadisplay directive

    .. sadisplay::
        :module: myapp.model, myapp.model2
        :link:
    """
    n = nodes[0]
    assert n['alt'] is None
    assert n['link'] is True
    assert n['render'] == None
    assert list(n['module']) == ['myapp.model', 'myapp.model2']
    assert list(n['include']) == []
    assert list(n['exclude']) == []


@with_parsed
def test_include(nodes):
    """Test sadisplay directive
    with include

    .. sadisplay::
        :module: myapp.model
        :include: User, Group
    """
    n = nodes[0]
    assert n['alt'] is None
    assert n['link'] is False
    assert n['render'] == None
    assert ['myapp.model'] == ['myapp.model']
    assert list(n['module']) == ['myapp.model']
    assert list(n['include']) == ['User', 'Group']


@with_parsed
def test_render(nodes):
    """Test sadisplay directive
    with include

    .. sadisplay::
        :module: myapp.model
        :render: graphviz
    """
    n = nodes[0]
    assert n['render'] == 'graphviz'


@raises(SphinxWarning)
@with_parsed
def test_warning(nodes):
    """Test sadisplay directive
    with include and exclude

    .. sadisplay::
        :module: myapp.model
        :include: User, Group
        :exclude: Permission
    """
    pass
