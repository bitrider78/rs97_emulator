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
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/tags/release-1-2-1/engines/sci/engine/savegame.cpp $
 * $Id: savegame.cpp 52876 2010-09-24 10:16:16Z wjpalenstijn $
 *
 */

#include "common/stream.h"
#include "common/system.h"
#include "common/func.h"
#include "common/serializer.h"
#include "graphics/thumbnail.h"

#include "sci/sci.h"
#include "sci/event.h"

#include "sci/engine/features.h"
#include "sci/engine/kernel.h"
#include "sci/engine/state.h"
#include "sci/engine/message.h"
#include "sci/engine/savegame.h"
#include "sci/engine/selector.h"
#include "sci/engine/vm_types.h"
#include "sci/engine/script.h"	// for SCI_OBJ_EXPORTS and SCI_OBJ_SYNONYMS
#include "sci/graphics/palette.h"
#include "sci/graphics/ports.h"
#include "sci/sound/audio.h"
#include "sci/sound/music.h"

namespace Sci {


#define VER(x) Common::Serializer::Version(x)


#pragma mark -

// Experimental hack: Use syncWithSerializer to sync. By default, this assume
// the object to be synced is a subclass of Serializable and thus tries to invoke
// the saveLoadWithSerializer() method. But it is possible to specialize this
// template function to handle stuff that is not implementing that interface.
template<typename T>
void syncWithSerializer(Common::Serializer &s, T &obj) {
	obj.saveLoadWithSerializer(s);
}

// By default, sync using syncWithSerializer, which in turn can easily be overloaded.
template <typename T>
struct DefaultSyncer : Common::BinaryFunction<Common::Serializer, T, void> {
	void operator()(Common::Serializer &s, T &obj) const {
		//obj.saveLoadWithSerializer(s);
		syncWithSerializer(s, obj);
	}
};

/**
 * Sync a Common::Array using a Common::Serializer.
 * When saving, this writes the length of the array, then syncs (writes) all entries.
 * When loading, it loads the length of the array, then resizes it accordingly, before
 * syncing all entries.
 *
 * Note: This shouldn't be in common/array.h nor in common/serializer.h, after
 * all, not all code using arrays wants to use the serializer, and vice versa.
 * But we could put this into a separate header file in common/ at some point.
 * Something like common/serializer-extras.h or so.
 *
 * TODO: Add something like this for lists, queues....
 */
template <typename T, class Syncer = DefaultSyncer<T> >
struct ArraySyncer : Common::BinaryFunction<Common::Serializer, T, void> {
	void operator()(Common::Serializer &s, Common::Array<T> &arr) const {
		uint len = arr.size();
		s.syncAsUint32LE(len);
		Syncer sync;

		// Resize the array if loading.
		if (s.isLoading())
			arr.resize(len);

		typename Common::Array<T>::iterator i;
		for (i = arr.begin(); i != arr.end(); ++i) {
			sync(s, *i);
		}
	}
};

// Convenience wrapper
template<typename T>
void syncArray(Common::Serializer &s, Common::Array<T> &arr) {
	ArraySyncer<T> sync;
	sync(s, arr);
}


template <>
void syncWithSerializer(Common::Serializer &s, reg_t &obj) {
	s.syncAsUint16LE(obj.segment);
	s.syncAsUint16LE(obj.offset);
}

void SegManager::saveLoadWithSerializer(Common::Serializer &s) {
	if (s.isLoading())
		resetSegMan();

	s.skip(4, VER(14), VER(18));		// OBSOLETE: Used to be _exportsAreWide

	if (s.isLoading()) {
		// Reset _scriptSegMap, to be restored below
		_scriptSegMap.clear();
	}


	uint sync_heap_size = _heap.size();
	s.syncAsUint32LE(sync_heap_size);
	_heap.resize(sync_heap_size);
	for (uint i = 0; i < sync_heap_size; ++i) {
		SegmentObj *&mobj = _heap[i];

		// Sync the segment type
		SegmentType type = (s.isSaving() && mobj) ? mobj->getType() : SEG_TYPE_INVALID;
		s.syncAsUint32LE(type);

		// If we were saving and mobj == 0, or if we are loading and this is an
		// entry marked as empty -> skip to next
		if (type == SEG_TYPE_INVALID)
			continue;

		// Don't save or load HunkTable segments
		if (type == SEG_TYPE_HUNK)
			continue;

		if (s.isLoading())
			mobj = SegmentObj::createSegmentObj(type);

		assert(mobj);

		// Let the object sync custom data
		mobj->saveLoadWithSerializer(s);

		// If we are loading a script, hook it up in the script->segment map.
		if (s.isLoading() && type == SEG_TYPE_SCRIPT)
			_scriptSegMap[((Script *)mobj)->getScriptNumber()] = i;
	}

	s.syncAsSint32LE(_clonesSegId);
	s.syncAsSint32LE(_listsSegId);
	s.syncAsSint32LE(_nodesSegId);

	syncArray<Class>(s, _classTable);
}


template <>
void syncWithSerializer(Common::Serializer &s, Class &obj) {
	s.syncAsSint32LE(obj.script);
	syncWithSerializer(s, obj.reg);
}

static void sync_SavegameMetadata(Common::Serializer &s, SavegameMetadata &obj) {
	// TODO: It would be a good idea to store a magic number & a header size here,
	// so that we can implement backward compatibility if the savegame format changes.

	s.syncString(obj.savegame_name);
	s.syncVersion(CURRENT_SAVEGAME_VERSION);
	obj.savegame_version = s.getVersion();
	s.syncString(obj.game_version);
	s.syncAsSint32LE(obj.savegame_date);
	s.syncAsSint32LE(obj.savegame_time);
	if (s.getVersion() < 22) {
		obj.game_object_offset = 0;
		obj.script0_size = 0;
	} else {
		s.syncAsUint16LE(obj.game_object_offset);
		s.syncAsUint16LE(obj.script0_size);
	}
}

void EngineState::saveLoadWithSerializer(Common::Serializer &s) {
	Common::String tmp;
	s.syncString(tmp, VER(14), VER(23));			// OBSOLETE: Used to be game_version

	if (getSciVersion() <= SCI_VERSION_1_1) {
		// Save/Load picPort as well for SCI0-SCI1.1. Necessary for Castle of Dr. Brain,
		// as the picPort has been changed when loading during the intro
		int16 picPortTop, picPortLeft;
		Common::Rect picPortRect;

		if (s.isSaving())
			picPortRect = g_sci->_gfxPorts->kernelGetPicWindow(picPortTop, picPortLeft);

		s.syncAsSint16LE(picPortRect.top);
		s.syncAsSint16LE(picPortRect.left);
		s.syncAsSint16LE(picPortRect.bottom);
		s.syncAsSint16LE(picPortRect.right);
		s.syncAsSint16LE(picPortTop);
		s.syncAsSint16LE(picPortLeft);

		if (s.isLoading())
			g_sci->_gfxPorts->kernelSetPicWindow(picPortRect, picPortTop, picPortLeft, false);
	}

	_segMan->saveLoadWithSerializer(s);

	g_sci->_soundCmd->syncPlayList(s);
	g_sci->_gfxPalette->saveLoadWithSerializer(s);
}

void LocalVariables::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsSint32LE(script_id);
	syncArray<reg_t>(s, _locals);
}


