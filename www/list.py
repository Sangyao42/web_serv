from flask import Flask, render_template_string
import os
import sys

app = Flask(__name__)

# @app.route('/')
def index():
    dir = os.path.dirname(__file__)
    folder = os.path.join(dir, 'upload')
    dir_path = '/upload'  # Change this to your folder path
    files = os.listdir(folder)

	# load script for list all the files
    script_file = open( os.path.join(dir, 'del.js'), "r")
    script = script_file.read()

    html = "<html><body><h1>Directory Listing</h1>"
    for file in files:
        if file != '.' and file != '..':
            file_path = os.path.join(dir_path, file)
            html += f"<ul><li><a href='{file_path}'>{file}</a></li>"
            html += f"<input type='button' value='delete' id='delete' onclick='del_file(\"{file_path}\")'></ul>"
            # html += f"<p id='{file}'></p>"



    # for file in files:
    #     if file != '.' and file != '..':
    #         script_clean = " \
    #     function delete()\
    #     {const fileToDelete = " + f"document.getElementById('{file}')" +";\
	# 	const fileLocation = fileToDelete.href; \
    #     const deleteBtn = document.querySelector('input[type=\"button\"]');\
    #         deleteBtn.addEventListener('click', () => { \
    #             fetch(fileLocation, { \
    #                 method: 'DELETE', \
    #             }) \
    #         .then(response => response.text()) \
    #         .then(data => { \
    #             console.log(data); \
    #         }) \
    #         .catch(error => { \
    #             console.error(error); \
    #         }); \
    #     }); \
    # } \
    # document.addEventListener('DOMContentLoaded', del_file); \
    # "
    #         html += "<script>" + script_clean
    #         html +=  "</script>"
    # html += "</body></html>"
    html += "</body>\n"
    html += "<script>" + script + "</script></html>"

    f = open( os.path.join(dir, 'delete1.html'), "w")
    f.write(html)
    f.close()
    print(html)


if __name__ == '__main__':
    try:
        index()
    except:
        sys.exit(1)
