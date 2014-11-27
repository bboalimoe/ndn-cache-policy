"""
    sphinxcontrib.autoprogram
    ~~~~~~~~~~~~~~~~~~~~~~~~~

    Documenting CLI programs.

    :copyright: Copyright 2014 by Hong Minhee
    :license: BSD, see LICENSE for details.

"""
import argparse
import collections
try:
    import builtins
except ImportError:
    import __builtin__ as builtins
import functools
import re
import unittest

from docutils import nodes
from docutils.parsers.rst.directives import unchanged
from docutils.statemachine import ViewList
from sphinx.util.compat import Directive
from sphinx.util.nodes import nested_parse_with_titles
from sphinx.domains import std

__all__ = ('BOOLEAN_OPTIONS', 'AutoprogramDirective', 'ScannerTestCase',
           'import_object', 'scan_programs', 'setup', 'suite')


def scan_programs(parser, command=[]):
    options = []
    for arg in parser._actions:
        if not (arg.option_strings or
                isinstance(arg, argparse._SubParsersAction)):
            name = (arg.metavar or arg.dest).lower()
            desc = (arg.help or '') % {'default': arg.default}
            options.append(([name], desc))
    for arg in parser._actions:
        if arg.option_strings:
            if isinstance(arg, (argparse._StoreAction,
                                argparse._AppendAction)):
                metavar = (arg.metavar or arg.dest).lower()
                names = ['{0} <{1}>'.format(option_string, metavar)
                         for option_string in arg.option_strings]
            else:
                names = list(arg.option_strings)
            desc = (arg.help or '') % {'default': arg.default}
            options.append((names, desc))
    yield command, options, parser.description or ''
    if parser._subparsers:
        choices = parser._subparsers._actions[-1].choices.items()
        if not (hasattr(collections, 'OrderedDict') and
                isinstance(choices, collections.OrderedDict)):
            choices = sorted(choices, key=lambda pair: pair[0])
        for cmd, sub in choices:
            if isinstance(sub, argparse.ArgumentParser):
                for program in scan_programs(sub, command + [cmd]):
                    yield program


def import_object(import_name):
    module_name, expr = import_name.split(':', 1)
    mod = __import__(module_name)
    reduce_ = getattr(functools, 'reduce', None) or reduce
    mod = reduce_(getattr, module_name.split('.')[1:], mod)
    globals_ = builtins
    if not isinstance(globals_, dict):
        globals_ = globals_.__dict__
    return eval(expr, globals_, mod.__dict__)


class AutoprogramDirective(Directive):

    has_content = False
    required_arguments = 1
    option_spec = {'prog': unchanged}

    def make_rst(self):
        import_name, = self.arguments
        parser = import_object(import_name or '__undefined__')
        prog = self.options.get('prog', parser.prog)
        for commands, options, desc in scan_programs(parser):
            command = ' '.join(commands)
            title = '{0} {1}'.format(prog, command).rstrip()
            yield ''
            yield '.. program:: ' + title
            yield ''
            yield title
            yield ('!' if commands else '?') * len(title)
            yield ''
            yield desc
            yield ''
            for option_strings, help_ in options:
                yield '.. option:: {0}'.format(', '.join(option_strings))
                yield ''
                yield '   ' + help_.replace('\n', '   \n')
                yield ''

    def run(self):
        node = nodes.section()
        node.document = self.state.document
        result = ViewList()
        for line in self.make_rst():
            result.append(line, '<autoprogram>')
        nested_parse_with_titles(self.state, result, node)
        return node.children


def patch_option_role_to_allow_argument_form():
    """Before Sphinx 1.2.2, :rst:dir:`.. option::` directive hadn't
    allowed to not start with a dash or slash, so it hadn't been possible
    to represent positional arguments (not options).

    https://bitbucket.org/birkenfeld/sphinx/issue/1357/

    It monkeypatches the :rst:dir:`.. option::` directive's behavior.

    """
    std.option_desc_re = re.compile(r'((?:/|-|--)?[-_a-zA-Z0-9]+)(\s*.*)')


