/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: osview.cpp 2.50 1996/03/17 22:35:01 jallen Exp jallen $";
#pragma warn +use

CPCHAR hexChar="0123456789ABCDEF";
SList<CString>	searchTextHistory;
SList<CString>	gotoLineHistory;
SList<CString>	gotoOffsetHistory;

const int LINE_BUF_MAX=1024;
DisplayMode	gDisplayMode = DM_TEXT;
BOOL			gWordBreak = FALSE;
BOOL			gSearchForward;

// macros for testing the current style or if it's NULL then tests
// the global settings
#define DoExpandTabs(style) (!iBinary && (style && *style->iExpandTabs.GetItem()))
#define HighBitFilter(style) (!iBinary && (style && *style->iHighBitFilter.GetItem()))
#define WithLayout(style) (!iBinary && (style && *style->iTextWithLayout.GetItem()))

/**
 * 
 */
int IsString(char* s, Style* sty)
{
	return strchr(*sty->iString.GetItem(), *s) != 0;
}

/**
 * 
 */
int IsEolComment(char* s, Style* sty)
{
	CString*				eolComment;
	Set<CString>*			eolCommentList = sty->iEolComment.GetItem();

	if (eolCommentList)
		{
		SetIterator<CString>	eolCommentIterator(*eolCommentList);

		for (eolComment = eolCommentIterator.GoHead(); eolComment;
				eolComment = eolCommentIterator.GetNext())
			{
			if (*sty->iCaseSensitive.GetItem())
				{
				if (strncmp(s, *eolComment, eolComment->length()) == 0)
					return 1;
				}
			else
				{
				if (strncmpi(s, *eolComment, eolComment->length()) == 0)
			 	   return 1;
				}
			}
		}

	return 0;
}

/**
 * 
 */
int IsBeginComment(char* s, Style* sty)
{
	CString*				openComment;
	Set<CString>*			openCommentList = sty->iOpenComment.GetItem();
	SetIterator<CString>	openCommentIterator(*openCommentList);

	if (openCommentList)
		{
		for (openComment = openCommentIterator.GoHead(); openComment;
				openComment = openCommentIterator.GetNext())
			{
			if (strncmp(s, *openComment, openComment->length()) == 0)
				return 1;
			}
		}

	return 0;
}

/**
 * 
 */
char* IsEndComment(char* s, Style* sty)
{
	CString*				closeComment;
	Set<CString>*			closeCommentList = sty->iCloseComment.GetItem();
	SetIterator<CString>	closeCommentIterator(*closeCommentList);

	if (closeCommentList)
		{
		for (closeComment = closeCommentIterator.GoHead(); closeComment;
				closeComment = closeCommentIterator.GetNext())
			{
			if (strncmp(s, *closeComment, closeComment->length()) == 0)
				return (char*)(const char*)*closeComment;
			}
		}

	return 0;
}

/**
 * 
 */
char* IsNumericPrefix(char* s, Style* sty)
{
	CString*				numericPrefix;
	Set<CString>*			numericPrefixList = sty->iNumericPrefix.GetItem();
	SetIterator<CString>	numericPrefixIterator(*sty->iNumericPrefix.GetItem());

	if (numericPrefixList)
		{
		for (numericPrefix = numericPrefixIterator.GoHead(); numericPrefix;
				numericPrefix = numericPrefixIterator.GetNext())
			{
			if (strncmp(s, *numericPrefix, numericPrefix->length()) == 0)
				return (char*)(const char*)*numericPrefix;
			}
		}

	return 0;
}

/**
 * 
 */
int IsSymbol(char ch, Style* sty)
{
	return strchr(*sty->iSymbols.GetItem(), ch) != 0;
}

/**
 * 
 */
int keywordCmp(const char* keyword, const char* s, BOOL caseSensitive)
{
	const char* p1 = keyword;
	const char* p2 = s;
	char c1, c2;

	if (caseSensitive)
		{
		while ((c1 = *p1) == (c2 = *p2)
				&& c1 != '\0')
			{
			p1++;
			p2++;
			}
		}
	else
		{
		while ((c1 = (char)toupper(*p1)) == (c2 = (char)toupper(*p2))
				&& c1 != '\0')
			{
			p1++;
			p2++;
			}
		}

	if (c1 == '\0' && !isalnum(c2) && c2 != '_')
		return 0;

	return (c1 - c2);
}

/**
 * 
 */
char* IsReservedWord(char* s, Style* sty)
{
	int 	high = sty->iReserved.Size(), low=0;
	int	i, j;

	while (low <= high)
		{
		i = (low+high)/2;

		if (i >= sty->iReserved.Size())
			return 0;

	  j = keywordCmp(sty->iReserved[i].iKeyword, s, *sty->iCaseSensitive.GetItem());

		if (j == 0)
			{
			while (i < sty->iReserved.Size()-1 &&
					keywordCmp(sty->iReserved[i+1].iKeyword, s, *sty->iCaseSensitive.GetItem()) == 0)
				i++;

			return (char*)(const char*)sty->iReserved[i].iKeyword;
			}
		else
		if (j < 0)
			{
			low = i+1;
			}
		else
			{
			high = i-1;
			}
		}

	return 0;
}

/**
 * 
 */
long CalcPercent(long lineCount, long topLine)
{
	if (lineCount == 0)
		return 0;

	lineCount--;
	topLine += di.screenheight-2;

	if (topLine > lineCount)
		return 100;

	if ((lineCount*10000) / (topLine) == 0)
		return 0;

	return 1000000 / ((lineCount*10000) / (topLine));
}

/**
 * 
 */
LinePtr::LinePtr():
	iLineStatus(LS_NONE),
	iText(0),
	iNullByte(0),
	iOriginalChar(0)
{
}

/**
 * 
 */
Style* findStyleForFile(const CString fileName)
{
	Style*			foundStyle = 0;
	const char* 	lastDot = strrchr(fileName, '.');

	// we have a style list
	if (styles.Size() != 0)
		{
		SListIterator<Style>	styleIterator(styles);
		Style*					style;

		for (style = styleIterator.GoHead();
				!foundStyle && style;
				style = styleIterator.GetNext())
			{
			if (style->iExtensions.GetItem())
				{
				SetIterator<CString>	extIterator(*style->iExtensions.GetItem());
				CString*				ext;

				ext = extIterator.GoHead();

				for (ext = extIterator.GoHead();
						!foundStyle && ext;
						ext = extIterator.GetNext())
					{
					if (lastDot && stricmp(*ext, lastDot) == 0)
						{
						foundStyle = style;
						}
					}
				}
			}
		}

	// no match and a style list
	if (!foundStyle)
		{
		// select default style if there is one
		foundStyle = gDefaultStyle;
		}

	return foundStyle;
}

/**
 * 
 */
Viewer::Viewer(FileInfo* dirent):
	iFileInfo(dirent),
	iData(0),
	iLineCount(0),
	iLinePtr(0),
	iHexTopLine(0),
	iTopLine(0),
	iSelectedLine(-1),
	iSelectedPos(0),
	iSelectedOffset(-1),
	iSelectedCount(0),
	iColumn(0),
	iIsBold(FALSE),
	iIsUnderlined(FALSE),
	iInComment(FALSE),
	iInPreprocessorStatement(FALSE),
	iCurrStyle(0),
	iChanged(FALSE)
{
	iCurrStyle = findStyleForFile(iFileInfo->iFileName);

	if (*iCurrStyle->iFilterEnabled.GetItem() && *iCurrStyle->iExternalFilterCmd.GetItem() != "")
		iFileInfo->iFromStdin = TRUE;
}

/**
 * 
 */
Viewer::~Viewer()
{
	dumpFile();

	if (iFileInfo)
		{
		delete iFileInfo;
		iFileInfo = 0;
		}
}

/**
 * 
 */
void Viewer::displayArrows()
{
	WaitSync();

	switch (getDisplayMode())
		{
		case DM_TEXT:
			{
			MoveToXY(di.screenwidth - 9, di.screenheight);
			setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);

			if (iTopLine < iLineCount-1)
				{
				tprintf("");
				}
			else
				{
				tprintf(" ");
				}

			if (iTopLine > 0)
				{
				tprintf("");
				}
			else
				{
				tprintf(" ");
				}
			}
		break;

		case DM_HEX:
			{
			MoveToXY(di.screenwidth - 9, di.screenheight);
			setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);

			if (iHexTopLine < hexLineCount()-1)
				{
				tprintf("");
				}
			else
				{
				tprintf(" ");
				}

			if (iHexTopLine > 0)
				{
				tprintf("");
				}
			else
				{
				tprintf(" ");
				}
			}
		break;
		}

	ClearSync();
}

/**
 * 
 */
long Viewer::hexLineCount()
{
	if ((iFileInfo->iFileSize % BYTES_PER_LINE_HEX_MODE) == 0)
		return iFileInfo->iFileSize / BYTES_PER_LINE_HEX_MODE;
	else
		return (iFileInfo->iFileSize / BYTES_PER_LINE_HEX_MODE)+1;
}

/**
 * 
 */
