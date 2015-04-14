/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: picklist.cpp 2.50 1996/03/17 22:35:03 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
PickList::PickList(BOOL multiSelect):
	iMultiSelect(multiSelect),
	iFirstItem(0),
	iCurrItem(0)
{
}

/**
 * 
 */
void PickList::Add(CString s)
{
	iListItems.Add(new PickListItem(s));
}

/**
 * 
 */
PickListItem& PickList::GetPickListItem(int i)
{
	return *iListItems[i];
}

/**
 * 
 */
void PickList::displayItemAt(int itemNo, int row)
{
	WaitSync();
	MoveToXY(1, row);

	if (iListItems[itemNo]->IsSelected())
		{
		setTextAttr(setupInfo.curFileTextForeColor, setupInfo.curFileTextBackColor);
		}
	else
		{
		setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);
		}

	tprintf("%c%-*s", itemNo == iCurrItem ? '':' ',
				di.screenwidth-1, (CPCHAR)iListItems[itemNo]->GetString());

	setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);
	ClearSync();
}

/**
 * 
 */
void PickList::displayList()
{
	int	y = 2;
	int	count = di.screenheight-2;

	for (int i=iFirstItem; count > 0 && i < iListItems.Size(); i++, count--, y++)
		{
		displayItemAt(i, y);
		}

	setTextAttr(setupInfo.fileTextForeColor, setupInfo.fileTextBackColor);

	while (y < di.screenheight)
		{
		MoveToXY(1, y++);
		clrline();
		}
}

/**
 * 
 */
int PickList::GetCurrItem()
{
	return iCurrItem;
}

/**
 * 
 */
int PickList::Size()
{
	return iListItems.Size();
}

/**
 * 
 */
int PickList::Pick(CString msg, int& startPos, int* count)
{
	int key = 0;

	if (startPos >= 0 && startPos < iListItems.Size())
		{
		iCurrItem = startPos;

		if (iFirstItem + (di.screenheight-2) < iCurrItem)
			{
			iFirstItem = iCurrItem - ((di.screenheight-2) /2);

			if (iFirstItem < 0)
				iFirstItem = 0;

			if (iFirstItem + (di.screenheight-2) > iListItems.Size())
				iFirstItem = iListItems.Size() - (di.screenheight-2);
			}
		}

	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);
	MoveToXY(1, 1);
	clrline();
	tprintf("%*s%s", (di.screenwidth - msg.length())/2, "", (CPCHAR)msg);

	if (!iMultiSelect)
		{
		iListItems[iCurrItem]->Select();
		}

	displayList();

	for (; key != '\r';)
		{
		displayItemAt(iCurrItem, (iCurrItem-iFirstItem)+2);
		key = getKey();

		if (!iMultiSelect)
			{
			iListItems[iCurrItem]->Deselect();
			}

		switch (key)
			{
      	case 0xFF47: // home
				{
				if (iCurrItem != 0)
            	{
					iCurrItem = 0;
					iFirstItem = 0;
					displayList();
					}
         	else
            	{
					displayErrMsg("Top of list");
					}
				}
      	break;

      	case 0xFF4F: // end
         	{
         	if (iCurrItem == iListItems.Size()-1)
            	{
					displayErrMsg("Bottom of list");
					}
				else
					{
					iCurrItem = iListItems.Size()-1;
	         	iFirstItem = iListItems.Size() - (di.screenheight-2);

					if (iFirstItem < 0)
						{
						iFirstItem = 0;
						}

					displayList();
					}
         	}
			break;

			case 0xFF51: // page down
				{
				if (iCurrItem == iListItems.Size()-1)
            	{
					displayErrMsg("Bottom of list");
					break;
					}

				if (iCurrItem == iFirstItem + di.screenheight-3)
					{
					iFirstItem += di.screenheight-3;
					}

				iCurrItem = iFirstItem + di.screenheight-3;

				if (iFirstItem + (di.screenheight-2) > iListItems.Size())
            	{
            	iFirstItem = iListItems.Size() - (di.screenheight-2);

            	if (iFirstItem < 0)
               	{
						iFirstItem = 0;
						}

					iCurrItem = iListItems.Size()-1;
            	}

				displayList();
         	}
      	break;

      	case 0xFF49: // page up
         	{
				if (iFirstItem == 0 && iCurrItem == 0)
            	{
					displayErrMsg("Top of list");
					break;
					}

				if (iCurrItem > iFirstItem)
	         	iCurrItem = iFirstItem;
				else
	         	iFirstItem -= di.screenheight-2;

         	if (iFirstItem < 0)
            	{
					iFirstItem = 0;
            	}

				iCurrItem = iFirstItem;
				displayList();
         	}
      	break;

lineDown:
			case 0xFF50: // down line
				{
				if (iCurrItem < iListItems.Size()-1)
					{
					int oldCurrItem = iCurrItem;
					iCurrItem++;
					displayItemAt(oldCurrItem, (oldCurrItem-iFirstItem)+2);

					if (iCurrItem - iFirstItem > di.screenheight-3)
						{
						iFirstItem++;
						MoveTextBuf(1, 3, di.screenwidth, di.screenheight-1, 1, 2);
						displayItemAt(iFirstItem+(di.screenheight-3), di.screenheight-1);
						}

					//displayItemAt(iCurrItem, (iCurrItem-iFirstItem)+2);
					}
				else
					{
					displayErrMsg("Bottom of list");
					}
				}
			break;

			case 0xFF48: // up line
				{
				if (iCurrItem > 0)
					{
					int oldCurrItem = iCurrItem;
					iCurrItem--;
					displayItemAt(oldCurrItem, (oldCurrItem-iFirstItem)+2);

					if (iCurrItem < iFirstItem)
						{
						iFirstItem = iCurrItem;
						MoveTextBuf(1, 2, di.screenwidth, di.screenheight-2, 1, 3);
						displayItemAt(iFirstItem, 2);
						}

					//displayItemAt(iCurrItem, (iCurrItem-iFirstItem)+2);
					}
				else
					{
					displayErrMsg("Top of list");
					}
				}
			break;

			case ' ':
				{
				if (iMultiSelect)
					{
					if (iListItems[iCurrItem]->IsSelected())
						{
						iListItems[iCurrItem]->Deselect();
						}
					else
						{
						iListItems[iCurrItem]->Select();
						}

					goto lineDown;
					}
				}
			break;

			case 'x':
			case 'X':
			case 'q':
			case 'Q':
			case VKALT_X:
			case VKCTRL_X:
			case VKALT_A:
				{
				cleanup(key);
				exit(EXIT_SUCCESS);
				}
			break;

			case 0x001B:
				{
				return -1;
				}
			}

		if (!iMultiSelect)
			{
			iListItems[iCurrItem]->Select();
			}
		}

	if (iMultiSelect && count)
		{
		*count = 0;

		for (int i=0; i < iListItems.Size(); i++)
			{
			if (iListItems[i]->IsSelected())
				{
				(*count)++;
				}
			}
		}

	startPos = iCurrItem;

	return iCurrItem;
}
