#ifndef USB_H_
#define USB_H_
#include <cstdlib>
#include <string>
#include <iostream>
#include "libusb-1.0/libusb.h"

namespace Toolset
{
    void FlasherView();
}

class Device {
public:
    Device(const char *i, const char *c);
    void display();
    const char *device_name;
    const char *manufacturer;
    const char *chipset;
};

/**
 * A class that passes around usb stats. Primarily we would like to pass through the view,
 * which is the default approach but we only want the view to grab things it needs and destroys
 * itself if need be.
*/
// class USBStats
// {
//     private:
//     USBStats::~USBStats();
//     public:
//     explicit USBStats();
// };

// namespace USB_UI
// {

    // class USBDevice;
    // class USBEditor {
        
    // private:
    //     USBEditor::~USBEditor();
    // public:

    //     // void FlasherView();
    //     int test_num;

    // };
    /**
     *
    */
    // class USBDevice {

    // public:
    //     USBDevice(USBEditor &editor_, libusb_device_handle *dev_handle, int id);
    //     USBEditor &editor_() {return _editor_;}
    // protected:
    //     USBEditor &_editor_;
    //     libusb_context *ctx;

    // };
    // class USBView {
        
    // public:
    //     struct USBView *usb_view_m;
    // protected:
    //     USBEditor *usbdev;
    // };

    // class USBEditor{
    // public:
    //     friend USBDevice;
    //     USBEditor();
    //     void test_run();
    // protected:
    //     libusb_device **devs;
    //     libusb_context *ctx;
    // };
// }

#endif//USB_H_