void Viewer::displayFileInfo0(BOOL lineInfoOnly)
{
	if (lineInfoOnly && !iChanged)
		{
		MoveToXY(1, 1);

		if (iColumn > 0)
			{
			tprintf("%10li � %10li � +%4li",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				iLineCount,
				iColumn);
			}
		else
			{
			tprintf("%10li � %10li �      ",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				iLineCount);
			}
		}
	else
		{
		MoveToXY(1, 1);

		CString tmp;

		if (iFileInfo->iFromStdin && (
			*iCurrStyle->iFilterEnabled.GetItem() &&
			*iCurrStyle->iExternalFilterCmd.GetItem() == ""))
			{
			tmp = stdinName + CString(' ', (di.screenwidth-58)-strlen(stdinName));
			}
		else
			{
			if (iFileInfo->iFileName.length() > (di.screenwidth-58))
				{
				tmp = SubCString(iFileInfo->iDirName, 1, (di.screenwidth-58) - (iFileInfo->iName.length() + 4));
				tmp = tmp + CString("...\\") + iFileInfo->iName;

				if (tmp.length() > (di.screenwidth-58))
					{
					tmp = SubCString(tmp, 1, di.screenwidth-59);
					tmp = tmp + "";
					}
				}
			else
			if (tmp.length() < (di.screenwidth-58))
				tmp = iFileInfo->iFileName + CString((di.screenwidth-58)-iFileInfo->iFileName.length());
			}

		CString date = iFileInfo->iFileDate, time = iFileInfo->iFileTime;

		if (iColumn > 0)
			{
			tprintf("%10li � %10li � +%4li � %s � %s �%c%s",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				iLineCount,
				iColumn,
				(CPCHAR)date,
				(CPCHAR)time,
				iChanged ? '*':' ',
				(CPCHAR)tmp);
			}
		else
			{
			tprintf("%10li � %10li �       � %s � %s �%c%s",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				iLineCount,
				(CPCHAR)date,
				(CPCHAR)time,
				iChanged ? '*':' ',
				(CPCHAR)tmp);
			}

		clrline();
		}
}

/**
 * 
 */
void Viewer::displayFileInfo1(BOOL lineInfoOnly)
{
	if (lineInfoOnly && !iChanged)
		{
		MoveToXY(1, 1);

		if (iColumn > 0)
			{
			tprintf("%10li � %9li%% � +%4li",
				CalcPercent(iLineCount, iTopLine),
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				iColumn);
			}
		else
			{
			tprintf("%10li � %9li%% �      ",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				CalcPercent(iLineCount, iTopLine));
			}
		}
	else
		{
		MoveToXY(1, 1);

		CString tmp;

		if (iFileInfo->iFromStdin && (
			*iCurrStyle->iFilterEnabled.GetItem() &&
			*iCurrStyle->iExternalFilterCmd.GetItem() == ""))
			{
			tmp = stdinName + CString(' ', (di.screenwidth-58)-strlen(stdinName));
			}
		else
			{
			if (iFileInfo->iFileName.length() > (di.screenwidth-58))
				{
				tmp = SubCString(iFileInfo->iDirName, 1, (di.screenwidth-58) - (iFileInfo->iName.length() + 4));
				tmp = tmp + CString("...\\") + iFileInfo->iName;

				if (tmp.length() > (di.screenwidth-58))
					{
					tmp = SubCString(tmp, 1, di.screenwidth-59);
					tmp = tmp + "";
					}
				}
			else
			if (tmp.length() < (di.screenwidth-58))
				tmp = iFileInfo->iFileName + CString((di.screenwidth-58)-iFileInfo->iFileName.length());
			}

		CString date = iFileInfo->iFileDate, time = iFileInfo->iFileTime;

		if (iColumn > 0)
			{
			tprintf("%10li � %9li%% � +%4li � %s � %s �%c%s",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				CalcPercent(iLineCount, iTopLine),
				iColumn,
				(CPCHAR)date,
				(CPCHAR)time,
				iChanged ? '*':' ',
				(CPCHAR)tmp);
			}
		else
			{
			tprintf("%10li � %9li%% �       � %s � %s �%c%s",
				iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
				CalcPercent(iLineCount, iTopLine),
				(CPCHAR)date,
				(CPCHAR)time,
				iChanged ? '*':' ',
				(CPCHAR)tmp);
			}

		clrline();
		}
}

/**
 * 
 */
void Viewer::displayFileInfo2(BOOL lineInfoOnly)
{
	if (lineInfoOnly && !iChanged)
		{
		MoveToXY(37, 1);
		tprintf("%10ld", iTopLine+1 > iLineCount ? iLineCount:iTopLine+1);
		}
	else
		{
		MoveToXY(1, 1);

		CString tmp;

		if (iFileInfo->iFromStdin && (
			*iCurrStyle->iFilterEnabled.GetItem() &&
			*iCurrStyle->iExternalFilterCmd.GetItem() == ""))
			{
			tmp = stdinName + CString(' ', 30-strlen(stdinName));
			}
		else
			{
			if (iFileInfo->iFileName.length() > 30)
				{
				tmp = SubCString(iFileInfo->iDirName, 1, 30 - (iFileInfo->iName.length() + 4));
				tmp = tmp + CString("...\\") + iFileInfo->iName;

				if (tmp.length() > (di.screenwidth-30))
					{
					tmp = SubCString(tmp, 1, di.screenwidth-31);
					tmp = tmp + "";
					}
				}
			else
			if (tmp.length() < 30)
				tmp = iFileInfo->iFileName + CString(30-iFileInfo->iFileName.length());
			}


		CString date = iFileInfo->iFileDate, time = iFileInfo->iFileTime;

		tprintf("%s � %s � %10ld � %10ld �%c%s",
			(CPCHAR)date,
			(CPCHAR)time,
			iFileInfo->iFileSize,
			iTopLine+1 > iLineCount ? iLineCount:iTopLine+1,
			iChanged ? '*':' ',
			(CPCHAR)tmp);

		clrline();
		}
}

/**
 * 
 */
void Viewer::displayFileInfo(BOOL lineInfoOnly)
{
	WaitSync();

	setTextAttr(setupInfo.statusTextForeColor, setupInfo.statusTextBackColor);

	switch (*iCurrStyle->iTopLineFormat.GetItem())
		{
		case 0:
			displayFileInfo0(lineInfoOnly);
		break;

		case 1:
			displayFileInfo1(lineInfoOnly);
		break;

		case 2:
			displayFileInfo2(lineInfoOnly);
		break;

		default: // error
			;
		}

	ClearSync();
}

/**
 * 
 */
void Viewer::clearSelected()
{
	iSelectedLine = -1;
	iSelectedPos = 0;
	iSelectedCount = 0;
	displayData();
}

/**
 * 
 */
void Viewer::displayTab(int& 			dispCol,
								Style* 		currStyle,
								BYTE 			defAttrib,
								DisplayCell*&	bp,
								int& 			bufPos)
{
	int tmpDispCol = dispCol;

	dispCol = nextTabStop(dispCol, currStyle);

	while (tmpDispCol++ < dispCol)
		{
		bp->Char.AsciiChar = ' ';
		bp->Attributes = defAttrib;
		bp++;
		bufPos++;
		}
}

/**
 * 
 */
void Viewer::displayChar(	char			ch,
									int& 			dispCol,
									BYTE 			defAttrib,
									BYTE 			useAttrib,
									BYTE 			curColor,
									DisplayCell*& bp,
									int& 			bufPos,
									Style*		currStyle)
{
	if (defAttrib == useAttrib)
		{
		useAttrib = (BYTE)(defAttrib & 0xF0) | (curColor);
		}
	else
		{
		useAttrib = defAttrib;
		}

	if (ch == '\t' && DoExpandTabs(currStyle))
		{
		displayTab(dispCol, iCurrStyle, useAttrib, bp, bufPos);
		}
	else
		{
		ch = (char)(HighBitFilter(currStyle) ? ch & 0x7F : ch);
		if (ch != '\r' && ch != '\n')
			{
			bp->Char.AsciiChar = ch;
			bp->Attributes = useAttrib;
			dispCol++;
			bp++;
			bufPos++;
			}
		}
}

/**
 * 
 */
