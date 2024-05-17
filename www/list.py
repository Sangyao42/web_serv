from flask import Flask, render_template_string
import os

app = Flask(__name__)

@app.route('/')
def index():
    directory = 'www/upload'  # Change this to your folder path
    files = os.listdir(directory)

    html = "<html><body><h1>Directory Listing</h1>"
    for file in files:
        if file != '.' and file != '..':
            file_path = os.path.join(directory, file)
            html += f"<ul><li><a href='\{file_path}' id='{file}'>{file}</a></li>"
            html += "<input type='button' value='delete' id='delete'></ul>"
    # script_file = open("del.js","r")
    # script = script_file.read()
    # script_clean = script.strip()
    html += "</body>"

    for file in files:
        if file != '.' and file != '..':
            script_clean = " \
        function main()\
        {const fileToDelete = " + f"document.getElementById('{file}')" +";\
		const fileLocation = fileToDelete.href; \
        const deleteBtn = document.querySelector('input[type=\"button\"]');\
            deleteBtn.addEventListener('click', () => { \
                fetch(fileLocation, { \
                    method: 'DELETE', \
                }) \
            .then(response => response.text()) \
            .then(data => { \
                console.log(data); \
            }) \
            .catch(error => { \
                console.error(error); \
            }); \
        }); \
    } \
    document.addEventListener('DOMContentLoaded', main); \
    "
            html += "<script>" + script_clean
            html +=  "</script>"
    html += "</html>"

    f = open("www/delete1.html", "w")
    f.write(html)
    f.close()
    print(html)


if __name__ == '__main__':
    # app.run(host='0.0.0.0', port=8080)
	index()
