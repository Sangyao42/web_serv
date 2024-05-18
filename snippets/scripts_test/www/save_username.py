import json
import sys
from sys import stdin

# body = input()
# user_name_json_obj = json.loads(body)

# user_name = user_name_json_obj["userName"]

# if not user_name:
#     exit (1)

# f = open('usernames/usernames.txt', 'a')
# f.write(user_name + '\n')
# f.close()
# print("Content-Type=text/html\r\n\r\n")
# print("Registration Success!")

def save_registry():
    # read from stdin. TODO: need test with execve
    body = sys.stdin.read()
    user_name_json_obj = json.loads(body)
    user_name = user_name_json_obj["userName"]
    if not user_name:
        sys.exit(1)

	# write to file
    f = open('usernames/usernames.txt', 'a+')
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


