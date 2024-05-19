import os
import sys

def index():
    dir_cgi = os.path.dirname(__file__)
    dir_parent = os.path.dirname(dir_cgi)
    upload_folder = os.path.join( dir_parent, 'upload')
    files = os.listdir(upload_folder)

    dir_path = '/upload'

	# load script for list all the files
    script_file = open( os.path.join(dir_cgi, 'del.js'), "r")
    script = script_file.read()

    html = "<!DOCTYPE html>\n<html><body><h1>Directory Listing</h1>"
    for file in files:
        if file != '.' and file != '..':
            file_path = os.path.join('..', dir_path, file)
            html += f"<ul><li><a href='{file_path}'>{file}</a></li>"
            html += f"<input type='button' value='delete' id='delete' onclick='del_file(\"{file_path}\")'></ul>"
    html += "</body>\n"
    html += "<script>" + script + "</script></html>"

    f = open( os.path.join(dir_cgi, 'delete1.html'), "w+")
    f.write(html)
    f.close()
    # TODO: Add "Content-Type in html"
    print(html)


if __name__ == '__main__':
    try:
        index()
    except:
        sys.exit(1)
