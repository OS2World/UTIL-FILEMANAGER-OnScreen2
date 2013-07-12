/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(SLIST_HPP)
#define SLIST_HPP

#include <sysdep.hpp>
#include <tracer.hpp>
#include <conditio.hpp>
#include <stdio.h>
#include <stdlib.h>

// external classes
class EXP_CLASS PIOBuffer;

template<class T>
class EXP_CLASS SList;

template<class T>
class SListIterator;

/**
 * 
 */
template<class T>
class EXP_CLASS SListItem
{
public:
	SListItem(T* ItemPtr, SListItem<T>* NextItemPtr);
	~SListItem();

	operator	T*();
protected:
private:
	T*					iItemPtr;
	SListItem<T>*	iNextItemPtr;

	friend class SList<T>;
	friend class SListIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS SList
{
public:
	SList(BOOL shouldDelete = TRUE);
	~SList();

	T*			Add(T* ItemPtr);
	T*			AddBefore(T* ItemBefore, T* ItemPtr);
	T*			AddAfter(T* ItemAfter, T* ItemPtr);
	BOOL		Remove(T* ItemPtr);
	void		Clear();
	LONG	   Size() const;
	const T*	GetItem(int position) const;
	const T*	operator[](int position) const;
	T*		   operator[](int position);

	const SList<T>& operator=(const SList<T>& list);
	SListItem<T>* Contains(T* ItemPtr);
	SListItem<T>* GetHead();
	SListItem<T>* GetTail();

protected:
private:
	SListItem<T>*	iHead;
	SListItem<T>*	iTail;
	BOOL			iShouldDelete;

	friend class SListIterator<T>;
};

/**
 * 
 */
template<class T>
class EXP_CLASS SListIterator
{
public:
	SListIterator(SList<T>& List);
	~SListIterator();
	T* GoHead();
	T* GoTail();
	T* GetPrev();
	T* GetNext();

protected:
private:
	SList<T>&		iList;
	SListItem<T>*	iCurrentItem;
};

/**
 * 
 */
template<class T> SListItem<T>::SListItem(T* ItemPtr, SListItem<T>* NextItemPtr):
	iItemPtr(ItemPtr),
	iNextItemPtr(NextItemPtr)
{
	MTrace(("SListItem::SListItem()"));
}

/**
 * 
 */
template<class T> SListItem<T>::~SListItem()
{
	MTrace(("SListItem::~SListItem()"));
}

/**
 * 
 */
template<class T> SListItem<T>::operator T*()
{
	MTrace(("SListItem::operator T*()"));

	MFailIf(iItemPtr == 0);

	return iItemPtr;
}

/**
 * 
 */
template<class T> SList<T>::SList(BOOL shouldDelete):
	iShouldDelete(shouldDelete)
{
	MTrace(("SList::SList()"));
	iHead = iTail = 0;
}

/**
 * 
 */
template<class T> SList<T>::~SList()
{
	MTrace(("SList::~SList()"));
	Clear();
}

/**
 * 
 */
template<class T>	const SList<T>& SList<T>::operator=(const SList<T>& list)
{
	MTrace(("SList::operator=()"));
	// if the 'list' contains anything, then traverse the list
	// adding the items to the current list one at a time.
	if (list.iHead)
		{
		SListItem<T>*	current = list.iHead;

		while (current)
			{
			MFailIf(current == 0);
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
template<class T> void SList<T>::Clear()
{
	MTrace(("SList::Clear()"));
	// if the list contains anything, then traverse the list
	// deleting the items one at a time.
	if (iHead)
		{
		SListItem<T>*	current = iHead;

		while (current)
			{
			SListItem<T>*	tmp = current;

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
template<class T> LONG SList<T>::Size() const
{
	LONG	count = 0;
	MTrace(("SList::Count()"));
	// if the list contains anything, then traverse the list
	// incrementing the count for each item found.
	if (iHead)
		{
		SListItem<T>*	current = iHead;

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
template<class T> const T* SList<T>::operator[](int position) const
{
	MTrace(("SList::operator[] const"));
	return this->GetItem(position);
}

/**
 * 
 */
template<class T> T* SList<T>::operator[](int position)
{
	MTrace(("SList::operator[]"));
	return (T*)this->GetItem(position);
}

/**
 * 
 */
template<class T> const T* SList<T>::GetItem(int position) const
{
	MTrace(("SList::GetItem()"));

	if (iHead)
		{
		SListItem<T>*	current = iHead;

		while (position > 0 && current)
			{
			SListItem<T>*	tmp = current;

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
template<class T> SListItem<T>* SList<T>::Contains(T* ItemPtr)
{
	if (iHead)
		{
		SListItem<T>* tmp = iHead;
		
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
template<class T> T*	SList<T>::Add(T* ItemPtr)
{
	MTrace(("SList::Add()"));

	SListItem<T>* NewItem = new SListItem<T>(ItemPtr, 0);

	if (!NewItem)
		return 0;

	if (iHead)
		{
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
template<class T> T*	SList<T>::AddBefore(T* ItemBefore, T* ItemPtr)
{
	MTrace(("SList::AddBefore()"));

	if (iHead)
		{
		if (iHead->iItemPtr == ItemBefore)
			{
			SListItem<T>* NewItem = new SListItem<T>(ItemPtr, iHead);
			
			if (NewItem)
				{
				iHead = NewItem;
				
				return ItemPtr;
				}
			}
		else
			{
			SListItem<T>* tmp = iHead;

			while (tmp)
				{
				if (tmp->iNextItemPtr && tmp->iNextItemPtr->iItemPtr == ItemBefore)
					break;
					
				tmp = tmp->iNextItemPtr;
				}
				
			if (tmp)
				{
				SListItem<T>* NewItem = new SListItem<T>(ItemPtr, tmp->iNextItemPtr);
				
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
template<class T> T*	SList<T>::AddAfter(T* ItemAfter, T* ItemPtr)
{
	MTrace(("SList::AddAfter()"));

	if (iHead)
		{
		SListItem<T>* tmp = iHead;
		
		while (tmp && tmp->iItemPtr != ItemAfter)
			tmp = tmp->iNextItemPtr;

		if (tmp)
			{
			SListItem<T>* NewItem = new SListItem<T>(ItemPtr, tmp->iNextItemPtr);

			if (NewItem)
				{				
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
template<class T> BOOL SList<T>::Remove(T* ItemPtr)
{
	SListItem<T>* tmp = iHead;
	SListItem<T>* prev = 0;
			
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
template<class T> SListItem<T>* SList<T>::GetHead()
{
	MTrace(("SList::GetHead()"));

	return iHead;
}

/**
 * 
 */
template<class T> SListItem<T>* SList<T>::GetTail()
{
	MTrace(("SList::GetTail()"));
	return iTail;
}

/**
 * 
 */
template<class T> SListIterator<T>::SListIterator(SList<T>& List):
	iList(List),
	iCurrentItem(0)
{
}

/**
 * 
 */
template<class T> SListIterator<T>::~SListIterator()
{
}

/**
 * 
 */
template<class T> T* SListIterator<T>::GoHead()
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
template<class T> T* SListIterator<T>::GoTail()
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
template<class T> T*	SListIterator<T>::GetNext()
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
template<class T> T*	SListIterator<T>::GetPrev()
{
	// can't go back past the head of the list
	if (iCurrentItem == iList.GetHead())
		iCurrentItem = 0;
		
	if (iCurrentItem)
		{
		SListItem<T>*	tmp = iList.GetHead();
		
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
