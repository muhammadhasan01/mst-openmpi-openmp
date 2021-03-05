import os
import sys
import requests

from os import walk

nim = sys.argv[1]
filename = sys.argv[2]

url = 'http://100.25.13.77/api/v1/submit?nim={nim}'.format(nim = nim)
files = {
    'file': open(filename, 'rb')
}

response = requests.post(url = url, files = files)
print('Submission Status Code: {statusCode}'.format(statusCode = response.status_code))
