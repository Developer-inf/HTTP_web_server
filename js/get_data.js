fetch("check-data", {
    method: "POST"
}).then(v => {
    
    // v.forEach(e => {
    //     let tr = document.createElement("tr");
    //     let str = document.createElement("td");
    //     str.innerHTML = e.
    //     tbody.appendChild()
    // });
    
    return v.json();
}).then(r => {
    let tbody = document.getElementsByTagName("tbody")[0];
    let i = 1;
    
    for (let key in r) {
        let row = `<td>${i}</td><td>${key}</td><td>${r[key]}</td>`;
        i++;
        tbody.innerHTML += `<tr>${row}</tr>`;
    }
});