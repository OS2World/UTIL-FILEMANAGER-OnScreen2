/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(ARRAY_HPP)
#define ARRAY_HPP

#include <sysdep.hpp>
#include <conditio.hpp>
#include <tracer.hpp>
#include <iostream.h>

/**
 * 
 */
template<class T>
class EXP_CLASS Array{
public:
	Array(int size = 0,
			BOOL dynamic = FALSE, int expandSize = 64);
	virtual ~Array();
	void	Erase();
	void SetDynamic(BOOL isDynamic);
	int Size() const;
	T& operator[] (int index);
	const T& operator[] (int index) const;

	int operator==(Array<T>& otherArray) {
		return Size() == otherArray.Size();
		}

	int operator!=(Array<T>& otherArray) {
		return Size() != otherArray.Size();
		}

	const Array& operator=(Array<T>& otherArray);

protected:
	int   	iSize;
	int   	iMax;
	int		iExpandSize;
	BOOL  	iDynamic;
	T**		iData;

private:
	void 	Init();
	T*		GetElement(int index);
};

/**
 * 
 */
template<class T> Array<T>::Array(
	int 	size,
	BOOL 	dynamic,
	int		expandSize):
iSize(size),
iMax(size),
iDynamic(dynamic),
iExpandSize(expandSize),
iData(0)
{
	MTrace(("Array<T>::Array()"));
	Init();
}

/**
 * 
 */
template<class T> Array<T>::~Array()
{
	MTrace(("Array<T>::~Array()"));
	Erase();
}

/**
 * 
 */
template<class T> void Array<T>::Erase()
{
	if (iData)
		{
		for (int i = 0; i < iSize; i++)
			{
			delete (T*)((T**)iData)[i];
			}

		free(iData);

		iData = 0;
		iSize = 0;
		iMax = 0;
		}
}

/**
 * 
 */
template<class T> void Array<T>::SetDynamic(BOOL isDynamic)
{
	iDynamic = isDynamic ? TRUE : FALSE;
}

/**
 * 
 */
template<class T> int Array<T>::Size() const
{
	return iSize;
}

/**
 * 
 */
template<class T>
T* Array<T>::GetElement(int index)
{
	if (iDynamic && index == iSize)
		{
		Init();	// call Init() just in case this is the first access
					// to an empty array

		if (index >= iMax)
			{
			int	oldMax = iMax;
			iMax = index + (iExpandSize - (index % iExpandSize));
			iData = (T**)realloc(iData, iMax * sizeof(T**));

			for (int i = oldMax; i < iMax; i++)
				{
				iData[i] = 0;
				}
			}

		iSize++;
		iData[index] = new T;
		}

	MFailIf(index < 0 || index >= iSize);
	MFailIf(iData[index] == 0);

	return iData[index];
}

/**
 * 
 */
template<class T> T& Array<T>::operator[] (int index)
{
	return *GetElement(index);
}

/**
 * 
 */
template<class T> const T& Array<T>::operator[] (int index) const
{
	return *GetElement(index);
}

/**
 * 
 */
template<class T> void Array<T>::Init()
{
	if (!iData && iMax > 0)
		{
		iData = (T**)calloc(iMax, sizeof(T**));
      int i;

		for (i = 0; i < iSize; i++)
			{
			iData[i] = new T;
			}

		while (i < iMax)
			{
			iData[i++] = 0;
			}
		}
}

//-----------------------------------oOo--------------------------------------

template<class T> const Array<T>& Array<T>::operator=(Array<T>& otherArray)
{
	Erase();
	SetDynamic(TRUE);

	for (int i=0; i < otherArray.iSize; i++)
		{
		(*this)[i] = otherArray[i];
		}

	SetDynamic(FALSE);

	return *this;
}

/**
 * 
 */
template<class T>
class EXP_CLASS SortableArray: public Array<T>
{
public:
	SortableArray(int size = 0,
			BOOL dynamic = FALSE, int expandSize = 64);

	void sort(int cmp(const T* elem1, const T* elem2) = 0);

private:
	void QSort(T* elems[], int l, int r, int cmp(const T* elem1, const T* elem2));
};

/**
 * 
 */
template<class T> SortableArray<T>::SortableArray(
	int size,
	BOOL dynamic,
	int expandSize):Array<T>(size, dynamic, expandSize)
{
}

/**
 * 
 */
template<class T> void SortableArray<T>::QSort(
	T* elems[], int l, int r, int cmp(const T* elem1, const T* elem2))
{
	int i=l, j=r;
	static T		pivot;
	static int  ranpos;

#if defined(__BORLANDC__)
	ranpos = l + random(r-l);
#else
	ranpos = l + ((r-l) / 2);
#endif

	pivot = *elems[ranpos];

	do
		{
		if (cmp)
			{
			while (cmp(elems[i], &pivot) < 0)
				{
				i++;
				}
			}
		else
			{
			while (*elems[i] < pivot)
				{
				i++;
				}
			}

		if (cmp)
			{
			while (cmp(&pivot, elems[j]) < 0)
				{
				j--;
				}
			}
		else
			{
			while (pivot < *elems[j])
				{
				j--;
				}
			}

		if (i <= j)
			{
			if (i != j)
				{
				T *tmp;

				tmp = elems[i];
				elems[i] = elems[j];
				elems[j] = tmp;
				}

			i++;
			j--;
			}
		}
	while (!(i > j));

	if (l < j)
		{
		QSort(elems, l, j, cmp);
		}

	if (i < r)
		{
		QSort(elems, i, r, cmp);
		}
}

/**
 * 
 */
template<class T> void SortableArray<T>::sort(int cmp(const T* elem1, const T* elem2))
{
	MStackCall(__FILE__, __LINE__);
	if (Size() > 1)
		QSort(iData, 0, iSize-1, cmp);
}

#endif
