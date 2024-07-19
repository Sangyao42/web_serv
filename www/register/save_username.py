import json
import sys
import os
from sys import stdin

def save_registry():
    root = os.environ['DOCUMENT_ROOT']

    # read from stdin.
    body = sys.stdin.read()

    user_name_json_obj = json.loads(body)
    user_name = user_name_json_obj["user_name"]
    if not user_name:
        sys.exit(1)

	# write to file
    f = open(root + '/usernames/usernames.txt', 'a+')
    f.write(user_name + '\n')
    f.close()
    print("Content-Type: text/html", end='\r\n')
    print("Location: ",end='\r\n\r\n')
    print("<!DOCTYPE html><html><body><h1>Registration Success!</h1></body></html>", end='')
    return

if __name__ == "__main__":
    try:
        save_registry()
    except:
        sys.exit(1)


