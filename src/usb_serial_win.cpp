/**************************************************************************
 *  Copyright 2007-2012 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of KISS (Kipr's Instructional Software System).     *
 *                                                                        *
 *  KISS is free software: you can redistribute it and/or modify          *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  KISS is distributed in the hope that it will be useful,               *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with KISS.  Check the LICENSE file in the project root.         *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#include "kovanserial/usb_serial_win.hpp"

#include <unistd.h>
#include <string.h>
#include <iostream>

UsbSerialWin::UsbSerialWin(const char *dev)
	: m_handle(INVALID_HANDLE_VALUE)
{
	strncpy(m_dev, dev, 128);
}

UsbSerialWin::~UsbSerialWin()
{
	endSession();
}

bool UsbSerialWin::makeAvailable()
{
	if(m_handle != INVALID_HANDLE_VALUE) endSession();
	COMMTIMEOUTS cto;
	char realPortname[10];
	memset(realPortname, 0, 10);
	strncpy(realPortname, "\\\\.\\", 4);
	strncpy(realPortname + 4, m_dev, 5);

	m_handle = CreateFileA(realPortname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	if(m_handle == INVALID_HANDLE_VALUE) return false;

	cto.ReadIntervalTimeout = MAXDWORD;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = 0;
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = 0;

	if(!SetCommTimeouts(m_handle, &cto)) {
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
		return false;
	}

	if(!SetupComm(m_handle, 2048, 2048)) {
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
		return false;
	}

	if(!SetCommMask(m_handle, 0)) {
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
		return false;
	}
	configure();
	return true;
}

bool UsbSerialWin::available() const
{
	return m_handle != INVALID_HANDLE_VALUE;
}

bool UsbSerialWin::isReliable() const
{
	return false;
}

void UsbSerialWin::endSession()
{
	if(m_handle == INVALID_HANDLE_VALUE) return;

	CloseHandle(m_handle);
	m_handle = INVALID_HANDLE_VALUE;
}

void UsbSerialWin::configure()
{
	DCB dcb;

	memset(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);

	if(!GetCommState(m_handle, &dcb)) return;

	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = 115200;
	dcb.fBinary = 1;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fTXContinueOnXoff = 0;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fNull = 0;
	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
	dcb.fAbortOnError = FALSE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.fParity = 0;
	dcb.StopBits = ONESTOPBIT;

	SetCommState(m_handle, &dcb);
}

ssize_t UsbSerialWin::readBlock(uint8_t *data, const size_t len, const uint32_t timeout)
{
	if(!available()) return -1;
	size_t pos = 0;
	long startTime = msystime();
	do {
		long endTime = msystime();
		if(timeout > 0 && endTime - startTime > timeout) return pos;

		ssize_t ret = read(data + pos, len - pos);
		if(ret < 0 && errno != EAGAIN) return ret;
		if(ret > 0) {
			pos += ret;
			startTime = endTime;
		}
		yield();
	} while(pos < len);

	return pos;
}

ssize_t UsbSerialWin::read(uint8_t *data, const size_t size)
{
	if(m_handle == INVALID_HANDLE_VALUE) return -1;
	DWORD read_count = 0;
	if(!ReadFile(m_handle, reinterpret_cast<char *>(data), size,
		&read_count, NULL)) {
		std::cerr << "ReadFile failed" << std::endl;
		return 0;
	}
	if(read_count) std::cerr << "Readfile returned " << read_count << std::endl;
	return read_count;
	
}

ssize_t UsbSerialWin::write(const uint8_t *data, const size_t size)
{
	if(m_handle == INVALID_HANDLE_VALUE) return -1;
	DWORD write_count;
	return !WriteFile(m_handle, reinterpret_cast<const char *>(data),
		size, &write_count, NULL) ? 0 : write_count;
}
