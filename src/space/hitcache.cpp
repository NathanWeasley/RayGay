
#include "space/hitcache.h"

HitCache::HitCache(unsigned int size) {
    this->size = size;
    this->first = 0;
    entries = new Entry[size];
}

HitCache::~HitCache() {
    delete [] entries;
}

void HitCache::addEntry(void* fromObject, void* toObject, double t) {
    int i = findEntryIndex(fromObject);
    if (i == -1) {
	first = (first + 1 ) % size;
	entries[first].fromObject = fromObject;
	entries[first].toObject = toObject;
	entries[first].t = t;
    } else {
	entries[i].toObject = toObject;
	entries[i].t = t;
    }
}

void* HitCache::findEntry(void* fromObject) const {
    int i = findEntryIndex(fromObject);
    if (i == -1)
	return (void*)0;
    else 
	return entries[i].toObject;
}

int HitCache::findEntryIndex(void* fromObject) const {
    for(unsigned int i = 0; i < size; i++) {
	if (entries[i].fromObject == fromObject)
	    return i;
    }
    return -1;
}