void Object::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsSint32LE(_flags);
	syncWithSerializer(s, _pos);
	s.syncAsSint32LE(_methodCount);		// that's actually a uint16

	syncArray<reg_t>(s, _variables);
}

template <>
void syncWithSerializer(Common::Serializer &s, Table<Clone>::Entry &obj) {
	s.syncAsSint32LE(obj.next_free);

	syncWithSerializer<Object>(s, obj);
}

template <>
void syncWithSerializer(Common::Serializer &s, Table<List>::Entry &obj) {
	s.syncAsSint32LE(obj.next_free);

	syncWithSerializer(s, obj.first);
	syncWithSerializer(s, obj.last);
}

template <>
void syncWithSerializer(Common::Serializer &s, Table<Node>::Entry &obj) {
	s.syncAsSint32LE(obj.next_free);

	syncWithSerializer(s, obj.pred);
	syncWithSerializer(s, obj.succ);
	syncWithSerializer(s, obj.key);
	syncWithSerializer(s, obj.value);
}

#ifdef ENABLE_SCI32
template <>
void syncWithSerializer(Common::Serializer &s, Table<SciArray<reg_t> >::Entry &obj) {
	s.syncAsSint32LE(obj.next_free);

	byte type = 0;
	uint32 size = 0;
	
	if (s.isSaving()) {
		type = (byte)obj.getType();
		size = obj.getSize();
		s.syncAsByte(type);
		s.syncAsUint32LE(size);
	} else {
		s.syncAsByte(type);
		s.syncAsUint32LE(size);
		obj.setType((int8)type);

		// HACK: Skip arrays that have a negative type
		if ((int8)type < 0)
			return;

		obj.setSize(size);
	}

	for (uint32 i = 0; i < size; i++) {
		reg_t value;
		
		if (s.isSaving())
			value = obj.getValue(i);

		syncWithSerializer(s, value);

		if (s.isLoading())
			obj.setValue(i, value);
	}
}

