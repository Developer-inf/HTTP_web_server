async function sign_in(event)
{
    event.preventDefault();
    let xhr = new XMLHttpRequest();
    
    xhr.onreadystatechange = state => {
        if (xhr.status === 200) {
            window.location.href = '/';
        } else if (xhr.status === 500) {
            alert("Данного пользователя не существует!");
        }
    } // err handling
    xhr.timeout = 5000;
    xhr.open("POST", '/sign-in');
    let req = `login=${login.value}&pass=${pass.value}`;
    xhr.send(req);
}

submit.onclick = sign_in;