import urllib
import urllib2
try:
    import json
except ImportError:
    import simplejson as json
import sys
import StringIO, gzip

data = sys.stdin.readlines()

g = {'public': False, 'files': {('%s.csv'%sys.argv[1]) : {'content': ''.join(data)}}}
req = urllib2.Request('https://api.github.com/gists')
req.add_header('Content-Type', 'application/json')

class BetterHTTPErrorProcessor(urllib2.BaseHandler):
    # a substitute/supplement to urllib2.HTTPErrorProcessor
    # that doesn't raise exceptions on status codes 201,204,206
    def http_error_201(self, request, response, code, msg, hdrs):
        return response
    def http_error_204(self, request, response, code, msg, hdrs):
        return response
    def http_error_206(self, request, response, code, msg, hdrs):
        return response

opener = urllib2.build_opener(BetterHTTPErrorProcessor)
urllib2.install_opener(opener)

response = urllib2.urlopen(req, json.dumps(g))
gist = json.loads(response.read())

collection = 'msp'
url = 'https://api.keen.io/3.0/projects/535b6356d97b8555b0000028/events/%s?api_key=570d50945324d46d47480b7f410afd1dfaffc6f4f09ad9ec531b9fa6c3e85c919c0678cc1bf721e760867d2ba5bc91716c3eba6bf0b3d7fc8c4865c73c41e34ae18ac19e4a37939d176004eda2d4ea6e56f810312309cc11703b5bc870fd712cd08c0ea213f99ccb6a1b8eb1d292bd9e'%collection
values = {'name': sys.argv[1],
          'out': open('%s.err'%sys.argv[1]).read().strip(),
          'profile': gist['files'][('%s.csv'%sys.argv[1])]['raw_url']}
print sys.argv[1]
req = urllib2.Request(url)
req.add_header('Content-Type', 'application/json')
response = urllib2.urlopen(req, json.dumps(values))

the_page = response.read()
print the_page