template <>
void syncWithSerializer(Common::Serializer &s, Table<SciString>::Entry &obj) {
	s.syncAsSint32LE(obj.next_free);

	uint32 size = 0;
	
	if (s.isSaving()) {
		size = obj.getSize();
		s.syncAsUint32LE(size);
	} else {
		s.syncAsUint32LE(size);
		obj.setSize(size);
	}

	for (uint32 i = 0; i < size; i++) {
		char value = 0;
		
		if (s.isSaving())
			value = obj.getValue(i);

		s.syncAsByte(value);

		if (s.isLoading())
			obj.setValue(i, value);
	}
}
#endif

template <typename T>
void sync_Table(Common::Serializer &s, T &obj) {
	s.syncAsSint32LE(obj.first_free);
	s.syncAsSint32LE(obj.entries_used);

	syncArray<typename T::Entry>(s, obj._table);
}

void CloneTable::saveLoadWithSerializer(Common::Serializer &s) {
	sync_Table<CloneTable>(s, *this);
}

void NodeTable::saveLoadWithSerializer(Common::Serializer &s) {
	sync_Table<NodeTable>(s, *this);
}

void ListTable::saveLoadWithSerializer(Common::Serializer &s) {
	sync_Table<ListTable>(s, *this);
}

void HunkTable::saveLoadWithSerializer(Common::Serializer &s) {
	// Do nothing, hunk tables are not actually saved nor loaded.
}

void Script::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsSint32LE(_nr);

	if (s.isLoading())
		init(_nr, g_sci->getResMan());
	s.skip(4, VER(14), VER(22));		// OBSOLETE: Used to be _bufSize
	s.skip(4, VER(14), VER(22));		// OBSOLETE: Used to be _scriptSize
	s.skip(4, VER(14), VER(22));		// OBSOLETE: Used to be _heapSize

	s.skip(4, VER(14), VER(19));		// OBSOLETE: Used to be _numExports
	s.skip(4, VER(14), VER(19));		// OBSOLETE: Used to be _numSynonyms
	s.syncAsSint32LE(_lockers);

	// Sync _objects. This is a hashmap, and we use the following on disk format:
	// First we store the number of items in the hashmap, then we store each
	// object (which is an 'Object' instance). For loading, we take advantage
	// of the fact that the key of each Object obj is just obj._pos.offset !
	// By "chance" this format is identical to the format used to sync Common::Array<>,
	// hence we can still old savegames with identical code :).

	uint numObjs = _objects.size();
	s.syncAsUint32LE(numObjs);

	if (s.isLoading()) {
		_objects.clear();
		Object tmp;
		for (uint i = 0; i < numObjs; ++i) {
			syncWithSerializer(s, tmp);
			_objects[tmp.getPos().offset] = tmp;
		}
	} else {
		ObjMap::iterator it;
		const ObjMap::iterator end = _objects.end();
		for (it = _objects.begin(); it != end; ++it) {
			syncWithSerializer(s, it->_value);
		}
	}

	s.skip(4, VER(14), VER(20));		// OBSOLETE: Used to be _localsOffset
	s.syncAsSint32LE(_localsSegment);

	s.syncAsSint32LE(_markedAsDeleted);
}

