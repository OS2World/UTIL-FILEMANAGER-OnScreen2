/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(__DLIST_HPP)
#define __DLIST_HPP

#include <sysdep.hpp>
#include <tracer.hpp>
#include <stdio.h>
#include <stdlib.h>

//#CLASS#-----------------------------oOo-----------------------------------//
// template<class T> class DListItem
//#END#-------------------------------oOo-----------------------------------//

template<class T>
class EXP_CLASS DList;

template<class T>
class EXP_CLASS DListIterator;

template<class T>
class EXP_CLASS DListItem
{
public:
	DListItem(T* ItemPtr, DListItem<T>* PrevItemPtr, DListItem<T>* NextItemPtr);
	~DListItem();

protected:
private:
	T*					iItemPtr;
	DListItem<T>*	iPrevItemPtr;
	DListItem<T>*	iNextItemPtr;

	friend class DList<T>;
	friend class DListIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS DList
{
public:
	DList();
	~DList();
	
	T*		Add(T* ItemPtr);
	T*		AddBefore(T* ItemBefore, T* ItemPtr);
	T*		AddAfter(T* ItemAfter, T* ItemPtr);
	BOOL	Remove(T* ItemPtr);
	
	DListItem<T>* Contains(T* ItemPtr);
	DListItem<T>* GetHead();
	DListItem<T>* GetTail();

protected:
private:
	DListItem<T>*	iHead;
	DListItem<T>*	iTail;

	friend class DListIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS DListIterator
{
public:
	DListIterator(DList<T>& List);
	~DListIterator();
	T* GoHead();
	T* GoTail();
	T*	GetNext();
	T*	GetPrev();

protected:
private:
	DList<T>&		iList;
	DListItem<T>*	iCurrentItem;
};

/**
 * 
 */
template<class T> DListItem<T>::DListItem(T* ItemPtr,
														DListItem<T>* PrevItemPtr,
														DListItem<T>* NextItemPtr):
	iItemPtr(ItemPtr),
	iPrevItemPtr(PrevItemPtr),
	iNextItemPtr(NextItemPtr)
{
	Trace(("DListItem::DListItem()"));
}
 
/**
 * 
 */
template<class T> DListItem<T>::~DListItem()
{
	Trace(("DListItem::~DListItem()"));
	// delete the object that the list contains
	delete iItemPtr;
}

/**
 * 
 */
template<class T> DList<T>::DList()
{
	Trace(("DList::DList()"));
	iHead = iTail = 0;
}

/**
 * 
 */
template<class T> DList<T>::~DList()
{
	Trace(("DList::~DList()"));
	// if the list contains anything, then traverse the list
	// deleting the items one at a time.
	if (iHead)
		{
		DListItem<T>*	current = iHead;

		while (current)
			{
			DListItem<T>*	tmp = current;

			current = tmp->iNextItemPtr;

			Trace(("Deleting item"));
			delete tmp;
			}
		}
}

/**
 * 
 */
template<class T> DListItem<T>* DList<T>::Contains(T* ItemPtr)
{
	if (iHead)
		{
		DListItem<T>* tmp = iHead;
		
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
template<class T> T*	DList<T>::Add(T* ItemPtr)
{
	Trace(("DList::Add()"));

	DListItem<T>* NewItem = new DListItem<T>(ItemPtr, 0, 0);

	if (!NewItem)
		return 0;

	if (iHead)
		{
		NewItem->iPrevItemPtr = iTail;
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
template<class T> T*	DList<T>::AddBefore(T* ItemBefore, T* ItemPtr)
{
	Trace(("DList::AddBefore()"));

	if (iHead)
		{
		if (iHead->iItemPtr == ItemBefore)
			{
			DListItem<T>* NewItem = new DListItem<T>(ItemPtr, 0, iHead);
			
			if (NewItem)
				{
				iHead->iPrevItemPtr = NewItem;
				iHead = NewItem;
				
				return ItemPtr;
				}
			}
		else
			{
			DListItem<T>* tmp = iHead;
			
			while (tmp)
				{
				if (tmp->iNextItemPtr && tmp->iNextItemPtr->iItemPtr == ItemBefore)
					break;
					
				tmp = tmp->iNextItemPtr;
				}
				
			if (tmp)
				{
				DListItem<T>* NewItem = new DListItem<T>(ItemPtr, tmp, tmp->iNextItemPtr);
				
				if (NewItem)
					{
					tmp->iNextItemPtr->iPrevItemPtr = NewItem;
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
template<class T> T*	DList<T>::AddAfter(T* ItemAfter, T* ItemPtr)
{
	Trace(("DList::AddAfter()"));

	if (iHead)
		{
		DListItem<T>* tmp = iHead;
		
		while (tmp && tmp->iItemPtr != ItemAfter)
			tmp = tmp->iNextItemPtr;
			
		if (tmp)
			{
			DListItem<T>* NewItem = new DListItem<T>(ItemPtr, tmp, tmp->iNextItemPtr);

			if (NewItem)
				{				
				tmp->iNextItemPtr->iPrevItemPtr = NewItem;
				tmp->iNextItemPtr = NewItem;
			
				// is we have just added to the tail of the list, then
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
template<class T> BOOL DList<T>::Remove(T* ItemPtr)
{
	DListItem<T>* tmp = iHead;
			
	while (tmp)
		{
		if (tmp->iItemPtr == ItemPtr)
			{
			if (tmp == iHead)
				{
				iHead = tmp->iNextItemPtr;
				iHead->iPrevItemPtr = 0;
							
				if (iTail == tmp)
					iTail = iHead;
					
				delete tmp;
				}
			else
				{
				tmp->iPrevItemPtr->iNextItemPtr = tmp->iNextItemPtr;
				tmp->iNextItemPtr->iPrevItemPtr = tmp->iPrevItemPtr;
				
				if (tmp == iTail)
					iTail = tmp->iPrevItemPtr;
					
				delete tmp;
				}
				
			return TRUE;
			}
			
		tmp = tmp->iNextItemPtr;
		}
		
	return FALSE;
}

/**
 * 
 */
template<class T> DListItem<T>* DList<T>::GetHead()
{
	Trace(("DList::GetHead()"));

	return iHead;
}

/**
 * 
 */
template<class T> DListItem<T>* DList<T>::GetTail()
{
	Trace(("DList::GetTail()"));
	return iTail;
}

/**
 * 
 */
template<class T> DListIterator<T>::DListIterator(DList<T>& List):
	iList(List),
	iCurrentItem(0)
{
}

/**
 * 
 */
template<class T> DListIterator<T>::~DListIterator()
{
}

/**
 * 
 */
template<class T> T* DListIterator<T>::GoHead()
{
	iCurrentItem = iList.GetHead();

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

/**
 * 
 */
template<class T> T* DListIterator<T>::GoTail()
{
	iCurrentItem = iList.GetTail();

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

/**
 * 
 */
template<class T> T*	DListIterator<T>::GetNext()
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
template<class T> T*	DListIterator<T>::GetPrev()
{
	if (iCurrentItem)
		iCurrentItem = iCurrentItem->iPrevItemPtr;

	if (iCurrentItem)
		return iCurrentItem->iItemPtr;
	else
		return 0;
}

#endif
