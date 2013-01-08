#ifndef _USB_SERIAL_HPP_
#define _USB_SERIAL_HPP_

#ifdef WIN32
#include "usb_serial_win.hpp"
typedef UsbSerialWin UsbSerial;
#else
#include "usb_serial_unix.hpp"
typedef UsbSerialUnix UsbSerial;
#endif

#endif
