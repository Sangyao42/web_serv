function del_file(file_path)
{
	fetch('http://localhost:8080' + file_path, {
		method: 'DELETE',
	})
	.then(response => response.text())
	.then(data => {
		console.log(data);
	})
	.catch(error => {
		console.error(error);
	});
}
