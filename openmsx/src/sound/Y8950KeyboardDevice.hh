// $Id: Y8950KeyboardDevice.hh 12629 2012-06-14 20:16:30Z m9710797 $

#ifndef Y8950KEYBOARDDEVICE_HH
#define Y8950KEYBOARDDEVICE_HH

#include "Pluggable.hh"
#include "openmsx.hh"

namespace openmsx {

class Y8950KeyboardDevice : public Pluggable
{
public:
	/**
	 * Send data to the device.
	 * Normally this is used to select a certain row from the
	 * keyboard but you might also connect a non-keyboard device.
	 * A 1-bit means corresponding row is selected ( 0V)
	 *   0-bit                        not selected (+5V)
	 */
	virtual void write(byte data, EmuTime::param time) = 0;

	/**
	 * Read data from the device.
	 * Normally this are the keys that are pressed but you might
	 * also connect a non-keyboard device.
	 * A 0-bit means corresponding key is pressed
	 *   1-bit                        not pressed
	 */
	virtual byte read(EmuTime::param time) = 0;

	// pluggable
	virtual string_ref getClass() const;
};

} // namespace openmsx

#endif
