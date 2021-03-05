import os
import sys
import requests

from os import walk

nim = sys.argv[1]
openmp_dir = sys.argv[2]

url = 'http://100.25.13.77/api/v1/check'

body = {
    'nim': nim,
    'is_openmp': True,
    'answers': [],
}

_, _, openmp_files = next(walk(openmp_dir))

for file in openmp_files:
    total_node = file.split('_')[0]
    testcase_id = file.split('_')[1]
    f = open(os.path.join(openmp_dir, file), 'r')
    content = f.read()
    f.close()
    answer = {
        'total_node': int(total_node),
        'testcase_id': int(testcase_id),
        'answer': content,
    }
    body['answers'].append(answer)

response = requests.post(url = url, json = body)

print('OpenMP Status Code:', response.status_code)
if response.status_code != 200:
    sys.exit(-1)
else:
    data = response.json()
    results = data['results']
    for result in results:
        print('Total Node:', result['total_node'])
        print('Correct:', result['is_accepted'])
