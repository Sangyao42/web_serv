import os
import sys
import traceback

def index():
	# get root from envp
	root = os.environ['DOCUMENT_ROOT']

	upload_folder = root + '/upload'
	files = os.listdir(upload_folder)

	# for generating uri
	dir_path = '/upload'

	# load script for list all the files
	dir_cgi = os.path.dirname(__file__)
	script_file = open( os.path.join(dir_cgi, 'del.js'), "r")
	print(os.path.join(dir_cgi, 'del.js'), file=sys.stderr)

	script = script_file.read()

	html = "<!DOCTYPE html>\n<html><body><h1>Directory Listing</h1>"
	for file in files:
		if file != '.' and file != '..':
			file_path = os.path.join(dir_path, file)
			html += f"<ul><li><a href='{file_path}'>{file}</a></li>"
			html += f"<input type='button' value='delete' id='delete' onclick='del_file(\"{file_path}\")'></ul>"
	html += "</body>\n"
	html += "<script>" + script + "</script></html>"

	# # TODO: For testing generated html, can be deleted aftertesting
    f = open( os.path.join(dir_cgi, 'delete.html'), "w+")
    f.write(html)
    f.close()
	# # End of TODO

	print("Content-Type: text/html", end='\r\n\r\n')
	print(html, end='')


if __name__ == '__main__':
	try:
		index()
	except:
		# log = open("errortrace.log", "w+")
		# log.write(traceback.format_exc())
		# log.close()
		sys.exit(1)
