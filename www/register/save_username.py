import json
import sys
import os
from sys import stdin

def save_registry():
    # TODO: get document root from parent process
    # root = os.environ['DOCUMENT_ROOT']
    root = './www'
    # read from stdin. TODO: need test with execve
    # TODO: for testing stdin
    body = input()
    # body = sys.stdin.read()
    user_name_json_obj = json.loads(body)
    user_name = user_name_json_obj["userName"]
    if not user_name:
        sys.exit(1)

	# write to file
    f = open(root + '/usernames/usernames.txt', 'a+')
    f.write(user_name + '\n')
    f.close()
    print("Content-Type=text/plain\r\n\r\n")
    print("Registration Success!")
    return

if __name__ == "__main__":
    try:
        save_registry()
    except:
        sys.exit(1)


