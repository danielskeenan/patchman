# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import json
from datetime import date

# -- Build info --------------------------------------------------------------
with open('build_info.json') as f:
    build_info = json.load(f)

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = build_info['name']
copyright = '{year}, {author}'.format(year=date.today().year, author=build_info['author'])
author = build_info['author']
# The full version, including alpha/beta/rc tags
version = build_info['version']
release = version

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = []

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'CMakeLists.txt', 'requirements.txt', '*.json', '.venv']

rst_prolog = """
.. |version| replace:: {version}
""".format(version=version)

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# https://github.com/executablebooks/sphinx-book-theme
html_theme = 'sphinx_book_theme'
html_static_path = ['_static']
html_theme_options = {
    'navigation_with_keys': False,
    'repository_url': 'https://github.com/danielskeenan/patchman',
    'use_source_button': False,
    'use_edit_page_button': False,
    'use_repository_button': True,
    'use_issues_button': True,
    'use_download_button': False,
    'use_fullscreen_button': False,
    'path_to_docs': 'doc/',
    'repository_branch': 'main',
}

# -- Options for QtHelp output -------------------------------------------------

# These are automatically set when the docs are generated using CMake.
qthelp_basename = build_info['name']
qthelp_namespace = 'org.dankeenan.{}'.format(build_info['name'])
