WorkerScript.onMessage = function(msg) {
    var param = msg.params.get(0)
    console.log(param, param.dm, param.device)
    var services = param.dm.connectToDevice(param.device)/*, function() {
        console.log("callback called from C++")
    })*/
    for (var i=0; i<services.length; i++) {
        console.log(services[i].uuid, services[i].name)
    }
}
