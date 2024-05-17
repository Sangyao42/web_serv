
    function main()
    {
        const fileToDelete = document.getElementById('filedel');
        const deleteBtn = document.querySelector('input[type="button"]');

        deleteBtn.addEventListener('click', () => {
            // const file = fileBtn.files[0];
            // readFile(file)
            // .then(fileContent => {
				fileLocation = fileToDelete.href;
                fetch(fileLocation, {
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

    document.addEventListener('DOMContentLoaded', main);
