async function SendFile(inp)
{
    const MAX_CHUNK_SIZE = 1024;
    let data = file.files[0];
    let txt = new Uint8Array(await data.arrayBuffer());
    
    let chunksCnt = Math.ceil(txt.length / MAX_CHUNK_SIZE);
    
    for (let i = 0; i < chunksCnt; i++) {
        let xhr = new XMLHttpRequest();
        
        xhr.onreadystatechange = state => {
            // if (xhr.status === 200) {
            //     window.location.href = '/';
            // }
        } // err handling
        xhr.timeout = 5000;
        xhr.open("POST", '/load-file');
        
        let respData = txt.slice(i * MAX_CHUNK_SIZE, (i + 1) * MAX_CHUNK_SIZE);
        let resp = `${data.name}\n${respData}`;
        // let resp = new Uint8Array(data.name)  + new Uint8Array('\n') + respData;
        
        xhr.setRequestHeader("Content-Type", "application/x-myapp");
        xhr.setRequestHeader("Content-Part", `${i}`);
        xhr.setRequestHeader("Content-Range", `${i * MAX_CHUNK_SIZE}-${i * MAX_CHUNK_SIZE + respData.length}/${txt.length}`);
        
        xhr.send(resp);
    }
    
    file.value = "";
}