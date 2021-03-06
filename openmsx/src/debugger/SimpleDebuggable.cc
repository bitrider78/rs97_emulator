// $Id: SimpleDebuggable.cc 10580 2009-09-14 20:37:31Z m9710797 $

#include "SimpleDebuggable.hh"
#include "MSXMotherBoard.hh"
#include "Debugger.hh"
#include "unreachable.hh"

namespace openmsx {

SimpleDebuggable::SimpleDebuggable(
		MSXMotherBoard& motherBoard_, const std::string& name_,
		const std::string& description_, unsigned size_)
	: motherBoard(motherBoard_)
	, name(name_)
	, description(description_)
	, size(size_)
{
	motherBoard.getDebugger().registerDebuggable(name, *this);
}

SimpleDebuggable::~SimpleDebuggable()
{
	motherBoard.getDebugger().unregisterDebuggable(name, *this);
}

unsigned SimpleDebuggable::getSize() const
{
	return size;
}

const std::string& SimpleDebuggable::getName() const
{
	return name;
}

const std::string& SimpleDebuggable::getDescription() const
{
	return description;
}

byte SimpleDebuggable::read(unsigned address)
{
	return read(address, motherBoard.getCurrentTime());
}

byte SimpleDebuggable::read(unsigned /*address*/, EmuTime::param /*time*/)
{
	UNREACHABLE; return 0;
}

void SimpleDebuggable::write(unsigned address, byte value)
{
	write(address, value, motherBoard.getCurrentTime());
}

void SimpleDebuggable::write(unsigned /*address*/, byte /*value*/,
                             EmuTime::param /*time*/)
{
	// does nothing
}

} // namespace openmsx
