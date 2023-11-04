fetch("check-data", { method: "POST" })
.then(resp => resp.text())
.then(txt => {
    if (txt.length <= 0) {
        return;
    }
    
    let tbody = document.getElementsByTagName("tbody")[0];
    let i = 1;
    
    for (let rec of txt.split(`\n`)) {
        let [key, val] = rec.split(';');
        let row = `<td>${i}</td><td>${key}</td><td>${val}</td>`;
        i++;
        tbody.innerHTML += `<tr>${row}</tr>`;
    }
});