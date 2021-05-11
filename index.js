const usb = require('usb')
const yargs = require('yargs')
const fs = require('fs')


const argv = yargs
    .option('f', {
        alias: 'file',
        describe: 'output video feed to file',
        type: 'string'
    })
    .option('o', {
        alias: 'stdout',
        describe: 'send video feed to stdout for playback. eg: node index.js -o | ffplay -',
        type: 'boolean'
    })
    .option('s', {
        alias: 'readsize',
        describe: 'size in bytes to queue for usb bulk interface reads',
        default: 512,
        type: 'integer'
    })
    .option('q', {
        alias: 'queuesize',
        describe: 'number of polling usb bulk read requests to keep in flight',
        default: 3,
        type: 'integer'
    })
    .option('v', {
        alias: 'verbose',
        describe: 'be noisy - doesn not play well with -o',
        type: 'boolean'
    })
    .help()
    .alias('help', 'h')
    .argv;

var goggles = usb.findByIds("0x2ca3", "0x1f")

if(!goggles) {
    console.error("Goggles USB device not found. Please connect your goggles and restart the script.")
    process.exit(1)
}
goggles.open()
if(!goggles.interfaces) {
    console.error("Couldn't open Goggles USB device")
    process.exit(1)
}
var interface = goggles.interface(3)
interface.claim()
if(!interface.endpoints) {
    console.error("Couldn't claim bulk interface")
    process.exit(1)
}

if(!argv.f && !argv.o) {
    console.log("warning: no outputs specified")
    argv.v = true
}

var fd

var inpoint = interface.endpoints[1]
inpoint.timeout = 100
//outpoint.timeout = 200

var outpoint = interface.endpoints[0]
var magic = Buffer.from("524d5654", "hex")

outpoint.transfer(magic, function(error) {
    if(error) {
        console.error(error)
    }
    console.debug("send magic bytes")

})
inpoint.addListener("data", function(data) {
    if(argv.o) {
        process.stdout.write(data)
    }
    if(argv.v) {
        console.debug("received "+data.length+" bytes")
    }
    if(argv.f) {
        if(!fd) {
            fd = fs.openSync(argv.f, "w")
            if(!fd) {
                console.error("couldn't open file "+ argv.f + ": "+err)
                process.exit(1)
            }
        }
        fs.writeSync(fd, data)
    }
})
inpoint.addListener("error", function(error) {
    console.error(error)
})
inpoint.startPoll(argv.q, argv.s)

