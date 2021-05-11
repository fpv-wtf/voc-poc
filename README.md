# ¯\\\_(ツ)_/¯
**voc-poc** is a proof of concept for video put from the DJI FPV Goggles over USB.

# tl;dr;

Install the dependencies

    npm install

Connect your DJI FPV goggles to your PC, power on your quad, make sure the stream is connected and run:

    node index.js
If all goes well voc-poc will find your DJI Goggles, obtain the USB interface, send the magic `0x524d5654` packet to the USB Bulk Transfer Out Endpoint and start dumping data from the USB Bulk Transfer Out Endpoint to **out.bin**.

## Troubleshooting
If you get USB errors on Windows make sure the Goggles Bulk Transfer Endpoint is using WinUSB drivers using [Zadiq](https://zadig.akeo.ie/).

## The Protocol
The Goggles have a Bulk Transfer Endpoint. To initiate the video stream one has to simply send `0x524d5654` to the Out Bulk Endpoint and then Listen on the In Bulk Endpoint for data.

The data format is currently largely unknown:

 - `0x00010910` seems to indicate the beginning of some sort of h264 frame according to [this](https://github.com/district-michael/fpv_live/blob/4c7bb40e5cc5daec67b39cc093235afb959a4bfe/src/main/java/com/dji/video/framing/internal/parser/VideoFrameParser.java#L47).
 - Followed by a 6 byte (frame?) counter
 - Which is always followed by `0xc867ff`?
 - ???
 - Profit

## Todo
### Essential

 - ~~Get data out over USB~~
 - Figure out the packet structure
 - Mux together an mp4 stream for output

### Nice to have

 - Set the output file path via cli
 - Graceful termination
 - Wait for Goggles to appear if not connected
 - Reconnect (and re send magic packet) when connection lost
 - Support multiple Goggles
 - Rewrite to C

## Why Nodejs?

Why not?

## Credits

Shout out to Jack from [D3VL](https://d3vl.com/) for the debugging help, the og-s for [dji-firmware-tools](https://github.com/o-gs/dji-firmware-tools) and everyone else doing great work in the scene.