def setup(app):
    app.add_directive('autoprogram', AutoprogramDirective)
    patch_option_role_to_allow_argument_form()


class ScannerTestCase(unittest.TestCase):

    def test_simple_parser(self):
        parser = argparse.ArgumentParser(description='Process some integers.')
        parser.add_argument('integers', metavar='N', type=int, nargs='*',
                            help='an integer for the accumulator')
        parser.add_argument('-i', '--identity', type=int, default=0,
                            help='the default result for no arguments '
                                 '(default: 0)')
        parser.add_argument('--sum', dest='accumulate', action='store_const',
                            const=sum, default=max,
                            help='sum the integers (default: find the max)')
        programs = scan_programs(parser)
        programs = list(programs)
        self.assertEqual(1, len(programs))
        pair, = programs
        program, options, desc = pair
        self.assertEqual([], program)
        self.assertEqual('Process some integers.', desc)
        self.assertEqual(4, len(options))
        self.assertEqual(
            (['n'], 'an integer for the accumulator'),
            options[0]
        )
        self.assertEqual(
            (['-h', '--help'], 'show this help message and exit'),
            options[1]
        )
        self.assertEqual(
            (['-i <identity>', '--identity <identity>'],
             'the default result for no arguments (default: 0)'),
            options[2]
        )
        self.assertEqual(
            (['--sum'], 'sum the integers (default: find the max)'),
            options[3]
        )

    def test_subcommands(self):
        parser = argparse.ArgumentParser(description='Process some integers.')
        subparsers = parser.add_subparsers()
        max_parser = subparsers.add_parser('max', description='Find the max.')
        max_parser.set_defaults(accumulate=max)
        max_parser.add_argument('integers', metavar='N', type=int, nargs='+',
                                help='An integer for the accumulator.')
        sum_parser = subparsers.add_parser('sum',
                                           description='Sum the integers.')
        sum_parser.set_defaults(accumulate=sum)
        sum_parser.add_argument('integers', metavar='N', type=int, nargs='+',
                                help='An integer for the accumulator.')
        programs = scan_programs(parser)
        programs = list(programs)
        self.assertEqual(3, len(programs))
        # main
        program, options, desc = programs[0]
        self.assertEqual([], program)
        self.assertEqual('Process some integers.', desc)
        self.assertEqual(1, len(options))
        self.assertEqual(
            (['-h', '--help'],
             'show this help message and exit'),
            options[0]
        )
        # max
        program, options, desc = programs[1]
        self.assertEqual(['max'], program)
        self.assertEqual('Find the max.', desc)
        self.assertEqual(2, len(options))
        self.assertEqual((['n'], 'An integer for the accumulator.'),
                         options[0])
        self.assertEqual(
            (['-h', '--help'],
             'show this help message and exit'),
            options[1]
        )
        # sum
        program, options, desc = programs[2]
        self.assertEqual(['sum'], program)
        self.assertEqual('Sum the integers.', desc)
        self.assertEqual(2, len(options))
        self.assertEqual((['n'], 'An integer for the accumulator.'),
                         options[0])


class UtilTestCase(unittest.TestCase):

    def test_import_object(self):
        cls = import_object('sphinxcontrib.autoprogram:UtilTestCase')
        self.assertTrue(cls is UtilTestCase)
        instance = import_object(
            'sphinxcontrib.autoprogram:UtilTestCase("test_import_object")'
        )
        self.assertIsInstance(instance, UtilTestCase)

    if not hasattr(unittest.TestCase, 'assertIsInstance'):
        def assertIsInstance(self, instance, cls):
            self.assertTrue(isinstance(instance, cls),
                            '{0!r} is not an instance of {1.__module__}.'
                            '{1.__name__}'.format(instance, cls))


suite = unittest.TestSuite()
suite.addTests(
    unittest.defaultTestLoader.loadTestsFromTestCase(ScannerTestCase)
)
suite.addTests(unittest.defaultTestLoader.loadTestsFromTestCase(UtilTestCase))
