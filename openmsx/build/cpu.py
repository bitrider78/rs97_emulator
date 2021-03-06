# $Id: cpu.py 10883 2009-12-05 18:54:27Z mthuurne $

class CPU(object):
	'''Abstract base class for CPU families.
	'''

	# String that we use to identify this CPU type.
	name = None

	# Big (True) or little (False) endian?
	# Note that some CPUs can do both; if both settings are used in practice
	# we treat them like different CPUs (see MIPS/MIPSel).
	bigEndian = None

	# Allow unaligned memory accesses?
	unalignedMemoryAccess = False

	# GCC flags to pass to the compile and link commands.
	gccFlags = ()

class Alpha(CPU):
	'''DEC Alpha.
	'''
	name = 'alpha'
	bigEndian = False

class ARM(CPU):
	'''ARM.
	'''
	name = 'arm'
	bigEndian = False

class AVR32(CPU):
	'''Atmel AVR32, an embedded RISC CPU.
	'''
	name = 'avr32'
	bigEndian = True

class HPPA(CPU):
	'''HP PA-RISC.
	'''
	name = 'hppa'
	bigEndian = True

class IA64(CPU):
	'''Intel Itanium.
	'''
	name = 'ia64'
	bigEndian = False

class M68k(CPU):
	'''Motorola 680x0.
	'''
	name = 'm68k'
	bigEndian = True

class MIPS(CPU):
	'''Big endian MIPS.
	'''
	name = 'mips'
	bigEndian = True

class MIPSel(MIPS):
	'''Little endian MIPS.
	'''
	name = 'mipsel'
	bigEndian = False

class PPC(CPU):
	'''32-bit Power PC.
	'''
	name = 'ppc'
	bigEndian = True

class PPC64(CPU):
	'''64-bit Power PC.
	'''
	name = 'ppc64'
	bigEndian = True

class S390(CPU):
	'''IBM S/390.
	'''
	name = 's390'
	bigEndian = True

class SH(CPU):
	'''Little endian Renesas SuperH.
	'''
	name = 'sh'
	bigEndian = False

class SHeb(CPU):
	'''Big endian Renesas SuperH.
	'''
	name = 'sheb'
	bigEndian = True

class Sparc(CPU):
	'''Sun Sparc.
	'''
	name = 'sparc'
	bigEndian = True

class X86(CPU):
	'''32-bit x86: Intel Pentium, AMD Athlon etc.
	'''
	name = 'x86'
	bigEndian = False
	unalignedMemoryAccess = True
	gccFlags = '-m32',

class X86_64(CPU):
	'''64-bit x86. Also known as AMD64 or x64.
	'''
	name = 'x86_64'
	bigEndian = False
	unalignedMemoryAccess = True
	gccFlags = '-m64',

# Build a dictionary of CPUs using introspection.
def _discoverCPUs(localObjects):
	for obj in localObjects:
		if isinstance(obj, type) and issubclass(obj, CPU):
			if not (obj is CPU):
				yield obj.name, obj
_cpusByName = dict(_discoverCPUs(locals().itervalues()))

def getCPU(name):
	return _cpusByName[name]
