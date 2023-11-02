async function SendFile(inp)
{
    let xhr = new XMLHttpRequest();
    let formData = new FormData();
    let data = file.files[0];
    
    formData.append("data", data);
    
    xhr.onreadystatechange = state => {
        if (xhr.status === 200) {
            window.location.href = '/';
        }
    } // err handling
    xhr.timeout = 5000;
    xhr.open("POST", '/load-file');
    let txt = await data.text();
    let resp = `${data.name}\n${txt}`;
    xhr.send(resp);
}