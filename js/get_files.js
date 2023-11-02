let units_list = ["B", "Kb", "Mb", "Gb", "Tb"];

fetch("/get-files", { method: "GET" })
.then(resp => resp.text())
.then(txt => {
    let tbody = document.getElementsByTagName("tbody")[0];
    tbody.innerHTML = "";
    
    for (let pair of txt.split('\n')) {
        let [filename, size] = pair.split(':');
        size = Number(size);
        let units = 0;
        while (size / 1024 > 1) {
            units++;
            size = (size / 1024.0).toFixed(2);
        }
        tbody.innerHTML += `<tr><td>${filename}</td><td>${size} ${units_list[units]}</td></tr>`;
    }
});