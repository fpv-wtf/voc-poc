#include <libusb-1.0/libusb.h>
#include <fstream>
#include <iostream>
#include <csignal>
#include <string>
#include "cmdline.h"

#define VENDOR_ID 0x2ca3
#define PRODUCT_ID 0x1f
#define INTERFACE_NUMBER 3
#define IN_ENDPOINT_ADDRESS 0x84
#define OUT_ENDPOINT_ADDRESS 0x03
#define TIMEOUT_MS 100

libusb_context *ctx = nullptr;
libusb_device_handle *dev_handle = nullptr;

void signalHandler(int signal)
{
    libusb_release_interface(dev_handle, INTERFACE_NUMBER);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    exit(signal);
}

int main(int argc, char *argv[])
{
    cmdline::parser cmdParser;
    cmdParser.add<std::string>("file", 'f', "output video feed to file", false, "");
    cmdParser.add<bool>("o", 'stdout', "send video feed to stdout for playback. eg: voc-poc -o | ffplay -", false, false);
    cmdParser.add<int>("s", 'readsize', "size in bytes to queue for usb bulk interface reads", false, 512);
    cmdParser.add<bool>("v", 'verbose', "be noisy - doesn not play well with -o", false, false);
    cmdParser.parse(argc, argv);

    std::ofstream outputFile;
    if (cmdParser.get<bool>("file"))
    {
        outputFile.open(cmdParser.get<std::string>("file"));
        if (!outputFile)
        {
            std::cerr << "cannot create file" << std::endl;
            return;
        }
    }
    signal(SIGINT, signalHandler);

    int ret = libusb_init(&ctx);
    if (ret < 0)
    {
        std::cerr << "Failed to initialize libusb" << std::endl;
        return 1;
    }

    dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == nullptr)
    {
        std::cerr << "Couldn't open Goggles USB device" << std::endl;
        libusb_exit(ctx);
        return 1;
    }

    ret = libusb_claim_interface(dev_handle, INTERFACE_NUMBER);
    if (ret < 0)
    {
        std::cerr << "Couldn't claim bulk interface" << std::endl;
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 1;
    }

    unsigned char send_buffer[] = {0x52, 0x4d, 0x56, 0x54};
    int transferred = 0;
    ret = libusb_bulk_transfer(dev_handle, OUT_ENDPOINT_ADDRESS, send_buffer, sizeof(send_buffer), &transferred, TIMEOUT_MS);
    if (ret < 0)
    {
        std::cerr << "Failed to perform bulk transfer" << std::endl;
        libusb_release_interface(dev_handle, INTERFACE_NUMBER);
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 1;
    }

    std::cout << "Magic Sned: Sent " << transferred << " bytes of data. (should be 4)" << std::endl;

    unsigned char read_buffer[cmdParser.get<int>("readsize")];

    while (1)
    {
        transferred = 0;
        ret = libusb_bulk_transfer(dev_handle, IN_ENDPOINT_ADDRESS, read_buffer, sizeof(read_buffer), &transferred, TIMEOUT_MS);
        if (ret < 0)
        {
            std::cerr << "Failed to perform bulk transfer" << std::endl;
            libusb_release_interface(dev_handle, INTERFACE_NUMBER);
            libusb_close(dev_handle);
            libusb_exit(ctx);
            return 1;
        }
        if (cmdParser.get<bool>("verbose"))
            std::cout << "Received " << transferred << " bytes of data." << std::endl;
        if (cmdParser.get<bool>("stdout"))
            std::cout.write(reinterpret_cast<const char *>(read_buffer), transferred);
        if (cmdParser.get<bool>("file"))
        {
            if (!outputFile)
            {
                outputFile.open(cmdParser.get<std::string>("file"));
                if (!outputFile)
                {
                    std::cerr << "cannot create file" << std::endl;
                    return;
                }
            }
            outputFile.write(reinterpret_cast<const char *>(read_buffer), transferred);
        }
    }
    libusb_release_interface(dev_handle, INTERFACE_NUMBER);
    libusb_close(dev_handle);
    libusb_exit(ctx);
    return 0;
}
