fetch("check-data", {
    method: "POST"
}).then(v => v.text())
.then(r => {
    let tbody = document.getElementsByTagName("tbody")[0];
    let i = 1;
    for (let rec of r.split(`\n`)) {
        let [key, val] = rec.split(';');
        let row = `<td>${i}</td><td>${key}</td><td>${val}</td>`;
        i++;
        tbody.innerHTML += `<tr>${row}</tr>`;
    }
});