void Viewer::displayTextLine(int line, int atY)
{
	if (line >= iLineCount)
		{
		MoveToXY(1, atY);
		setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());

		clrline();

#if defined(DISPLAY_END_OF_FILE)
		// TODO: change this to a configurable item
		if (line == iLineCount)
			{
			setTextAttr(*iCurrStyle->iSelectedForeGndColor.GetItem(), *iCurrStyle->iSelectedBackGndColor.GetItem());

			tprintf("  END OF FILE  ");
			}
#endif

		return;
		}

	DisplayCell 	buf[LINE_BUF_MAX];
	BYTE			attribs[LINE_BUF_MAX];
	PCHAR 			pCh;
	DisplayCell*	bp;
	BYTE			defColor;
	BYTE			curColor;
	BYTE			selAttrib;
	BYTE			useAttrib;
	BYTE			boldColor;
	BYTE			underlineColor;
	BYTE			boldAndUnderlineColor;
	BYTE* 			defAttrib = attribs;
	LinePtr*        lp = &iLinePtr[line];

	curColor = defColor  = BYTE(*iCurrStyle->iForeGndColor.GetItem());
	useAttrib = BYTE(*iCurrStyle->iForeGndColor.GetItem() + (*iCurrStyle->iBackGndColor.GetItem()*16));
	selAttrib = BYTE(*iCurrStyle->iSelectedForeGndColor.GetItem() + (*iCurrStyle->iSelectedBackGndColor.GetItem()*16));
	boldColor = (BYTE)*iCurrStyle->iBoldColor.GetItem();
	underlineColor = (BYTE)*iCurrStyle->iUnderlineColor.GetItem();
	boldAndUnderlineColor = (BYTE)*iCurrStyle->iBoldUnderlineColor.GetItem();

	int	col, bufPos, dispCol;

	if (WithLayout(iCurrStyle))
		{
		iIsBold = lp->iLineStatus == LS_BOLD || lp->iLineStatus == LS_BOLD_UNDERLINED;
		iIsUnderlined = lp->iLineStatus == LS_UNDERLINED || lp->iLineStatus == LS_BOLD_UNDERLINED;

		if (iIsUnderlined && iIsBold)
			curColor = boldAndUnderlineColor;
		else
		if (iIsBold)
			curColor = boldColor;
		else
		if (iIsUnderlined)
			curColor = underlineColor;
		}
	else
		{
		iIsBold = iIsUnderlined = FALSE;
		}

	if (iSelectedLine == line)
		{
		if (iSelectedCount == -1)
			{
			memset(&attribs, selAttrib, LINE_BUF_MAX);
			}
		else
		   {
			int i, j;

			memset(&attribs, useAttrib, LINE_BUF_MAX);

			for (i=iSelectedPos, j=0; j < iSelectedCount; i++, j++)
				{
				attribs[i] = selAttrib;
				}
			}
		}
	else
		{
		memset(&attribs, useAttrib, LINE_BUF_MAX);
		}

	if (lp->iLineStatus == LS_INCOMMENT)
		iInComment = TRUE;
	else
		iInComment = FALSE;

	if (lp->iLineStatus == LS_INPREPROCESSOR)
		iInPreprocessorStatement = TRUE;
	else
		iInPreprocessorStatement = FALSE;

	// start moving text into buffer, up to LINE_BUF_MAX chars
	for (	bp = buf, bufPos=0, col=0, dispCol=0, pCh=lp->iText, defAttrib = attribs;
			*pCh && bufPos < LINE_BUF_MAX;
			pCh++, col++)
		{
		if (*iCurrStyle->iSyntaxHighlightEnabled.GetItem() &&
			iCurrStyle->iReserved.Size() > 0)
			{
			// no support for text with layout when
			// syntax highlighting has been enabled
			char* reservedWord = 0;
			char*	numericPrefix = 0;

			if (iInComment)
				{
				char* ec;

				if ((ec = IsEndComment(pCh, iCurrStyle)) != 0)
					{
					int len = strlen(ec);

					for (int i=0; i < len; i++)
						{
						displayChar(*pCh, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iCommentColor.GetItem(), bp, bufPos, iCurrStyle);
						pCh++;
						}

					iInComment = FALSE;
					pCh--;
					}
				else
					{
					displayChar(*pCh, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iCommentColor.GetItem(), bp, bufPos, iCurrStyle);
					}
				}
			else
			if (IsEolComment(pCh, iCurrStyle))
				{
				// end-of line comment

				while (*pCh)
					displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iCommentColor.GetItem(), bp, bufPos, iCurrStyle);

				pCh--;
				}
			else
			if (IsBeginComment(pCh, iCurrStyle))
				{
				displayChar(*pCh, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iCommentColor.GetItem(), bp, bufPos, iCurrStyle);
				iInComment = TRUE;
				}
			else
			if (iInPreprocessorStatement)
				{
				CString*	pClosePreProcessorStr = iCurrStyle->iClosePreprocessor.GetItem();
				if (pClosePreProcessorStr)
					{
					while (*pCh &&
						strncmp(*pClosePreProcessorStr, pCh, pClosePreProcessorStr->length()) != 0)
						{
						if (IsEolComment(pCh, iCurrStyle) || IsBeginComment(pCh, iCurrStyle))
							{
							goto breakOut;
							}

						displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iPreprocessorColor.GetItem(), bp, bufPos, iCurrStyle);
						}

					if (*pCh)
						{
						// we found the end of the preprocessor statement
						int len = pClosePreProcessorStr->length();

						for (int i=0; i < len; i++)
							{
							displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iPreprocessorColor.GetItem(), bp, bufPos, iCurrStyle);
							}
						}
					}
				else
					{
					// assume preprocessor statement ends at EOL
					while (*pCh)
						{
						if (IsEolComment(pCh, iCurrStyle) || IsBeginComment(pCh, iCurrStyle))
							{
							goto breakOut;
							}

						displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iPreprocessorColor.GetItem(), bp, bufPos, iCurrStyle);
						}
					}

				breakOut:
				pCh--;
				}
			else
			if (IsString(pCh, iCurrStyle))
				{
				char ch  = *pCh;

				do
					{
					displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iStringColor.GetItem(), bp, bufPos, iCurrStyle);

					if (*pCh == *iCurrStyle->iEscape.GetItem() && *pCh)
						{
						displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iStringColor.GetItem(), bp, bufPos, iCurrStyle);
						displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iStringColor.GetItem(), bp, bufPos, iCurrStyle);
						}
					}
				while (*pCh && *pCh != ch);

				if (*pCh)
					displayChar(*pCh, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iStringColor.GetItem(), bp, bufPos, iCurrStyle);
				else
					pCh--;
				}
			else
			if (IsSymbol(*pCh, iCurrStyle))
				{
				displayChar(*pCh, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iSymbolsColor.GetItem(), bp, bufPos, iCurrStyle);
				}
			else
			if ((numericPrefix = IsNumericPrefix(pCh, iCurrStyle)) != 0)
				{
				if (numericPrefix)
					{
					int len = strlen(numericPrefix);

					for (int i = 0; i < len; i++)
						{
						displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iNumberColor.GetItem(), bp, bufPos, iCurrStyle);
						}
					}

				do
					{
					displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iNumberColor.GetItem(), bp, bufPos, iCurrStyle);
					}
				while (*pCh && isxdigit(*pCh));

				pCh--;
				}
			else
			if (isdigit(*pCh))
				{
				do
					{
					displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iNumberColor.GetItem(), bp, bufPos, iCurrStyle);
					}
				while (*pCh && isdigit(*pCh));

				pCh--;
				}
			else
			if ((reservedWord = IsReservedWord(pCh, iCurrStyle)) != 0)
				{
				if (iCurrStyle && *iCurrStyle->iCaseConvert.GetItem())
					{
					while (*reservedWord)
						{
						displayChar(*reservedWord++, dispCol, * defAttrib++, useAttrib, *iCurrStyle->iReservedColor.GetItem(), bp, bufPos, iCurrStyle);
						pCh++;
						}
					}
				else
					{
					while (*reservedWord++)
						{
						displayChar(*pCh++, dispCol, * defAttrib++, useAttrib, *iCurrStyle->iReservedColor.GetItem(), bp, bufPos, iCurrStyle);
						}
					}

				pCh--;
				}
			else
			if (isalpha(*pCh) || *pCh == '_')
				{
				do
					{
					displayChar(*pCh++, dispCol, *defAttrib++, useAttrib, *iCurrStyle->iIdentColor.GetItem(), bp, bufPos, iCurrStyle);
					}
				while (*pCh && isalnum(*pCh) || *pCh == '_');

				pCh--;
				}
			else
				{
				displayChar(*pCh, dispCol, *defAttrib++, useAttrib, curColor, bp, bufPos, iCurrStyle);
				}
			}
		else
		if (WithLayout(iCurrStyle) && *pCh == '\b' && pCh > lp->iText && *(pCh+1) != '\0')
			{
			pCh++;
			defAttrib++;
			bp--;
			bp->Char.AsciiChar = (char)(HighBitFilter(iCurrStyle) ? (*pCh) & 0x7F : *pCh);

			if (*(pCh-2) == *pCh)
				bp->Attributes = (BYTE)((*defAttrib) & 0xF0) | (boldColor);
			else
			if (*(pCh-2) == '_')
				bp->Attributes = (BYTE)((*defAttrib) & 0xF0) | (underlineColor);
			else
				bp->Attributes = (BYTE)((*defAttrib) & 0xF0) | (curColor);

			defAttrib++;
			bp++;
			}
		else
		if (WithLayout(iCurrStyle) && ((*pCh) & 0x7F) == BOLD_CODE)
			{
			defAttrib++;
			iIsBold = !iIsBold;

			if (iIsUnderlined && iIsBold)
				curColor = boldAndUnderlineColor;
			else
			if (iIsBold)
				curColor = boldColor;
			else
			if (iIsUnderlined)
				curColor = underlineColor;
			else
				curColor = defColor;
			}
		else
		if (WithLayout(iCurrStyle) && ((*pCh) & 0x7F) == UNDERLINE_CODE)
			{
			defAttrib++;
			iIsUnderlined = !iIsUnderlined;

			if (iIsUnderlined && iIsBold)
				curColor = boldAndUnderlineColor;
			else
			if (iIsBold)
				curColor = boldColor;
			else
			if (iIsUnderlined)
				curColor = underlineColor;
			else
				curColor = defColor;
			}
		else
			{
			displayChar(*pCh, dispCol, *defAttrib++, useAttrib, curColor, bp, bufPos, iCurrStyle);
			}
		}

	// clear the rest to spaces
	if (iSelectedLine == line && iSelectedCount == -1)
		useAttrib = selAttrib;

	int eob = iColumn+di.screenwidth;
	for (; bufPos < eob; bufPos++, bp++)
		{
		bp->Char.AsciiChar = ' ';
		bp->Attributes = useAttrib;
		}

	PutTextBuf(1, atY, di.screenwidth, atY, &buf[iColumn]);
}