static void sync_SystemString(Common::Serializer &s, SystemString &obj) {
	s.syncString(obj._name);
	s.syncAsSint32LE(obj._maxSize);

	// Sync obj._value. We cannot use syncCStr as we must make sure that
	// the allocated buffer has the correct size, i.e., obj._maxSize
	Common::String tmp;
	if (s.isSaving() && obj._value)
		tmp = obj._value;
	s.syncString(tmp);
	if (s.isLoading()) {
		if (!obj._maxSize) {
			obj._value = NULL;
		} else {
			//free(*str);
			obj._value = (char *)calloc(obj._maxSize, sizeof(char));
			strncpy(obj._value, tmp.c_str(), obj._maxSize);
		}
	}
}

void SystemStrings::saveLoadWithSerializer(Common::Serializer &s) {
	for (int i = 0; i < SYS_STRINGS_MAX; ++i)
		sync_SystemString(s, _strings[i]);
}

void DynMem::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsSint32LE(_size);
	s.syncString(_description);
	if (!_buf && _size) {
		_buf = (byte *)calloc(_size, 1);
	}
	if (_size)
		s.syncBytes(_buf, _size);
}

void DataStack::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsUint32LE(_capacity);
	if (s.isLoading()) {
		free(_entries);
		_entries = (reg_t *)calloc(_capacity, sizeof(reg_t));
	}
}

#pragma mark -

void SciMusic::saveLoadWithSerializer(Common::Serializer &s) {
	// Sync song lib data. When loading, the actual song lib will be initialized
	// afterwards in gamestate_restore()
	Common::StackLock lock(_mutex);

	int songcount = 0;
	byte masterVolume = soundGetMasterVolume();
	byte reverb = _pMidiDrv->getReverb();

	if (s.isSaving()) {
		s.syncAsByte(_soundOn);
		s.syncAsByte(masterVolume);
		s.syncAsByte(reverb, VER(17));
	} else if (s.isLoading()) {
		if (s.getVersion() >= 15) {
			s.syncAsByte(_soundOn);
			s.syncAsByte(masterVolume);
			reverb = 0;
			s.syncAsByte(reverb, VER(17));
		} else {
			_soundOn = true;
			masterVolume = 15;
			reverb = 0;
		}

		soundSetSoundOn(_soundOn);
		soundSetMasterVolume(masterVolume);
		setReverb(reverb);
	}

	if (s.isSaving())
		songcount = _playList.size();
	s.syncAsUint32LE(songcount);

	if (s.isLoading()) {
		clearPlayList();

		for (int i = 0; i < songcount; i++) {
			MusicEntry *curSong = new MusicEntry();
			curSong->saveLoadWithSerializer(s);
			_playList.push_back(curSong);
		}
	} else {
		for (int i = 0; i < songcount; i++) {
			_playList[i]->saveLoadWithSerializer(s);
		}
	}
}

