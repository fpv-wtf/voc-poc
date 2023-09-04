# ¯\\\_(ツ)_/¯
**voc-poc** is a proof of concept for video put from the DJI FPV Goggles over USB.

## Community projects
This proof of concept has lead to many awesome things:

 - [WideFPV](https://widefpv.com/) - an awesome Chrome based webapp with local and RTMP streaming, DVR, clean HDMI out and more.
 - [CosmoStreamer](https://cosmostreamer.com/products/djifpvgoggles/) - a paid, feature rich Raspberry PI image for HDMI out and streaming, including iOS support
 - [CosmoStreamer-NG](https://cosmostreamer.com/apps/cosmoviewer-ng/) - CosmoStreamers free, native windows applcation with direct Goggles connection and RTMP streaming support.
 - [DigiView-SBC](https://github.com/fpvout/DigiView-SBC) - an open source Raspberry PI image for your HDMI out needs
 - [fpv-dvca](https://github.com/d4rken/fpv-dvca) - an open source Android app by d4rken, including DVR and VR mode
 
# tl;dr;

Install **nodejs** and the Javascript dependencies

    npm install
    
Connect your DJI FPV goggles to your PC, power on your quad, make sure the stream is connected and run:

    node index.js -f outfile.bin
    
If all goes well voc-poc will find your DJI Goggles, obtain the USB interface and write the raw video stream to outfile.bin.

If you want to preview the stream live install ffplay (part of the **ffmpeg** package) and use the -o option.

    node index.js -o | ffplay -i - -analyzeduration 1 -probesize 32 -sync ext

If you want to produce a nice outfile.mp4, pipe the output through ffmpeg:

    node index.js -o | ffmpeg -vcodec copy outfile.mp4 -i -

If needed -o and -f may be combined.

See `node index.js --help` for more options.

## If you want to use C++ version

**only tested in ubuntu!!**

build the main.cpp with g++

    g++ main.cpp -o voc-poc -lusb-1.0

run the voc-poc

    ./voc-poc

the usage is same as nodejs version

    ./voc-poc -o | ffplay -i - -analyzeduration 1 -probesize 32 -sync ext

    ./voc-poc -o | ffmpeg -vcodec copy outfile.mp4 -i -

    ./voc-poc -f outfile.bin


## Troubleshooting
If you get USB errors on Windows make sure the Goggles Bulk Transfer Endpoint is using WinUSB drivers using [Zadiq](https://zadig.akeo.ie/).

## How does this work

It sends the magic `0x524d5654` packet to the USB Bulk Transfer Out Endpoint on the Goggles and starts dumping data from the USB Bulk Transfer Out Endpoint to file or stdout.

## The Protocol
The Goggles have a Bulk Transfer Endpoint. To initiate the video stream one has to simply send `0x524d5654` to the Out Bulk Endpoint and then Listen on the In Bulk Endpoint for data.

The data format is currently largely unknown:

 - `0x00010910` seems to indicate the beginning of some sort of h264 frame according to [this](https://github.com/district-michael/fpv_live/blob/4c7bb40e5cc5daec67b39cc093235afb959a4bfe/src/main/java/com/dji/video/framing/internal/parser/VideoFrameParser.java#L47).
 - Followed by a 6 byte (frame?) counter
 - Which is always followed by `0xc867ff`?

In any case, ffmpeg and other an make sense of it.

## Todo
### Essential

 - ~~Get data out over USB~~
 - ~~Figure out the packet structure~~
	 - It sort of just works
 - ~~Mux together an mp4 stream for output~~
	 - ffmpeg is good enough for the time being

### Nice to have

 - ~~Set the output file path via cli~~
 - More error handling
 - Wait for Goggles to appear if not connected
 - Reconnect (and re send magic packet) when connection lost
 - Support multiple Goggles
 - ~~Rewrite to C~~

## Why Nodejs?

Why not?

## Support the effort

If you'd like, you can send some ETH to `0xbAB1fec80922328F27De6E2F1CDBC2F322397637` or BTC to `3L7dE5EHtyd2b1tXBwdnWC2MADkV2VTbrq`  or [buy me a coffe](https://www.buymeacoffee.com/fpv.wtf).

## Credits

Shout out to Jack from [D3VL](https://d3vl.com/) for the debugging help, the og-s for [dji-firmware-tools](https://github.com/o-gs/dji-firmware-tools) and everyone else doing great work in the scene.
