
 function del_file(file_path)
    {
		// document.getElementById(file_path).innerHTML = file_path;
        // const fileToDelete = document.getElementById('filedel');
		const fileToDelete = file_path;
        const deleteBtn = document.querySelector('input[type="button"]');

        deleteBtn.addEventListener('click', () => {
            // const file = fileBtn.files[0];
            // readFile(file)
            // .then(fileContent => {
				// fileLocation = fileToDelete.href;
            fetch(fileToDelete, {
                method: 'DELETE',
            })
            // })
            .then(response => response.text())
            .then(data => {
                console.log(data);
            })
            .catch(error => {
                console.error(error);
            });
        });
    }

    // document.addEventListener('DOMContentLoaded', main);