void MusicEntry::saveLoadWithSerializer(Common::Serializer &s) {
	syncWithSerializer(s, soundObj);
	s.syncAsSint16LE(resourceId);
	s.syncAsSint16LE(dataInc);
	s.syncAsSint16LE(ticker);
	s.syncAsSint16LE(signal, VER(17));
	s.syncAsByte(priority);
	s.syncAsSint16LE(loop, VER(17));
	s.syncAsByte(volume);
	s.syncAsByte(hold, VER(17));
	s.syncAsByte(fadeTo);
	s.syncAsSint16LE(fadeStep);
	s.syncAsSint32LE(fadeTicker);
	s.syncAsSint32LE(fadeTickerStep);
	s.syncAsByte(status);

	// pMidiParser and pStreamAud will be initialized when the
	// sound list is reconstructed in gamestate_restore()
	if (s.isLoading()) {
		soundRes = 0;
		pMidiParser = 0;
		pStreamAud = 0;
	}
}

void SoundCommandParser::syncPlayList(Common::Serializer &s) {
	_music->saveLoadWithSerializer(s);
}

void SoundCommandParser::reconstructPlayList(int savegame_version) {
	Common::StackLock lock(_music->_mutex);

	const MusicList::iterator end = _music->getPlayListEnd();
	for (MusicList::iterator i = _music->getPlayListStart(); i != end; ++i) {
		if ((*i)->resourceId && _resMan->testResource(ResourceId(kResourceTypeSound, (*i)->resourceId))) {
			(*i)->soundRes = new SoundResource((*i)->resourceId, _resMan, _soundVersion);
			_music->soundInitSnd(*i);
		} else {
			(*i)->soundRes = 0;
		}
		if ((*i)->status == kSoundPlaying) {
			processPlaySound((*i)->soundObj);
		}
	}
}

#ifdef ENABLE_SCI32
void ArrayTable::saveLoadWithSerializer(Common::Serializer &ser) {
	if (ser.getVersion() < 18)
		return;

	sync_Table<ArrayTable>(ser, *this);
}

void StringTable::saveLoadWithSerializer(Common::Serializer &ser) {
	if (ser.getVersion() < 18)
		return;

	sync_Table<StringTable>(ser, *this);
}
#endif

void GfxPalette::palVarySaveLoadPalette(Common::Serializer &s, Palette *palette) {
	s.syncBytes(palette->mapping, 256);
	s.syncAsUint32LE(palette->timestamp);
	for (int i = 0; i < 256; i++) {
		s.syncAsByte(palette->colors[i].used);
		s.syncAsByte(palette->colors[i].r);
		s.syncAsByte(palette->colors[i].g);
		s.syncAsByte(palette->colors[i].b);
	}
	s.syncBytes(palette->intensity, 256);
}

void GfxPalette::saveLoadWithSerializer(Common::Serializer &s) {
	if (s.getVersion() >= 25) {
		// We need to save intensity of the _sysPalette at least for kq6 when entering the dark cave (room 390)
		//  from room 340. scripts will set intensity to 60 for this room and restore them when leaving.
		//  Sierra SCI is also doing this (although obviously not for SCI0->SCI01 games, still it doesn't hurt
		//  to save it everywhere). ffs. bug #3072868
		s.syncBytes(_sysPalette.intensity, 256);
	}
	if (s.getVersion() >= 24) {
		if (s.isLoading() && _palVaryResourceId != -1)
			palVaryRemoveTimer();

		s.syncAsSint32LE(_palVaryResourceId);
		if (_palVaryResourceId != -1) {
			palVarySaveLoadPalette(s, &_palVaryOriginPalette);
			palVarySaveLoadPalette(s, &_palVaryTargetPalette);
			s.syncAsSint16LE(_palVaryStep);
			s.syncAsSint16LE(_palVaryStepStop);
			s.syncAsSint16LE(_palVaryDirection);
			s.syncAsUint16LE(_palVaryTicks);
			s.syncAsSint32LE(_palVaryPaused);
		}

		if (s.isLoading() && _palVaryResourceId != -1) {
			_palVarySignal = 0;
			palVaryInstallTimer();
		}
	}
}