/**
 * 
 */
void Viewer::displayHexLine(int line, int atY)
{
	if (line >= hexLineCount())
		{
		MoveToXY(1, atY);
		setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());

		clrline();

#if defined(DISPLAY_END_OF_FILE)
		// TODO: change this to a configurable item
		if (line == hexLineCount())
			{
			setTextAttr(*iCurrStyle->iSelectedForeGndColor.GetItem(), *iCurrStyle->iSelectedBackGndColor.GetItem());

			tprintf("  END OF FILE  ");
			}
#endif

		return;
		}

	DisplayCell buf[LINE_BUF_MAX];
	DisplayCell* pBuf = buf;
	long	offset = line * BYTES_PER_LINE_HEX_MODE;
	PCHAR	pData = iData + (offset);
	PCHAR	pEnd = iData + iFileInfo->iFileSize;
	int	attrib;
	int selectedAttrib;
	int	i;

	MoveToXY(1, atY);
	setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());
	attrib = *iCurrStyle->iForeGndColor.GetItem() + (*iCurrStyle->iBackGndColor.GetItem()*16);
	selectedAttrib = *iCurrStyle->iSelectedForeGndColor.GetItem() + (*iCurrStyle->iSelectedBackGndColor.GetItem()*16);

	CString s = sformat("%08X �", pData - iData);
	CPCHAR  ps = s;

	while (*ps)
		{
		pBuf->Char.AsciiChar = *ps++;
		pBuf->Attributes = (unsigned char)attrib;
		pBuf++;
		}

	for (i=0; i < BYTES_PER_LINE_HEX_MODE; i++, pData++)
		{
		if ((i % 4) == 0)
			{
			pBuf->Char.AsciiChar = ' ';
			pBuf->Attributes = (unsigned char)attrib;
			pBuf++;
			}

		if (pData < pEnd)
			{
			if (offset >= iSelectedOffset && offset < iSelectedOffset+iSelectedCount)
				{
				pBuf->Char.AsciiChar = hexChar[(((*pData) & 0xF0) >> 4)];
				pBuf->Attributes = (unsigned char)selectedAttrib;
				pBuf++;
				pBuf->Char.AsciiChar = hexChar[((*pData) & 0x0F)];
				pBuf->Attributes = (unsigned char)selectedAttrib;
				pBuf++;
				}
			else
				{
				pBuf->Char.AsciiChar = hexChar[(((*pData) & 0xF0) >> 4)];
				pBuf->Attributes = (unsigned char)attrib;
				pBuf++;
				pBuf->Char.AsciiChar = hexChar[((*pData) & 0x0F)];
				pBuf->Attributes = (unsigned char)attrib;
				pBuf++;
				}

			offset++;
			}
		else
			{
			pBuf->Char.AsciiChar = ' ';
			pBuf->Attributes = (unsigned char)attrib;
			pBuf++;
			pBuf->Char.AsciiChar = ' ';
			pBuf->Attributes = (unsigned char)attrib;
			pBuf++;
			}

		pBuf->Char.AsciiChar = ' ';
		pBuf->Attributes = (unsigned char)attrib;
		pBuf++;
		}

	pBuf->Char.AsciiChar = '�';
	pBuf->Attributes = (unsigned char)attrib;
	pBuf++;
	pBuf->Char.AsciiChar = ' ';
	pBuf->Attributes = (unsigned char)attrib;
	pBuf++;

	offset = line * BYTES_PER_LINE_HEX_MODE;
	pData = iData + (offset);

	for (i=0; i < BYTES_PER_LINE_HEX_MODE; i++)
		{
		if (pData < pEnd)
			{
			pBuf->Char.AsciiChar = *pData++;
			}
		else
			{
			pBuf->Char.AsciiChar = ' ';
			}

		if (offset >= iSelectedOffset && offset < iSelectedOffset+iSelectedCount)
			pBuf->Attributes = (unsigned char)selectedAttrib;
		else
			pBuf->Attributes = (unsigned char)attrib;

		offset++;
		pBuf++;
		}

	pBuf->Char.AsciiChar = ' ';
	pBuf->Attributes = (unsigned char)attrib;
	pBuf++;

	PutTextBuf(1, atY, 80, atY, buf);
}

/**
 * 
 */
void Viewer::displayData()
{
	gCurrStyle = iCurrStyle;
	switch (getDisplayMode())
		{
		case DM_TEXT:
			{
			displayDataAsText();
			}
		break;

		case DM_HEX:
			{
			displayDataAsHex();
			}
		break;
		}

	displayArrows();
}

/**
 * 
 */
void Viewer::displayDataAsHex()
{
	WaitSync();

	int y = 2;

	for (int line=iHexTopLine; line <= hexLineCount() && y < di.screenheight; line++, y++)
		{
		displayHexLine(line, y);
		}

	setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());

	while (y < di.screenheight)
		{
		MoveToXY(1, y++);
		clrline();
		}

	ClearSync();
}

/**
 * 
 */
void Viewer::displayDataAsText()
{
	WaitSync();

	int y = 2;

	// iIsBold = iIsUnderlined = FALSE;

	for (int line=iTopLine; line <= iLineCount && y < di.screenheight; line++, y++)
		{
		displayTextLine(line, y);
		}

	setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());

	while (y < di.screenheight)
		{
		MoveToXY(1, y++);
		clrline();
		}

	ClearSync();
}

/**
 * 
 */
BOOL Viewer::fileHasChanged()
{
	Directory	d(iFileInfo->iDirName);

	d.fill(iFileInfo->iName);

	if (d.size() == 1)
		{
		if (d[0].time() != iFileInfo->iFileTime ||
			 d[0].size() != iFileInfo->iFileSize ||
			 d[0].date() != iFileInfo->iFileDate)
			 {
			 iFileInfo->iFileSize = d[0].size();
			 iFileInfo->iFileDate = d[0].date();
			 iFileInfo->iFileTime = d[0].time();

			 return iChanged = TRUE;
			 }
		}

	return iChanged;
}

/**
 * 
 */
void Viewer::dumpFile(BOOL bDump)
{
	if (iFileInfo->iFromStdin)
		{
		// don't dump stdin no way to reload it.
		return;
		}

	if (bDump && iData)
		{
		iLineCount = 0;

		if (iLinePtr)
			{
			delete iLinePtr;
			iLinePtr = 0;
			}

		if (iData)
			{
			delete iData;
			iData = 0;
			}
		}
}

/**
 * 
 */
BOOL Viewer::shouldWrap(char* p, int len, int width)
{
	if (*p == ' ')
		{
		char c = *++p & 0x7F;

		while (c && c != ' ' && c != '\r' && c != '\n'
			&& c != BOLD_CODE && c != UNDERLINE_CODE)
			{
			len++;
			c = *++p & 0x7F;
			}

		if (len >= width)
			return TRUE;
		}

	return FALSE;
}

/**
 * 
 */
