import os
import sys
from datetime import datetime

def generateFileName():
	# based on the time
	# return string

	now = datetime.now() # current date and time
	file_name = now.strftime("CGI-%Y-%m-%dT%H-%M-%SZ")
	return file_name

def generateFileExtension():
	# based on Content-Type
	try:
		content_type = os.environ['CONTENT_TYPE']
	except:
		print("CGI: No CONTENT_TYPE env", file=sys.stderr)
	print("CGI: CONTENT_TYPE:", content_type, file=sys.stderr)

	if content_type == "text/plain":
		file_extent = "txt"
	elif content_type == "text/html":
		file_extent = "html"
	elif content_type == "image/jpeg":
		file_extent = "jpg"
	elif content_type == "image/png":
		file_extent = "png"
	else:
		file_extent = ""

	return file_extent


def uploadFile():

	try:
		root = os.environ['DOCUMENT_ROOT']
	except:
		print("CGI: No root env", file=sys.stderr)
	print("CGI: ROOT:", root, file=sys.stderr)

	upload_folder = root + '/upload/'

	file_name = generateFileName()
	file_extent = generateFileExtension()
	if file_extent != "":
		full_file_name = file_name + '.' + file_extent
	else:
		full_file_name = file_name
	print("CGI: full file name:", full_file_name,  file=sys.stderr)

	full_path = upload_folder + full_file_name
	print("CGI: full path:", full_path,  file=sys.stderr)

	try:
		os.makedirs(upload_folder, 0o777, exist_ok=True)
	except OSError as e:
		print("CGI: makedirs failed", file=sys.stderr)
		print(e, file=sys.stderr)

	# read from stdin.
	# body = sys.stdin.read()
	body = sys.stdin.buffer.read()
	if body == "":
		print("CGI: nothing read from parent process", file=sys.stderr)
		sys.exit(1)
	# body = "12345678912345"

	try:
		# write to file
		f = open(full_path,'wb+')
		f.write(body)
		print("try write to file", file=sys.stderr)
		f.close()
	except e:
		print("write to file failed", file=sys.stderr)
		print(e, file=sys.stderr)

	# send to parent process
	print("Content-Type: text/html", end='\r\n')
	print("Location:", full_path, end='\r\n\r\n')
	print("<!DOCTYPE html><html><body><h1>CGI Upload Success!</h1></body></html>", end='')
	return


if __name__ == '__main__':
	method = os.environ['REQUEST_METHOD']
	if method == 'GET' or method == 'DETELE':
		print("CGI: method not allowed: " + method, file=sys.stderr)
		sys.exit(1)

	try:
		uploadFile()
		# print("CGI: upload file failed.", file=sys.stderr)

		# TEST
		# test_filename =	generateFileName()
		# print(test_filename, file=sys.stderr)
		# END OF TEST
	except:
		# log = open("errortrace.log", "w+")
		# log.write(traceback.format_exc())
		# log.close()
		sys.exit(1)
