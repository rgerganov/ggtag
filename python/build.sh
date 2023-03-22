#!/bin/bash

set -o xtrace

rm -rf venv build dist host shared ggtag.egg-info
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip
pip install twine
cp -r ../host .
cp -r ../shared .
python setup.py sdist

#twine upload dist/*