void Viewer::scanData()
{
	// if scanData is called multiple times on a file
	// then we need to replace the characters where NULLS were placed
	replaceNULLS();

	if (iLinePtr)
		{
		delete iLinePtr;
		iLinePtr = 0;
		iLineCount = 0;
		}

	CString	msg = "Counting lines...";

	displayMsg((const char*)msg);

	messageTime = 0; // prevent this message from being cleared by timer display thread

	PCHAR p = iData;
	long  fileSize = iFileInfo->iFileSize;
	PCHAR ep = iData + fileSize;
	int   lineLen = 0;
   long	i;

	iBinary = FALSE;

	for (i=0; i < fileSize; i++)
		{
		char c = *p & 0x7F;

		if (*p == '\r' || *p == '\n' || *p == '\0' ||
			(getWordBreak() && shouldWrap(p, lineLen, di.screenwidth)))
			{
			if (*p == '\0' && !iBinary)
				{
				displayMsg("Binary file detected...");
				iBinary = TRUE;
				}

			if (*p == '\r')
				{
				p++;

				if (p < ep)
					{
					if (*p == '\n')
						{
						p++;
						i++;
						}
         		}
         	}
      	else
         	{
			// here only for UNIX files or \0 or word wrapping
            p++;
			}

		lineLen = 0;
        iLineCount++;
        }
	else
    	{
        *p++;
        }

	if (c != BOLD_CODE && c != UNDERLINE_CODE)
		lineLen++;
	}

	if (!iBinary && iData[fileSize-1] != '\r' && iData[fileSize-1] != '\n')
		{
		// this is a text file and the last char is
		// neither a CR or an LF so we have one more line
		// than we think we have. We have allocated space
		// for and added an additional NULL byte to the end
		// of the data to cope with this.

		iLineCount++;
		}

	if (i == fileSize)
		{
		if (*iCurrStyle->iSyntaxHighlightEnabled.GetItem())
			{
			msg = sformat("Analysing syntax against style %s...", (const char*)iCurrStyle->iStyleName);
			iCurrStyle->iWordBreak.SetItem(FALSE);
			}
		else
			msg = "Scanning for text with layout info...";

		displayMsg((const char*)msg);

		iLinePtr = new LinePtr[iLineCount];
		MFailIf(iLinePtr == 0);

		BOOL		IsUnderlined = FALSE, IsBold = FALSE;
		LinePtr*	lp = iLinePtr;
		int			line = 0;
		PCHAR 		p = iData;

		if (IsBold && IsUnderlined)
			lp->iLineStatus = LS_BOLD_UNDERLINED;
		else
		if (IsUnderlined)
			lp->iLineStatus = LS_UNDERLINED;
		else
		if (IsBold)
			lp->iLineStatus = LS_BOLD;

		lp->iText = p;
		lp++;
		line++;

		if (!iBinary && iData[fileSize-1] != '\r' && iData[fileSize-1] != '\n')
			fileSize++;

		int 	nonSpaceCount = 0;
		char	stringOpener = '\0';

		enum State
			{
			ST_NONE,
			ST_INSTRING,
			ST_INPREPROCESSOR,
			ST_INEOLCOMMENT,
			ST_INMULTICOMMENT
			} state = ST_NONE;

		lineLen = 0;

		for (long i=0; i < fileSize; i++, p++)
			{
			char c = *p & 0x7F;

			if (!iBinary)
				{
				if (*iCurrStyle->iSyntaxHighlightEnabled.GetItem())
					{
					switch (state)
						{
						case ST_INSTRING:
							{
							if (*p == stringOpener)
								state = ST_NONE;

							if (*p == *iCurrStyle->iEscape.GetItem() && *(p+1) == stringOpener)
								{
								i++;
								p++;
								}
							}
						break;

						case ST_INMULTICOMMENT:
							{
							if (IsEndComment(p, iCurrStyle))
								state = ST_NONE;
							}
						break;

						case ST_NONE:
							{
							if (IsString(p, iCurrStyle))
								{
								stringOpener = *p;
								state = ST_INSTRING;
								}
							else
							if (iCurrStyle->iOpenPreprocessor.GetItem() &&
								strncmp(*iCurrStyle->iOpenPreprocessor.GetItem(), p, strlen(*iCurrStyle->iOpenPreprocessor.GetItem())) == 0 &&
								nonSpaceCount == 0)
								{
								(lp-1)->iLineStatus = LS_INPREPROCESSOR;
								state = ST_INPREPROCESSOR;
								}
							else
							if (IsEolComment(p, iCurrStyle))
								{
								state = ST_INEOLCOMMENT;
								}
							else
							if (IsBeginComment(p, iCurrStyle))
								{
								state = ST_INMULTICOMMENT;
								}
							}
						break;

						case ST_INPREPROCESSOR:
							{
							if (IsEolComment(p, iCurrStyle))
								{
								state = ST_INEOLCOMMENT;
								}
							else
							if (IsBeginComment(p, iCurrStyle))
								{
								state = ST_INMULTICOMMENT;
								}
							}
						break;

						default: ;
						}
					}

				if (((*p) & 0x7F) == BOLD_CODE)
					{
					IsBold = !IsBold;
					}
				if (((*p) & 0x7F) == UNDERLINE_CODE)
					{
					IsUnderlined = !IsUnderlined;
					}
				}

			if (!isspace(*p))
				nonSpaceCount++;

			char	lastChar;

			if (*p == '\r' || *p == '\n' || *p == '\0' ||
				(getWordBreak() && shouldWrap(p, lineLen, di.screenwidth)))
				{
				lastChar = *(p-1);

				if (*p == '\r')
					{
					(lp-1)->iNullByte = p;
					(lp-1)->iOriginalChar = *p;
					*p = '\0';

					if (p+1 < ep)
						{
						if (*(p+1) == '\n')
							{
                     		p++;
							i++;
	               			}
						}
					}
				else
    	          	{
				   	// here only for UNIX files, embedded NULL's or word wrapping

				   	if (*p != '\0')
				   		{
						(lp-1)->iNullByte = p;
						(lp-1)->iOriginalChar = *p;
						*p = '\0';
						}
					}

				if (line < iLineCount)
					{
					nonSpaceCount = 0;
					if (p < ep)
						{
						if (state == ST_INPREPROCESSOR && *iCurrStyle->iLineContinuation.GetItem() == lastChar)
							lp->iLineStatus = LS_INPREPROCESSOR;
						else
						if (state == ST_INMULTICOMMENT)
							lp->iLineStatus = LS_INCOMMENT;
						else
						if (IsBold && IsUnderlined)
							lp->iLineStatus = LS_BOLD_UNDERLINED;
						else
						if (IsUnderlined)
							lp->iLineStatus = LS_UNDERLINED;
						else
						if (IsBold)
							lp->iLineStatus = LS_BOLD;

						lp->iText = p+1;
						lp++;
						line++;
						}
					else
						{
						if (IsBold && IsUnderlined)
							lp->iLineStatus = LS_BOLD_UNDERLINED;
						if (IsUnderlined)
							lp->iLineStatus = LS_UNDERLINED;
						else
						if (IsBold)
							lp->iLineStatus = LS_BOLD;

						lp->iText = "";
						lp++;
						line++;
						}

					if (state != ST_INMULTICOMMENT)
						state = ST_NONE;
					}

				lineLen = 0;
				}

			if (c != BOLD_CODE && c != UNDERLINE_CODE)
				lineLen++;
			}
		}

	if (getDisplayMode() == DM_HEX)
		{
		replaceNULLS();
		}

	clearMsg();
}

/**
 * 
 */
void Viewer::loadFile()
{
	if (fileHasChanged())
		{
		dumpFile(TRUE);
		}

	if (iData != 0)
		return;

	iChanged = FALSE;
	displayFileInfo();
	clearMsg();

	FILE* inFile = 0;
	long	fileSize = 0;
	CString	fileInError;

	if (!iFileInfo->iFromStdin)
		{
		inFile = fopen(fileInError=iFileInfo->iFileName, "rb");
		}
	else
		{
		if (*iCurrStyle->iFilterEnabled.GetItem() &&
			*iCurrStyle->iExternalFilterCmd.GetItem() != "")
			{
			CString filter = SubCString(*iCurrStyle->iExternalFilterCmd.GetItem(),
										1,
										iCurrStyle->iExternalFilterCmd.GetItem()->pos(' ')-1);

			fileInError=filter;

			if (searchpath(filter) || access(filter, 0) == 0 ||
				searchpath(filter+".exe") || access(filter+".exe", 0) == 0 ||
				searchpath(filter+".exe") || access(filter+".cmd", 0) == 0
				)
				{
				CString	cmd = sformat(*iCurrStyle->iExternalFilterCmd.GetItem(), (const char*)iFileInfo->iFileName);

				#if defined(__OS2__)
				MEnsure(DosSuspendThread(gTimeThread) == NO_ERROR);
				#elif defined(__WIN32__) && !defined(__DPMI32__)
				MEnsure(SuspendThread(gTimeThread) != 0xFFFFFFFF);
				#endif

				#if defined(__GNUG__)
				inFile = _popen(cmd, "r");
				#else
				inFile = _popen(cmd, "rb");
				#endif

				#if defined(__OS2__)
				MEnsure(DosResumeThread(gTimeThread) == NO_ERROR);
				#elif defined(__WIN32__) && !defined(__DPMI32__)
				MEnsure(ResumeThread(gTimeThread) != 0xFFFFFFFF);
				#endif
				}
			}
		else
			{
			inFile = stdin;
			setmode(fileno(stdin), O_BINARY);
			}
		}

	if (inFile)
		{
		displayMsg("Reading...");
		messageTime = 0; // prevent this message from being cleared by timer display thread

		if (!iFileInfo->iFromStdin)
			{
			fileSize = iFileInfo->iFileSize;
			iData = new char[fileSize+1];
			iData[fileSize] = '\0';	// add a NULL character to the end of the data
			MFailIf(iData == 0);
			fread(iData, fileSize, 1, inFile);
			fclose(inFile);
			}
		else
			{
			// we are reading from stdin.

			char	buf[1024];
			int	count;

			while ((count = fread(buf, 1, sizeof(buf), inFile)) > 0)
				{
				if (iData == 0)
					{
					iData = (PCHAR)malloc(count+1);
					memcpy(iData, buf, count);
					}
				else
					{
					iData = (PCHAR)realloc(iData, fileSize+count+1);
					memcpy(iData+fileSize, buf, count);
					}

				fileSize += count;
				}

			iFileInfo->iFileSize = fileSize;
			if (fileSize == 0)
				{
				iData = (PCHAR)malloc(1);
				}

			iData[fileSize] = '\0';	// add a NULL character to the end of the data

			if (*iCurrStyle->iFilterEnabled.GetItem() &&
				*iCurrStyle->iExternalFilterCmd.GetItem() != "")
				{
				if (_pclose(inFile) == -1)
					{
					displayErrMsg("Bad pipe");
					}

				_setcursortype(_NOCURSOR);

				// make the status line display function
				// clear the status line
				extern BOOL bFirstTime;
				bFirstTime = TRUE;

				#if defined(__OS2__)
				MEnsure(DosResumeThread(gTimeThread) == NO_ERROR);
				#elif defined(__WIN32__) && !defined(__DPMI32__)
				MEnsure(ResumeThread(gTimeThread) != 0xFFFFFFFF);
				#endif
				}
			else
				{
				fclose(inFile);
#if defined(__WIN32__) && !defined(__DPMI32__)
				if (freopen("CONIN$", "r+b", stdin))
					{
					SetStdHandle(STD_INPUT_HANDLE, (HANDLE)_get_osfhandle(fileno(stdin)));
					}
				else
#elif defined(__MSDOS__) || defined(__DPMI32__)
				if (freopen("con", "rb", stdin) == 0)
#elif defined(__OS2__)
				if (freopen("con", "rb", stdin) == 0)
#endif
					{
					fprintf(stderr, "OnScreen/2: Could not reopen the console after reading from redirected stdin\n");
					fprintf(stderr, "            Terminating...\n");
					}
				}

			displayFileInfo();
			clearMsg();
			}

		scanData();
		}
	else
		{
		displayErrMsg(fileInError + ": " + sys_errlist[errno]);
		}

	messageTime = gMessageClearTime; // allow message to be cleared by timer display thread
}

