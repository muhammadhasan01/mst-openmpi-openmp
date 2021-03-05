import os
import sys
import requests

nim = sys.argv[1]

url = 'http://100.25.13.77/api/v1/testcase'

params = {
    'nim': nim,
}

response = requests.get(url = url, params = params)

print('Status Code:', response.status_code)
if response.status_code != 200:
    sys.exit(-1)

data = response.json()

for tc in data['set']:
    total_node = tc['total_node']
    tc_id = tc['testcase_id']
    content = tc['question']
    filename = str(total_node) + '_' + str(tc_id)
    f = open(os.path.join('tmp', 'testcases', filename), 'w')
    f.write(content)
    f.close()
