/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/tags/release-1-2-1/engines/gob/helper.h $
 * $Id: helper.h 41299 2009-06-06 20:03:13Z fingolfin $
 *
 */

#ifndef GOB_HELPER_H
#define GOB_HELPER_H

namespace Gob {

/** A strncpy that forces the final \0. */
inline char *strncpy0(char *dest, const char *src, size_t n) {
	strncpy(dest, src, n);
	dest[n] = 0;
	return dest;
}

} // End of namespace Gob

#endif // GOB_HELPER_H