/**
 * 
 */
void Viewer::replaceNULLS()
{
	LinePtr*	lp = iLinePtr;

	for (int i=0; i < iLineCount; i++, lp++)
		{
		if (lp->iNullByte)
			*lp->iNullByte = lp->iOriginalChar;
		}
}

/**
 * 
 */
void Viewer::insertNULLS()
{
	LinePtr*	lp = iLinePtr;

	for (int i=0; i < iLineCount; i++, lp++)
		{
		if (lp->iNullByte)
			*lp->iNullByte = '\0';
		}
}

/**
 * 
 */
void Viewer::AdjustRowAndColumn(long& row, long newRow, long& col)
{
	char* p = iLinePtr[newRow].iText;
	int selectedPos=0, selectedCount = 0;

	if (DoExpandTabs(iCurrStyle))
		{
      long i;
		for (i=0; i < iSelectedPos; i++, p++)
			{
			if (*p == '\t')
				{
				selectedPos = nextTabStop(selectedPos, iCurrStyle);
				}
			else
				{
				selectedPos++;
				}
			}

		int endSelPos = selectedPos;

		for (; i < iSelectedPos + iSelectedCount; i++, p++)
			{
			if (*p == '\t')
				{
				endSelPos = nextTabStop(endSelPos, iCurrStyle);
				}
			else
				{
				endSelPos++;
				}
			}

		selectedCount = endSelPos - selectedPos;
		}
	else
		{
		selectedPos = iSelectedPos;
		selectedCount = iSelectedCount;
		}

	MTrace(("selectedPos = %li, selectedCount = %li", selectedPos, selectedCount));

	if (selectedPos >= col && selectedPos - col < di.screenwidth)
		{
		// start of selection is visible

		if ((selectedPos + selectedCount) - col >= di.screenwidth)
			{
			col += ((selectedCount+10) / 10) * 10;
			}
		}
	else
		{
		// start of selection ain't visible
		col += (selectedPos - (col + di.screenwidth)) + selectedCount + 10;
		col = (col / 10) * 10;
		}

	if (newRow >= row)
		{
		if (newRow > row + (di.screenheight-3))
			{
			row = newRow - (di.screenheight-5);

			if (row < 0)
				row = 0;
			}
		}
	else
		{
		// row ain't visible
		row = newRow;
		}

	if (col < 0)
		col = 0;
}

/**
 * 
 */
