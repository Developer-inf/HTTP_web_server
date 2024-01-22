function _(el) {
return document.getElementById(el);
}

function uploadFile() {
    _("progressBar").value = 0;
    _("status").innerHTML = "0% загружено... Пожалуйста, подождите";
    
    var file = _("file").files[0];
    var formdata = new FormData();
    formdata.append("file", file);
    
    var ajax = new XMLHttpRequest();
    ajax.upload.addEventListener("progress", progressHandler, false);
    ajax.addEventListener("load", completeHandler, false);
    ajax.addEventListener("error", errorHandler, false);
    ajax.addEventListener("abort", abortHandler, false);
    ajax.open("POST", "/load-file");
    
    ajax.send(formdata);
}

function progressHandler(event) {
    _("loaded_n_total").innerHTML = "Загружено " + event.loaded + " байт из " + event.total;
    var percent = (event.loaded / event.total) * 100;
    _("progressBar").value = Math.round(percent);
    _("status").innerHTML = Math.round(percent) + "% загружено... Пожалуйста, подождите";
}

function completeHandler(event) {
    _("status").innerHTML = "Загрузка завершена!";
    // _("progressBar").value = 0; //wil clear progress bar after successful upload
}

function errorHandler(event) {
    _("status").innerHTML = "Ошибка загрузки";
    _("progressBar").value = 0;
    _("loaded_n_total").innerHTML = "";
}

function abortHandler(event) {
    _("status").innerHTML = "Загрузка отменена";
}