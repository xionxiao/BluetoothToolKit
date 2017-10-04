/* Use worker script as connectToDevice() doesn't return until finish
 * connectToDevice is blocks the listview refresh
 */
WorkerScript.onMessage = function(msg) {
    var param = msg.params.get(0)
    console.log(param, param.dm, param.device)
    var services = param.dm.connectToDevice(param.device)
    for (var i=0; i<services.length; i++) {
        console.log(services[i].uuid, services[i].name)
    }
}
