import json

body = input()
user_name_json_obj = json.loads(body)

user_name = user_name_json_obj["userName"]

if not user_name:
    exit (1)

f = open('usernames/usernames.txt', 'a')
f.write(user_name + '\n')
f.close()
print("Content-Type=text/html\r\n\r\n")
print("Registration Success!")

