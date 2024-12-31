import os
import json

input_path = '/Users/qinziwen/Downloads/taiqiu/432770454-1-208'
res_file = '/Users/qinziwen/Downloads/taiqiu/432770454-1-208/432770454-1-208_0.json'

with open(res_file, 'r', encoding='utf-8') as file:
    data = json.load(file)

for root, dirs, files in os.walk(input_path):
    for file in files:
        if file.endswith('.jpg'):
            file_name = file.split('.')[0]
            data['imagePath'] = file_name + '.jpg'
            with open(os.path.join(root, file_name + '.json'), 'w', encoding='utf-8') as f:
                json.dump(data, f, ensure_ascii=False, indent=4)
    break