void SegManager::reconstructStack(EngineState *s) {
	DataStack *stack = (DataStack *)(_heap[findSegmentByType(SEG_TYPE_STACK)]);
	s->stack_base = stack->_entries;
	s->stack_top = s->stack_base + stack->_capacity;
}

// TODO: Move this function to a more appropriate place, such as vm.cpp or script.cpp
void SegManager::reconstructScripts(EngineState *s) {
	uint i;

	for (i = 0; i < _heap.size(); i++) {
		if (!_heap[i] ||  _heap[i]->getType() != SEG_TYPE_SCRIPT)
			continue;

		Script *scr = (Script *)_heap[i];
		scr->load(g_sci->getResMan());
		scr->_localsBlock = (scr->_localsSegment == 0) ? NULL : (LocalVariables *)(_heap[scr->_localsSegment]);

		for (ObjMap::iterator it = scr->_objects.begin(); it != scr->_objects.end(); ++it)
			it->_value._baseObj = scr->getBuf(it->_value.getPos().offset);
	}

	for (i = 0; i < _heap.size(); i++) {
		if (!_heap[i] ||  _heap[i]->getType() != SEG_TYPE_SCRIPT)
			continue;

		Script *scr = (Script *)_heap[i];

		for (ObjMap::iterator it = scr->_objects.begin(); it != scr->_objects.end(); ++it) {
			reg_t addr = it->_value.getPos();
			Object *obj = scr->scriptObjInit(addr, false);

			if (getSciVersion() < SCI_VERSION_1_1) {
				if (!obj->initBaseObject(this, addr, false)) {
					// TODO/FIXME: This should not be happening at all. It might indicate a possible issue
					// with the garbage collector. It happens for example in LSL5 (German, perhaps English too).
					warning("Failed to locate base object for object at %04X:%04X; skipping", PRINT_REG(addr));
					scr->scriptObjRemove(addr);
				}
			}
		}
	}
}

void SegManager::reconstructClones() {
	for (uint i = 0; i < _heap.size(); i++) {
		SegmentObj *mobj = _heap[i];
		if (mobj && mobj->getType() == SEG_TYPE_CLONES) {
			CloneTable *ct = (CloneTable *)mobj;

			for (uint j = 0; j < ct->_table.size(); j++) {
				// Check if the clone entry is used
				uint entryNum = (uint)ct->first_free;
				bool isUsed = true;
				while (entryNum != ((uint) CloneTable::HEAPENTRY_INVALID)) {
					if (entryNum == j) {
						isUsed = false;
						break;
					}
					entryNum = ct->_table[entryNum].next_free;
				}

				if (!isUsed)
					continue;

				CloneTable::Entry &seeker = ct->_table[j];
				const Object *baseObj = getObject(seeker.getSpeciesSelector());
				seeker.cloneFromObject(baseObj);
				if (!baseObj) {
					// Can happen when loading some KQ6 savegames
					warning("Clone entry without a base class: %d", j);
				}
			}	// end for
		}	// end if
	}	// end for
}


#pragma mark -


bool gamestate_save(EngineState *s, Common::WriteStream *fh, const char* savename, const char *version) {
	TimeDate curTime;
	g_system->getTimeAndDate(curTime);

	SavegameMetadata meta;
	meta.savegame_version = CURRENT_SAVEGAME_VERSION;
	meta.savegame_name = savename;
	meta.game_version = version;
	meta.savegame_date = ((curTime.tm_mday & 0xFF) << 24) | (((curTime.tm_mon + 1) & 0xFF) << 16) | ((curTime.tm_year + 1900) & 0xFFFF);
	meta.savegame_time = ((curTime.tm_hour & 0xFF) << 16) | (((curTime.tm_min) & 0xFF) << 8) | ((curTime.tm_sec) & 0xFF);

	Resource *script0 = g_sci->getResMan()->findResource(ResourceId(kResourceTypeScript, 0), false);
	meta.script0_size = script0->size;
	meta.game_object_offset = g_sci->getGameObject().offset;

	// Checking here again
	if (s->executionStackBase) {
		warning("Cannot save from below kernel function");
		return false;
	}

	Common::Serializer ser(0, fh);
	sync_SavegameMetadata(ser, meta);
	Graphics::saveThumbnail(*fh);
	s->saveLoadWithSerializer(ser);		// FIXME: Error handling?

	return true;
}

