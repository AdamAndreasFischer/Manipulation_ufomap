"""
setup.py file
"""
from setuptools import setup, find_packages
import codecs
import os

here = os.path.abspath(os.path.dirname(__file__))

with codecs.open(os.path.join(here, "README.md"), encoding="utf-8") as fh:
    long_description = "\\n" + fh.read()

setup(
    name="ufopy",
    version='{{VERSION_PLACEHOLDER}}',
    author="Daniel Duberg",
    author_email="danielduberg@gmail.com",
    description="UFO",
    url = "https://github.com/UnknownFreeOccupied/ufo",
    long_description_content_type="text/markdown",
    long_description=long_description,
    packages=[],
    install_requires=[],
    keywords=['ufo', 'mapping', 'ros'],
    classifiers=[
        "Development Status :: 1 - Planning",
        "Intended Audience :: Developers",
        "Programming Language :: Python :: 3",
        "Operating System :: Unix",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows"
    ]
)
