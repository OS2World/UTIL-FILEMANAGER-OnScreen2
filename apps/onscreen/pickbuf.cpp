/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: pickbuf.cpp 2.50 1996/03/17 22:35:02 jallen Exp jallen $";
#pragma warn +use

/**
 * 
 */
void chooseBuffer(SList<Viewer>& viewedFiles, int& currFile)
{
	static int startPos = 0;

	PickList	pl(FALSE);

	if (viewedFiles.Size() == 1)
		{
		displayErrMsg("can't choose, no other files loaded");
		return;
		}

	for (int i=0; i < viewedFiles.Size(); i++)
		{
		CString tmp;

		if (!viewedFiles[i]->iFileInfo->iFromStdin)
			{
			if (viewedFiles[i]->iFileInfo->iFileName.length() > 50)
				{
				tmp = SubCString(viewedFiles[i]->iFileInfo->iDirName, 1, 50 - (viewedFiles[i]->iFileInfo->iName.length() + 4));
				tmp = tmp + CString("...\\") + viewedFiles[i]->iFileInfo->iName;
				}
			else
				{
				tmp = viewedFiles[i]->iFileInfo->iFileName;
				}

			CString date = viewedFiles[i]->iFileInfo->iFileDate, time = viewedFiles[i]->iFileInfo->iFileTime;

			pl.Add(sformat("%10s %7s %10ld %s",
								(CPCHAR)date,
								(CPCHAR)time,
								viewedFiles[i]->iFileInfo->iFileSize,
								(CPCHAR)tmp));
			}
		else
			{
			tmp = viewedFiles[i]->iFileInfo->iName;
			CString date = systemDate(), time = systemTime();

			pl.Add(sformat("%10s %7s %10ld %s",
								(CPCHAR)date,
								(CPCHAR)time,
								viewedFiles[i]->iFileInfo->iFileSize,
								(CPCHAR)tmp));
			}
		}

	int choice = pl.Pick("Select Buffer", startPos);

	if (choice != -1)
		{
		viewedFiles[currFile]->dumpFile();
		currFile = choice;
		viewedFiles[currFile]->loadFile();
		}

	if (currFile < viewedFiles.Size())
		{
		viewedFiles[currFile]->displayFileInfo();
		viewedFiles[currFile]->displayData();
		}
}
