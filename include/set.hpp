/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(SET_HPP)
#define SET_HPP

#include <sysdep.hpp>
#include <tracer.hpp>
#include <conditio.hpp>
#include <stdio.h>
#include <stdlib.h>

template<class T>
class EXP_CLASS Set;

template<class T>
class EXP_CLASS SetIterator;

/**
 * 
 */
template<class T>
class EXP_CLASS SetItem
{
public:
	SetItem(T* ItemPtr, SetItem<T>* NextItemPtr);
	~SetItem();

	operator	T*();
protected:
private:
	T*					iItemPtr;
	SetItem<T>*	iNextItemPtr;

	friend class Set<T>;
	friend class SetIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS Set
{
public:
	Set(BOOL shouldDelete = TRUE);
	~Set();

	T*			Add(T* ItemPtr);
	T*			AddBefore(T* ItemBefore, T* ItemPtr);
	T*			AddAfter(T* ItemAfter, T* ItemPtr);
	BOOL		Remove(T* ItemPtr);
	void		Clear();
	LONG	   Size() const;
	const T*	GetItem(int position) const;
	const T*	operator[](int position) const;
	T*		   operator[](int position);

	int operator==(const Set<T>& set);
	const Set<T>& operator=(const Set<T>& set);
	SetItem<T>* Contains(T* ItemPtr);
	SetItem<T>* GetHead();
	SetItem<T>* GetTail();

protected:
private:
	SetItem<T>*	iHead;
	SetItem<T>*	iTail;
	BOOL			iShouldDelete;

	friend class SetIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS SetIterator
{
public:
	SetIterator(Set<T>& Set);
	~SetIterator();
	T* GoHead();
	T* GoTail();
	T* GetPrev();
	T* GetNext();

protected:
private:
	Set<T>&		iSet;
	SetItem<T>*	iCurrentItem;
};

/**
 * 
 */
template<class T> SetItem<T>::SetItem(T* ItemPtr, SetItem<T>* NextItemPtr):
	iItemPtr(ItemPtr),
	iNextItemPtr(NextItemPtr)
{
	MTrace(("SetItem::SetItem()"));
}

/**
 * 
 */
template<class T> SetItem<T>::~SetItem()
{
	MTrace(("SetItem::~SetItem()"));
}

/**
 * 
 */
template<class T> SetItem<T>::operator T*()
{
	MTrace(("SetItem::operator T*()"));

	MFailIf(iItemPtr == 0);

	return iItemPtr;
}

/**
 * 
 */
template<class T> Set<T>::Set(BOOL shouldDelete):
	iShouldDelete(shouldDelete)
{
	MTrace(("Set::Set()"));
	iHead = iTail = 0;
}

/**
 * 
 */
template<class T> Set<T>::~Set()
{
	MTrace(("Set::~Set()"));
	Clear();
}

/**
 * 
 */
template<class T>
const Set<T>& Set<T>::operator=(const Set<T>& set)
{
	MTrace(("Set::operator=()"));
	// if the 'set' contains anything, then traverse the set
	// adding the items to the current set one at a time.
	if (set.iHead)
		{
		SetItem<T>*	current = set.iHead;

		while (current)
			{
			T*	newItem  = new T(*(T*)*current);
			Add(newItem);

			current = current->iNextItemPtr;
			}
		}

	return *this;
}

/**
 * 
 */
template<class T>
int Set<T>::operator==(const Set<T>& set)
{
	MTrace(("Set::operator==()"));

	if (!set.iHead && !iHead)
		{
		return true;
		}

	if (set.iHead && iHead)
		{
		const SetItem<T>*	current1 = iHead;
		const SetItem<T>*	current2 = set.iHead;

		while (current1 && current2)
			{
			if (*current1 != *current2)
				return false;

			current1 = current1->iNextItemPtr;
			current2 = current2->iNextItemPtr;
			}

		return !current1 && !current2;
		}

	return false;
}

/**
 * 
 */
template<class T> void Set<T>::Clear()
{
	MTrace(("Set::Clear()"));
	// if the set contains anything, then traverse the set
	// deleting the items one at a time.
	if (iHead)
		{
		SetItem<T>*	current = iHead;

		while (current)
			{
			SetItem<T>*	tmp = current;

			current = tmp->iNextItemPtr;

			MTrace(("Deleting item"));
			if (iShouldDelete)
				delete tmp->iItemPtr;

			delete tmp;
			}
		}

	iHead = iTail = 0;
}

/**
 * 
 */
template<class T> LONG Set<T>::Size() const
{
	LONG	count = 0;
	MTrace(("Set::Count()"));
	// if the set contains anything, then traverse the set
	// incrementing the count for each item found.
	if (iHead)
		{
		SetItem<T>*	current = iHead;

		while (current)
			{
			count++;
			current = current->iNextItemPtr;
			}
		}

	return count;
}

/**
 * 
 */
template<class T> const T* Set<T>::operator[](int position) const
{
	MTrace(("Set::operator[] const"));
	return this->GetItem(position);
}

/**
 * 
 */
template<class T> T* Set<T>::operator[](int position)
{
	MTrace(("Set::operator[]"));
	return (T*)this->GetItem(position);
}

/**
 * 
 */
template<class T> const T* Set<T>::GetItem(int position) const
{
	MTrace(("Set::GetItem()"));

	if (iHead)
		{
		SetItem<T>*	current = iHead;

		while (position > 0 && current)
			{
			SetItem<T>*	tmp = current;

			current = tmp->iNextItemPtr;
			position--;
			}

		if (current)
			return current->iItemPtr;
		}

	return 0;
}

/**
 * 
 */
template<class T> SetItem<T>* Set<T>::Contains(T* ItemPtr)
{
	if (iHead)
		{
		SetItem<T>* tmp = iHead;
		
		while (tmp)
			{
			if (tmp->iItemPtr == ItemPtr)
				return tmp;
				
			tmp = tmp->iNextItemPtr;
			}
		}
		
	return 0;
}

/**
 * 
 */
template<class T> T*	Set<T>::Add(T* ItemPtr)
{
	MTrace(("Set::Add()"));

	SetItem<T>* NewItem = new SetItem<T>(ItemPtr, 0);

	if (!NewItem)
		return 0;

	if (iHead)
		{
		// check to see if this item is already in the set
		SetItem<T>*	current = iHead;

		while (current)
			{
			if (*current->iItemPtr == *ItemPtr)
				return 0;

			current = current->iNextItemPtr;
			}

		iTail = iTail->iNextItemPtr = NewItem;
		}
	else
		{
		iHead = iTail = NewItem;
		}

	return ItemPtr;
}

/**
 * 
 */
template<class T>
T* Set<T>::AddBefore(T* ItemBefore, T* ItemPtr)
{
	MTrace(("Set::AddBefore()"));

	if (iHead)
		{
		// check to see if this item is already in the set
		SetItem<T>*	current = iHead;

		while (current)
			{
			if (*current->iItemPtr == *ItemPtr)
				return 0;

			current = current->iNextItemPtr;
			}

		if (iHead->iItemPtr == ItemBefore)
			{
			SetItem<T>* NewItem = new SetItem<T>(ItemPtr, iHead);

			if (NewItem)
				{
				iHead = NewItem;

				return ItemPtr;
				}
			}
		else
			{
			SetItem<T>* tmp = iHead;

			while (tmp)
				{
				if (tmp->iNextItemPtr && tmp->iNextItemPtr->iItemPtr == ItemBefore)
					break;

				tmp = tmp->iNextItemPtr;
				}

			if (tmp)
				{
				SetItem<T>* NewItem = new SetItem<T>(ItemPtr, tmp->iNextItemPtr);
				
				if (NewItem)
					{
					tmp->iNextItemPtr = NewItem;
					
					return ItemPtr;
					}
				}
			}
		}
		
	return 0;
}

/**
 * 
 */
template<class T>
T* Set<T>::AddAfter(T* ItemAfter, T* ItemPtr)
{
	MTrace(("Set::AddAfter()"));

	if (iHead)
		{
		// check to see if this item is already in the set
		SetItem<T>*	current = iHead;

		while (current)
			{
			if (*current->iItemPtr == *ItemPtr)
				return 0;

			current = current->iNextItemPtr;
			}

		SetItem<T>* tmp = iHead;

		while (tmp && tmp->iItemPtr != ItemAfter)
			tmp = tmp->iNextItemPtr;

		if (tmp)
			{
			SetItem<T>* NewItem = new SetItem<T>(ItemPtr, tmp->iNextItemPtr);

			if (NewItem)
				{
				tmp->iNextItemPtr = NewItem;

				// is we have just added to the tail of the set, then
				// the new item is now the new tail
				if (tmp == iTail)
					iTail = NewItem;

				return ItemPtr;
				}
			}
		}

	return 0;
}

/**
 * 
 */
template<class T> BOOL Set<T>::Remove(T* ItemPtr)
{
	SetItem<T>* tmp = iHead;
	SetItem<T>* prev = 0;
			
	while (tmp)
		{
		if (tmp->iItemPtr == ItemPtr)
			{
			if (tmp == iHead)
				{
				iHead = tmp->iNextItemPtr;
				
				if (iTail == tmp)
					iTail = iHead;
					
				delete tmp;
				}
			else
				{
				prev->iNextItemPtr = tmp->iNextItemPtr;
				
				if (tmp == iTail)
					iTail = prev;
					
				delete tmp;
				}
				
			return TRUE;
			}

		prev = tmp;
		tmp = tmp->iNextItemPtr;
		}
		
	return FALSE;
}

/**
 * 
 */
template<class T> SetItem<T>* Set<T>::GetHead()
{
	MTrace(("Set::GetHead()"));

	return iHead;
}

/**
 * 
 */
template<class T> SetItem<T>* Set<T>::GetTail()
{
	MTrace(("Set::GetTail()"));
	return iTail;
}

/**
 * 
 */
template<class T> SetIterator<T>::SetIterator(Set<T>& Set):
	iSet(Set),
	iCurrentItem(0)
{
}

/**
 * 
 */
template<class T> SetIterator<T>::~SetIterator()
{
}

/**
 * 
 */
template<class T> T* SetIterator<T>::GoHead()
{
	iCurrentItem = iSet.GetHead();

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

/**
 * 
 */
template<class T> T* SetIterator<T>::GoTail()
{
	iCurrentItem = iSet.GetTail();

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

/**
 * 
 */
template<class T> T*	SetIterator<T>::GetNext()
{
	if (iCurrentItem)
		iCurrentItem = iCurrentItem->iNextItemPtr;

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

/**
 * 
 */
template<class T> T*	SetIterator<T>::GetPrev()
{
	// can't go back past the head of the set
	if (iCurrentItem == iSet.GetHead())
		iCurrentItem = 0;
		
	if (iCurrentItem)
		{
		SetItem<T>*	tmp = iSet.GetHead();
		
		while (tmp && tmp->iNextItemPtr != iCurrentItem)
			tmp = tmp->iNextItemPtr;
			
		iCurrentItem = tmp;
		}

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

#endif