extern void showScummVMDialog(const Common::String &message);

void gamestate_restore(EngineState *s, Common::SeekableReadStream *fh) {
	SavegameMetadata meta;

	Common::Serializer ser(fh, 0);
	sync_SavegameMetadata(ser, meta);

	if (fh->eos()) {
		s->r_acc = TRUE_REG;	// signal failure
		return;
	}

	if ((meta.savegame_version < MINIMUM_SAVEGAME_VERSION) ||
	    (meta.savegame_version > CURRENT_SAVEGAME_VERSION)) {
		/*
		if (meta.savegame_version < MINIMUM_SAVEGAME_VERSION)
			warning("Old savegame version detected, unable to load it");
		else
			warning("Savegame version is %d, maximum supported is %0d", meta.savegame_version, CURRENT_SAVEGAME_VERSION);
		*/

		showScummVMDialog("The format of this saved game is obsolete, unable to load it");

		s->r_acc = TRUE_REG;	// signal failure
		return;
	}

	if (meta.game_object_offset > 0 && meta.script0_size > 0) {
		Resource *script0 = g_sci->getResMan()->findResource(ResourceId(kResourceTypeScript, 0), false);
		if (script0->size != meta.script0_size || g_sci->getGameObject().offset != meta.game_object_offset) {
			//warning("This saved game was created with a different version of the game, unable to load it");

			showScummVMDialog("This saved game was created with a different version of the game, unable to load it");

			s->r_acc = TRUE_REG;	// signal failure
			return;
		}
	}

	// We don't need the thumbnail here, so just read it and discard it
	Graphics::skipThumbnail(*fh);

	s->reset(true);
	s->saveLoadWithSerializer(ser);	// FIXME: Error handling?

	// Now copy all current state information

	s->_segMan->reconstructStack(s);
	s->_segMan->reconstructScripts(s);
	s->_segMan->reconstructClones();
	s->initGlobals();
	s->gcCountDown = GC_INTERVAL - 1;

	// Time state:
	s->lastWaitTime = g_system->getMillis();
	s->gameStartTime = g_system->getMillis();
	s->_screenUpdateTime = g_system->getMillis();

	if (g_sci->_gfxPorts)
		g_sci->_gfxPorts->reset();

	g_sci->_soundCmd->reconstructPlayList(meta.savegame_version);

	// Message state:
	delete s->_msgState;
	s->_msgState = new MessageState(s->_segMan);

	s->abortScriptProcessing = kAbortLoadGame;

	// signal restored game to game scripts
	s->gameIsRestarting = GAMEISRESTARTING_RESTORE;
}

bool get_savegame_metadata(Common::SeekableReadStream *stream, SavegameMetadata *meta) {
	assert(stream);
	assert(meta);

	Common::Serializer ser(stream, 0);
	sync_SavegameMetadata(ser, *meta);

	if (stream->eos())
		return false;

	if ((meta->savegame_version < MINIMUM_SAVEGAME_VERSION) ||
	    (meta->savegame_version > CURRENT_SAVEGAME_VERSION)) {
		if (meta->savegame_version < MINIMUM_SAVEGAME_VERSION)
			warning("Old savegame version detected- can't load");
		else
			warning("Savegame version is %d- maximum supported is %0d", meta->savegame_version, CURRENT_SAVEGAME_VERSION);

		return false;
	}

	return true;
}

} // End of namespace Sci
