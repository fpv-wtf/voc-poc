var usb = require('usb')

var goggles = usb.findByIds("0x2ca3", "0x1f")
console.log(goggles)
goggles.open()
console.log(goggles.interfaces)
var interface = goggles.interface(3)
console.log(interface)
interface.claim()


const fs = require('fs');

// specify the path to the file, and create a buffer with characters we want to write
let path = 'out.bin';

// open the file in writing mode, adding a callback function where we do the actual writing
fs.open(path, 'w', function(err, fd) {
    if (err) {
        throw 'could not open file: ' + err;
    }
    var inpoint = interface.endpoints[1]
    inpoint.timeout = 100


    var outpoint = interface.endpoints[0]
    //var magic = Buffer.from("54564d52", "hex")
    var magic = Buffer.from("524d5654", "hex")
    console.log(magic)

    outpoint.transfer(magic, function() {
        console.log("magic written")
        /*inpoint.transfer(20000, function(error, data){
            console.log("error", error)
            console.log("data", data)
            setTimeout(getData)
        })*/
  
    })
    inpoint.addListener("data", function(data) {
        console.log(data)
        fs.writeSync(fd, data)
    })
    inpoint.addListener("error", function(error) {
        console.log(error)
    })
    inpoint.startPoll(3, 20000)
    
    // write the contents of the buffer, from position 0 to the end, to the file descriptor returned in opening our file


});


/*endpoint.transfer(512, function(error, data){
    console.log(error)
    console.log(data)
})*/