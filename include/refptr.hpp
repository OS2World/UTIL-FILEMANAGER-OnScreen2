/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(__REFPTR_HPP)
#define __REFPTR_HPP

#include <sysdep.hpp>
#include <conditio.hpp>
#include <tracer.hpp>

/**
 * 
 */
template <class T>
class EXP_CLASS Storage {
	public:
		Storage(T* pt);
		Storage<T>* operator=(const Storage<T>* lvps);
		~Storage();
		UINT AddRef();
		UINT ReleaseRef();
		T* GetPointer();
      UINT GetRefCount();

	private:
		UINT	refCount;
		T*		pt;
};

/**
 * 
 */
template <class T>
class EXP_CLASS RefPtr
{
public:
	// create a refptr object and by default create the object to which it refers
	RefPtr(BOOL bCreate = TRUE):ps(0) {
		if (bCreate) {
			ps = new Storage<T>(new T);
		}
	}

	// create a refptr from another refptr
	RefPtr(const RefPtr<T>& rp) {
			ps = rp.ps;
		}

	// create a refptr from a pointer
	RefPtr(T* pt):ps(new Storage<T>(pt)) {
	}

	// destroy a refptr
	~RefPtr() {
		ps->ReleaseRef();
	}

	const RefPtr<T>& operator=(const RefPtr<T>& rp) {
			ps = rp.ps;

			return *this;
	}

	T* operator->() {
		return ps->GetPointer() ? ps->GetPointer() : 0;
	}

	const T* operator->() const {
		return ps->GetPointer() ? ps->GetPointer() : 0;
	}

	T& operator*() {
		MFailIf(ps->GetPointer() == 0);
		return *ps->GetPointer();
	}

	const T& operator*() const {
		MFailIf(ps->GetPointer() == 0);
		return *ps->GetPointer();
	}

	operator T& () {
		MFailIf(ps->GetPointer() == 0);
		return *ps->GetPointer();
	}

	operator T* () {
		MFailIf(ps->GetPointer() == 0);
		return ps->GetPointer();
	}

	UINT	getRefCount() {
		return ps->GetRefCount();
	}

private:
	Storage<T>*	ps;
};


template<class T> Storage<T>::Storage(T* pt):pt(pt), refCount(1) {
			}

template<class T> Storage<T>::~Storage() {
				if (--refCount == 0)
					if (pt) {
						if (pt)
							delete pt;

						pt = 0;
						}
			}

template<class T> Storage<T>* Storage<T>::operator=(const Storage<T>* lvps) {
			ReleaseRef();
			pt = lvps->GetPointer();
			AddRef();

			return this;
		}

template<class T> UINT Storage<T>::AddRef() {
			return ++refCount;
		}

template<class T> UINT Storage<T>::ReleaseRef() {
			if (--refCount == 0) {
				if (pt)
					delete pt;

				pt = 0;	// help with those GPF/Segmentation Violations
				}

			return refCount;
		}

template<class T>
T* Storage<T>::GetPointer()
{
	return pt;
}

template<class T>
UINT Storage<T>::GetRefCount()
{
	return refCount;
}


#endif
