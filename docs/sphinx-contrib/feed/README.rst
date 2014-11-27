.. -*- restructuredtext -*-

======================
README for sphinx-feed
======================

This Sphinx extension generates an RSS feed your site containing all articles
that occur therein, indexed by document date. Date is supplied as a standard
document metadata field. (The `ReST cheat sheet`_ has one, for example, or see
the test suite for other examples.)

Licence
=======

Incorporates feedgenerator.py from the Django project, licensed under the BSD
license.

The rest of the project is also released under the BSD licence.

Contributors
============

Original version by `Dan MacKinlay`_ with contributions by `Keegan
Carruthers-Smith`_. Contributors hereby release their code under the BSD
license.

TODO
====

* inject rel="alternate" RSS links into document header pointing to
  the RSS feed. For now you roll your own in the template
* make the `latest` directive more useful
* use sphinx-style documentation (!)

.. _ReST cheat sheet: http://docutils.sourceforge.net/docs/user/rst/cheatsheet.txt
.. _Dan MacKinlay: http://livingthing.org/
.. _Keegan Carruthers-Smith: http://people.cs.uct.ac.za/~ksmith/
