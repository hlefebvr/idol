# Configuration file for the Sphinx docs builder.
#
# For the full list of built-in configuration values, see the docs:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# docs root, use os.path.abspath to make it absolute, like shown here.
#

from sphinx.builders.html import StandaloneHTMLBuilder
import subprocess, os

# Doxygen
subprocess.call('doxygen Doxyfile.in', shell=True)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'idol'
copyright = '2025, Henri Lefebvre'
author = 'Henri Lefebvre'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.coverage',
    'sphinx.ext.ifconfig',
    'sphinx.ext.viewcode',
    'sphinx_sitemap',
    'sphinx.ext.inheritance_diagram',
    'breathe',
    'sphinx.ext.mathjax',
    'sphinx_copybutton',
    'sphinxmermaid',
    'sphinxcontrib.bibtex'
]
bibtex_bibfiles = ['bibliography.bib']
bibtex_default_style = 'plain'

mathjax_path = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

highlight_language = 'c++'

html_css_files = [
    'design.css',
    'pseudocode-js/pseudocode.min.css'
]

html_js_files = [
    'pseudocode-js/pseudocode.min.js'
]

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the docs for
# a list of builtin themes.
#

html_theme = 'press'
html_theme_options = {
    "navigation_depth": -1,
    "titles_only": True,
    "style_external_links": True
}
pygments_style = "colorful"
html_title = "idol"
# html_logo = ''
github_url = 'https://github.com/hlefebvr/idol'
html_baseurl = 'https://hlefebvr.github.io/idol/'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# -- Breathe configuration -------------------------------------------------

breathe_projects = {
    "idol": "_build/xml/"
}
breathe_default_project = "idol"
breathe_default_members = ('members', 'undoc-members')