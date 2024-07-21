import json
import sys
import os
from sys import stdin

def save_registry():
    root = os.environ['DOCUMENT_ROOT']
    # read from stdin.
    body = sys.stdin.read()
    if body == "":
        print("body is empty", file=sys.stderr)
        sys.exit(1)

    try:
        user_name_json_obj = json.loads(body)
    except:
        print("json.loads() failed", file=sys.stderr)
        sys.exit(1)
    user_name = user_name_json_obj["user_name"]

    if not user_name:
        print("user name is empty", file=sys.stderr)
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
    method = os.environ['REQUEST_METHOD']
    if method == 'GET' or method == 'DETELE':
        print("CGI: method not allowed: " + method, file=sys.stderr)
        sys.exit(1)
    try:
        save_registry()
    except:
        sys.exit(1)