void Viewer::handleKeyInHexMode(int key)
{
	switch (key)
		{
		case 'G': // goto offset
		case 'g':
			{
			char s[11];

			strcpy(s, "");

			LineEdit	l(sizeof(s)-1,
							"Offset",
							s, &gotoOffsetHistory
							);

			if (l.Go() != 0x001B)
				{
				long offset = 0;

				sscanf(s, "%lx", &offset);

				if (offset >= 0 && offset < iFileInfo->iFileSize)
					{
					// reduce the offset to the nearest 16 bytes
					offset -= (offset % 16);

					iHexTopLine = offset / 16;

					displayFileInfo(TRUE);
					displayData();
					}
				}
			}
		break;

		case 0xFF47: // home
			{
			if (iHexTopLine > 0)
				{
				iHexTopLine = 0;
				displayFileInfo(TRUE);
				displayData();
				}
			else
				{
				displayErrMsg("Begining of file");
				}
			}
		break;

		case 0xFF4F: // end
			{
			long tmp = iHexTopLine;

			iHexTopLine = hexLineCount() - (di.screenheight-2);

			if (iHexTopLine < 0)
				iHexTopLine= 0;

			if (iHexTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				{
				displayErrMsg("End of file");
				}
			}
		break;

		case '\r':
		case ' ':
		case 0xFF51: // page down
			{
			long tmp = iHexTopLine;

			iHexTopLine += di.screenheight-2;

			if (iHexTopLine + (di.screenheight-1) >= hexLineCount())
				{
				iHexTopLine = hexLineCount() - (di.screenheight-2);

				if (iHexTopLine < 0)
					iHexTopLine = 0;
				}

			if (iHexTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				displayErrMsg("End of file");
			}
		break;

		case 0xFF49: // page up
			{
			long tmp = iHexTopLine;

			iHexTopLine -= di.screenheight-2;

			if (iHexTopLine < 0)
				{
				iHexTopLine = 0;
				}

			if (iHexTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				displayErrMsg("Begining of file");
			}
		break;

		case 0xFF50: // down line
			{
			if (iHexTopLine < hexLineCount()-1)
				{
				WaitSync();

				iHexTopLine++;
				MoveTextBuf(1, 3, di.screenwidth, di.screenheight-1, 1, 2);
				displayFileInfo(TRUE);
				displayHexLine(iHexTopLine + (di.screenheight-3), di.screenheight-1);

				ClearSync();
				}
			else
				{
				displayErrMsg("End of file");
				}
			}
		break;

		case 0xFF48: // up line
			{
			if (iHexTopLine > 0)
				{
				WaitSync();

				iHexTopLine--;
				MoveTextBuf(1, 2, di.screenwidth, di.screenheight-2, 1, 3);
				displayFileInfo(TRUE);
				displayHexLine(iHexTopLine, 2);

				ClearSync();
				}
			else
				{
				displayErrMsg("Begining of file");
				}
			}
		break;

		default:
#if defined(DEBUG)
			displayErrMsg(sformat("Invalid key %04X", key));
#else
			; // do nothing
#endif
		}

	WaitSync();

	displayArrows();

	ClearSync();
}

/**
 * 
 */
void Viewer::handleKeyInTextMode(int key)
{
	switch (key)
		{
#if defined(BETA)
		// performance tests
		case 0xFF03:	// CRTL+2
			{
			time_t	tStart, tFinish;
			int	pages = 0;
			time(&tStart);
			iTopLine = 0;

			displayFileInfo(TRUE);
			displayData();

			for (;;pages++)
				{
				long tmp = iTopLine;

				iTopLine += di.screenheight-2;

				if (iTopLine + (di.screenheight-1) >= iLineCount)
					{
					iTopLine = iLineCount - (di.screenheight-2);

					if (iTopLine < 0)
						iTopLine = 0;
					}

				if (iTopLine != tmp)
					{
					displayFileInfo(TRUE);
					displayData();
					}
				else
					break;
				}

			time(&tFinish);

			time_t elapsed = tFinish - tStart;
			textcolor(7);
			textbackground(0);
			clrscr();
			if (elapsed == 0)
				elapsed = 1;
			tprintf("display rate: %i PPS\r\n", pages / elapsed);
			exit(3);
			}
		break;
#endif

		case 0xFF22: // ALT+G, goto to bookmark
			{
			displayMsg("Select bookmark 0-9");
			messageTime = 0; // prevent this message from being cleared by timer display thread

			key = getKey();
			clearMsg();

			switch (key)
				{
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '0':
					{
					int bookMark = key - '0';

					if (iBookMark[bookMark].iLine == -1)
						{
						displayErrMsg(sformat("bookmark %i not set", bookMark));
						}
					else
						{
						iTopLine = iBookMark[bookMark].iLine;
						iColumn = iBookMark[bookMark].iCol;
						displayFileInfo(TRUE);
						displayData();
						}
					}
				break;

				default:
					displayErrMsg("not a bookmark");
				}
			}
		break;

		case 0xFF78:
		case 0xFF79:
		case 0xFF7A:
		case 0xFF7B:
		case 0xFF7C:
		case 0xFF7D:
		case 0xFF7E:
		case 0xFF7F:
		case 0xFF80:
		case 0xFF81:
			{
			int bookMark = (key - 0xFF78)+1;

			if (bookMark == 10)
				bookMark = 0;

			if (iBookMark[bookMark].iLine == -1)
				{
				displayMsg(sformat("Dropped bookmark %i", bookMark));
				iBookMark[bookMark].iLine = iTopLine;
				iBookMark[bookMark].iCol = iColumn;
				}
			else
				{
				if (iBookMark[bookMark].iLine == iTopLine)
					{
					displayMsg(sformat("Cleared bookmark %i", bookMark));
					iBookMark[bookMark].iLine = -1;
					iBookMark[bookMark].iCol = -1;
					}
				else
					{
					displayMsg(sformat("Reset bookmark %i", bookMark));
					iBookMark[bookMark].iLine = iTopLine;
					iBookMark[bookMark].iCol = iColumn;
					}
				}
			}
		break;

		case 0xFF2C:	// ALT+Z change top line format
			{
			iCurrStyle->iTopLineFormat.SetItem((*iCurrStyle->iTopLineFormat.GetItem())+1);

			if (*iCurrStyle->iTopLineFormat.GetItem() > 2)
				iCurrStyle->iTopLineFormat.SetItem(0);

			displayFileInfo();
			}
		break;

		case 'w':
		case 'W':
			{
			if (!iBinary && !*iCurrStyle->iSyntaxHighlightEnabled.GetItem())
				{
				// OK, it's not a binary file and it does not have a syntax
				// highlighting enabled.

				iCurrStyle->iWordBreak.SetItem(!*iCurrStyle->iWordBreak.GetItem());

				long	savedLineCount = iLineCount;

				scanData();

				if (iLineCount != savedLineCount)
					{
					// reset values that are affected by the changed line count
					iHexTopLine		= 0;
					iTopLine			= 0;
					iSelectedLine	= -1;
					iSelectedPos	= 0;
					iSelectedCount	= 0;
					iColumn			= 0;

					displayStdStatusLine();
					displayFileInfo(FALSE);
					displayData();
					}

				displayMsg(sformat("%s word break %s",
					(const char*)iCurrStyle->iStyleName,
					*iCurrStyle->iWordBreak.GetItem() ? "on":"off"));
				}
			}
		break;

		case 'G': // goto line number
		case 'g':
			{
			char s[11];

			strcpy(s, "");

			LineEdit	l(sizeof(s)-1,
							"Line",
							s, &gotoLineHistory
							);

			if (l.Go() != 0x001B)
				{
				long line = 0;

				sscanf(s, "%li", &line);
				line--;

				if (line >= 0 && line < iLineCount)
					{
					iSelectedLine = line;
					iSelectedPos = 0;
					iSelectedCount = -1;
					iTopLine = line;

					if (iTopLine < 0)
						iTopLine = 0;

					displayFileInfo(TRUE);
					displayData();
					}
				}
			}
		break;

		case 0xFF47: // home
			{
			if (iTopLine > 0)
				{
				iTopLine = 0;
				displayFileInfo(TRUE);
				displayData();
				}
			else
				{
				displayErrMsg("Begining of file");
				}
			}
		break;

		case 0xFF4F: // end
			{
			long tmp = iTopLine;

			iTopLine = iLineCount - (di.screenheight-2);

			if (iTopLine < 0)
				iTopLine = 0;

			if (iTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				{
				displayErrMsg("End of file");
				}
			}
		break;

		case '\r':
		case ' ':
		case 0xFF51: // page down
			{
			long tmp = iTopLine;

			iTopLine += di.screenheight-2;

			if (iTopLine + (di.screenheight-1) >= iLineCount)
				{
				iTopLine = iLineCount - (di.screenheight-2);

				if (iTopLine < 0)
					iTopLine = 0;
				}

			if (iTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				displayErrMsg("End of file");
			}
		break;

		case 0xFF49: // page up
			{
			long tmp = iTopLine;

			iTopLine -= di.screenheight-2;

			if (iTopLine < 0)
				{
				iTopLine = 0;
				}

			if (iTopLine != tmp)
				{
				displayFileInfo(TRUE);
				displayData();
				}
			else
				displayErrMsg("Begining of file");
			}
		break;

		case 0xFF50: // down line
			{
			if (iTopLine < iLineCount-1)
				{
				WaitSync();

				iTopLine++;
				MoveTextBuf(1, 3, di.screenwidth, di.screenheight-1, 1, 2);
				displayFileInfo(TRUE);
				displayTextLine(iTopLine + (di.screenheight-3), di.screenheight-1);

				ClearSync();
				}
			else
				{
				displayErrMsg("End of file");
				}
			}
		break;

		case 0xFF4D: // scroll left (right arrow key)
			{
			if (iColumn < ((LINE_BUF_MAX) - (di.screenwidth+10)))
				{
				iColumn+=10;
				displayFileInfo(TRUE);
				displayData();
				}
			}
		break;

		case 0xFF4B: // scroll right (left arrow key)
			{
			if (iColumn > 0)
				{
				iColumn-=10;
				displayFileInfo(TRUE);
				displayData();
				}
			}
		break;

		case 0xFF73: // move to column 0
			{
			if (iColumn > 0)
				{
				iColumn=0;
				displayFileInfo(TRUE);
				displayData();
				}
			}
		break;

		case 0xFF48: // up line
			{
			if (iTopLine > 0)
				{
				WaitSync();

				iTopLine--;
				MoveTextBuf(1, 2, di.screenwidth, di.screenheight-2, 1, 3);
				displayFileInfo(TRUE);
				displayTextLine(iTopLine, 2);

				ClearSync();
				}
			else
				{
				displayErrMsg("Begining of file");
				}
			}
		break;

		default:
#if defined(DEBUG)
			displayErrMsg(sformat("Invalid key %04X", key));
#else
			; // do nothing
#endif
		}

	WaitSync();

	displayArrows();

	ClearSync();
}

/**
 * 
 */
DisplayMode Viewer::getDisplayMode()
{
	return *iCurrStyle->iDisplayMode.GetItem();
}

/**
 * 
 */
BOOL Viewer::getWordBreak()
{
	return *iCurrStyle->iWordBreak.GetItem();
}

/**
 * 
 */
void Viewer::calcHexTopLine(long offset)
{
	if (offset < iHexTopLine * BYTES_PER_LINE_HEX_MODE)
		{
		while (offset < iHexTopLine * BYTES_PER_LINE_HEX_MODE)
			iHexTopLine--;
		}
	else
	if (offset > (iHexTopLine+(di.screenheight-2)) * BYTES_PER_LINE_HEX_MODE)
		{
		while (offset > (iHexTopLine+(di.screenheight-2)) * BYTES_PER_LINE_HEX_MODE)
			iHexTopLine++;
		}
}

/**
 * 
 */
void Viewer::calcNearestHexTopLine(long topLine)
{
	LinePtr*	lp = &iLinePtr[topLine];
	iHexTopLine = (lp->iText - iData) / BYTES_PER_LINE_HEX_MODE;

	if ((lp->iText - iData) % BYTES_PER_LINE_HEX_MODE)
		iHexTopLine++;
}

/**
 * 
 */
void Viewer::switchToHexMode()
{
	calcNearestHexTopLine(iTopLine);
	replaceNULLS();

	gDisplayMode = DM_HEX;

	iCurrStyle->iDisplayMode.SetItem(gDisplayMode);

	if (di.screenwidth > 80)
		{
		setTextAttr(*iCurrStyle->iForeGndColor.GetItem(), *iCurrStyle->iBackGndColor.GetItem());

		for (int y=2; y < di.screenheight; y++)
			{
			MoveToXY(1, y);
			clrline();
			}
		}
}

/**
 * 
 */
void Viewer::switchToTextMode()
{
	// scan to find the nearest line to where we are in hex mode
	PCHAR p = iData + (iHexTopLine * BYTES_PER_LINE_HEX_MODE);
	LinePtr*	lp;
	int		i;

	iTopLine = 0;
	insertNULLS();

	for (i=0, lp=&iLinePtr[0]; i < iLineCount; i++, lp++)
		{
		if (lp->iText > p)
			{
			iTopLine = i-1 > 0 ? i-1:0;
			break;
			}
		}

	gDisplayMode = DM_TEXT;

	iCurrStyle->iDisplayMode.SetItem(gDisplayMode);
}

/**
 * 
 */
void Viewer::handleKey(int key)
{
	switch (key)
		{
		case 0xFF1F:
		case 's': // search exact text (case sensitive)
		case 'S':
		case '/':
			{
			char s[63];
			int fromLine, toLine;
			int direction;

			strcpy(s, "");

			gSearchForward = key != 0xFF1F;

			LineEdit	l(sizeof(s)-1,
							gSearchForward ? "Scan" : "Scan",
							s, &searchTextHistory
							);

			if (l.Go() != 0x001B && *s != '\0')
				{
				if (getDisplayMode() == DM_HEX)
					insertNULLS();

				setupInfo.caseSensitive = TRUE;
				prevSrch = s;
				RegExp r(prevSrch);
				SearchExpression se(prevSrch);

				displayMsg("Searching...");
				messageTime = 0; // prevent this message from being cleared by timer display thread

				fromLine = iTopLine;
				toLine = iLineCount;
				direction = 1;

				if (!gSearchForward)
					{
					toLine = -1;
					direction = -1;
					}

				LinePtr*	lp;
				int			i;

				for (i=fromLine, lp=&iLinePtr[fromLine]; i != toLine; i+=direction, lp+=direction)
					{
					if (setupInfo.regexpSearch)
						{
						if (r == lp->iText)
							{
							break;
							}
						}
					else
						{
						const char* match;

						if ((match = strsrch(lp->iText, se)) != 0)
							{
							iSelectedPos = (match - lp->iText)+1;
							break;
							}
						}
					}

				if (getDisplayMode() == DM_HEX)
					replaceNULLS();

				if (i == iLineCount || i == -1)
					{
					clearSelected();
					displayErrMsg("Not found");
					}
				else
					{
					clearMsg();
					iSelectedLine  = i;

					if (setupInfo.regexpSearch)
						{
						iSelectedPos   = r.FirstMatchingChar() - iLinePtr[iSelectedLine].iText;
						iSelectedCount = r.MatchLen();
						}
					else
						{
						iSelectedPos--;
						iSelectedCount = prevSrch.length();
						}

					iSelectedOffset = (iLinePtr[iSelectedLine].iText-iData) + iSelectedPos;
					AdjustRowAndColumn(iTopLine, i, iColumn);
					calcHexTopLine(iSelectedOffset);

					displayFileInfo(TRUE);
					displayData();
					}
				}
			}
      break;

		case 0xFF21:
		case 'f': // search text (case in-sensitive)
		case 'F':
		case '\\':
		 {
		 char s[63];
			int fromLine, toLine;
			int direction;

			strcpy(s, "");

			gSearchForward = key != 0xFF21;

			LineEdit	l(sizeof(s)-1,
							gSearchForward ? "Find" : "Find",
							s, &searchTextHistory
							);

			if (l.Go() != 0x001B && *s != '\0')
				{
				if (getDisplayMode() == DM_HEX)
					insertNULLS();

				setupInfo.caseSensitive = FALSE;
				prevSrch = s;
				RegExp r(prevSrch, TRUE);
				SearchExpression se(prevSrch, TRUE);

				displayMsg("Searching...");
				messageTime = 0; // prevent this message from being cleared by timer display thread

				fromLine = iTopLine;
				toLine = iLineCount;
				direction = 1;

				if (!gSearchForward)
					{
					toLine = -1;
					direction = -1;
					}

				LinePtr*	lp;
				int			i;

				for (i=fromLine, lp=&iLinePtr[fromLine]; i != toLine; i+=direction, lp+=direction)
					{
					if (setupInfo.regexpSearch)
						{
						if (r == lp->iText)
							{
							break;
							}
						}
					else
						{
						const char* match;

						if ((match = strsrch(lp->iText, se)) != 0)
							{
							iSelectedPos = (match - lp->iText)+1;
							break;
							}
						}
					}

				if (getDisplayMode() == DM_HEX)
					replaceNULLS();

				if (i == iLineCount || i == -1)
					{
					clearSelected();
					displayErrMsg("Not found");
					}
				else
					{
					clearMsg();
					iSelectedLine  = i;

					if (setupInfo.regexpSearch)
						{
						iSelectedPos   = r.FirstMatchingChar() - iLinePtr[iSelectedLine].iText;
						iSelectedCount = r.MatchLen();
						}
					else
						{
						iSelectedPos--;
						iSelectedCount = prevSrch.length();
						}

					iSelectedOffset = (iLinePtr[iSelectedLine].iText-iData) + iSelectedPos;
					AdjustRowAndColumn(iTopLine, i, iColumn);
					calcHexTopLine(iSelectedOffset);

					displayFileInfo(TRUE);
					displayData();
					}
				}
			}
		break;

		case 0xFF1E: // ALT+A, search again backwards
		case 0x000C: // ^L
		case 'a': // search again forwards
		case 'A':
			{
			int i = iSelectedLine;
			int fromLine, toLine;
			int direction;

			if (key == 0xFF1E)
				{
				gSearchForward = FALSE;
				}
			else
				{
				gSearchForward = TRUE;
				}

			if (prevSrch == "")
				{
				displayErrMsg("No previous search string");
				break;
				}

			int startLine;

			RegExp r(prevSrch, !setupInfo.caseSensitive);
			SearchExpression se(prevSrch, !setupInfo.caseSensitive);

			displayMsg("Searching...");
			messageTime = 0; // prevent this message from being cleared by timer display thread

			if (getDisplayMode() == DM_HEX)
				insertNULLS();

			if (iSelectedLine != -1)
				{
				iSelectedPos++;

				if (setupInfo.regexpSearch)
					{
					if (r == iLinePtr[iSelectedLine].iText+iSelectedPos)
						{
						goto found;
						}
					}
				else
					{
					int iPrevSelectedPos = iSelectedPos;
					const char* match;

					if ((match = strsrch(iLinePtr[iSelectedLine].iText+iPrevSelectedPos, se)) != 0)
						{
						iSelectedPos = (match - iLinePtr[iSelectedLine].iText)+1;
						goto found;
						}
					}
				}

			if (gSearchForward)
				{
				startLine = iSelectedLine+1;
				}
			else
				{
				if (iSelectedLine == -1 || iSelectedLine == 0)
					startLine = iLineCount-1;
				else
					startLine = iSelectedLine-1;
				}

			fromLine = startLine, toLine = iLineCount;
			direction = 1;

			if (!gSearchForward)
				{
				toLine = -1;
				direction = -1;
				}

			LinePtr*	lp;

			for (i=fromLine, lp=&iLinePtr[fromLine]; i != toLine; i+=direction, lp+=direction)
				{
				if (setupInfo.regexpSearch)
					{
					if (r == lp->iText)
						{
						break;
						}
					}
				else
					{
					const char* match;

					if ((match = strsrch(lp->iText, se)) != 0)
						{
						iSelectedPos = (match - lp->iText)+1;
						break;
						}
					}
				}

found:
			if (getDisplayMode() == DM_HEX)
				replaceNULLS();

			if (i == iLineCount || i == -1)
				{
				clearSelected();
				displayErrMsg("Not found");
				}
			 else
				{
				clearMsg();
				iSelectedLine  = i;

				if (setupInfo.regexpSearch)
					{
					iSelectedPos   = r.FirstMatchingChar() - iLinePtr[iSelectedLine].iText;
					iSelectedCount = r.MatchLen();
					}
				else
					{
					iSelectedPos--;
					iSelectedCount = prevSrch.length();
					}

				iSelectedOffset = (iLinePtr[iSelectedLine].iText-iData) + iSelectedPos;
				AdjustRowAndColumn(iTopLine, i, iColumn);
				calcHexTopLine(iSelectedOffset);

				displayFileInfo(TRUE);
				displayData();
				}
			}
		break;

		case 0x0006:	// Ctrl+F, display full filename on status line
			{
			CString tmp = mergePath(iFileInfo->iDirName, iFileInfo->iName) +
							" � Style "+iCurrStyle->iStyleName;

			if (tmp.length() > 70)
				tmp = SubCString(tmp, 1, 70);

			displayMsg(tmp);
			}
		break;

		case 'D':
		case 'd': // disable/enable external filter command
			{
			if (*iCurrStyle->iExternalFilterCmd.GetItem() != "")
				{
				iCurrStyle->iFilterEnabled.SetItem(!*iCurrStyle->iFilterEnabled.GetItem());

				// needs to be false becase dumpFile() won't dump files
				// from stdin
				iFileInfo->iFromStdin = FALSE;

				// now dump all files using this style
				SListIterator<Viewer>	viewerIterator(App::GetViewedFiles());
				Viewer*						viewer;

				for (viewer = viewerIterator.GoHead();
						viewer;
						viewer = viewerIterator.GetNext())
					{
					if (viewer->iCurrStyle == iCurrStyle)
						{
						viewer->iFileInfo->iFromStdin = FALSE;
						viewer->dumpFile(TRUE);

						if (*viewer->iCurrStyle->iFilterEnabled.GetItem())
							viewer->iFileInfo->iFromStdin = TRUE;
						else
							viewer->iFileInfo->iFromStdin = FALSE;
						}
					}

				loadFile();
				displayStdStatusLine();
				displayFileInfo(FALSE);
				displayData();

				displayMsg(sformat("%s external filter %s",
					(const char*)gCurrStyle->iStyleName,
					*iCurrStyle->iFilterEnabled.GetItem() ? "enabled":"disabled"));
				}
			}
		break;

		case 0x002A:	// * refresh file
			{
			if (fileHasChanged() || iChanged)
				{
				dumpFile(TRUE);
				loadFile();
				displayStdStatusLine();
				displayFileInfo(FALSE);
				displayData();
				}
			}
		break;

		case 0xFF12:	// ALT+E invoke editor
			{
			if (iFileInfo->iFromStdin)
				{
				displayErrMsg("Can't edit <stdin>");
				break;
				}

			if (Edit(mergePath(iFileInfo->iDirName, iFileInfo->iName)))
				{
				loadFile();
				displayStdStatusLine();
				displayFileInfo(FALSE);
				displayData();
				}
			}
		break;

		case 'h':
		case 'H':
			{
			if (getDisplayMode() == DM_HEX)
				{
				switchToTextMode();
				}
			else
				{
				switchToHexMode();
				}

			displayFileInfo(TRUE);
			displayData();
			}
		break;

		default:
			{
			if (getDisplayMode() == DM_TEXT)
				{
				handleKeyInTextMode(key);
				}
			else
				{
				handleKeyInHexMode(key);
				}
			}
		}
}
