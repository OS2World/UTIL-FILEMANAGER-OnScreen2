/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#include <ostxt.hpp>
#pragma hdrstop

#pragma warn -use
static char rcsID[]="$Id: osstyle.cpp 2.50 1996/03/17 22:34:55 jallen Exp jallen $";
#pragma warn +use

SList<Style>	styles;
static int lineno;

static const char* OnString = "On";
static const char* OffString = "Off";

static const char* YesString = "Yes";
static const char* NoString = "No";

enum StyleSection
{
	SS_UNKNOWN=-1,
	SS_COMMENTCOLOR,
	SS_SYMBOLSCOLOR,
	SS_STRINGCOLOR,
	SS_RESERVEDCOLOR,
	SS_PREPROCESSORCOLOR,
	SS_NUMBERCOLOR,
	SS_IDENTCOLOR,
	SS_FOREGNDCOLOR,
	SS_BACKGNDCOLOR,
	SS_SELECTEDFOREGNDCOLOR,
	SS_SELECTEDBACKGNDCOLOR,
	SS_BOLDCOLOR,
	SS_UNDERLINECOLOR,
	SS_BOLDUNDERLINECOLOR,
	SS_EXPANDTABS,
	SS_HIGHBITFILTER,
	SS_TEXTWITHLAYOUT,
	SS_TABWIDTH,
	SS_DISPLAYMODE,
	SS_WORDBREAK,
	SS_TOPLINEFORMAT,
	SS_RESERVED,
	SS_STRINGS,
	SS_ESCAPE,
	SS_SYMBOLS,
	SS_OPENCOMMENT,
	SS_CLOSECOMMENT,
	SS_SINGLELINECOMMENT,
	SS_NUMBERPREFIX,
	SS_OPENPREPROCESSOR,
	SS_CLOSEPREPROCESSOR,
	SS_COMMENTCOLUMN,
	SS_LINECONTINUATION,
	SS_EXTERNALFILTER,
	SS_CASESENSITIVE,
	SS_CASECONVERT,
	SS_EDITOR,
	SS_LAST
} styleSec;

enum SettingSection
{
	SET_UNKNOWN=-1,
	SET_SOUND,
	SET_SEARCH,
	SET_STATUSTEXTFORECOLOR,
	SET_STATUSTEXTBACKCOLOR,
	SET_KEEPFILESLOADED,
	SET_DISKTEXTFORECOLOR,
	SET_DISKTEXTBACKCOLOR,
	SET_CURDISKTEXTFORECOLOR,
	SET_CURDISKTEXTBACKCOLOR ,
	SET_FILETEXTFORECOLOR,
	SET_FILETEXTBACKCOLOR,
	SET_CURFILETEXTFORECOLOR,
	SET_CURFILETEXTBACKCOLOR,
	SET_LAST
};

const char* styleSectionTable[] =
{
	"CommentColor",
	"SymbolsColor",
	"StringColor",
	"ReservedColor",
	"PreProcessorColor",
	"NumberColor",
	"IdentColor",
	"ForeGndColor",
	"BackGndColor",
	"SelectedForeGndColor",
	"SelectedBackGndColor",
	"BoldColor",
	"UnderlineColor",
	"BoldUnderlineColor",
	"ExpandTabs",
	"HighbitFilter",
	"TextWithLayout",
	"TabWidth",
	"DisplayMode",
	"WordBreak",
	"TopLineFormat",
	"Reserved",
	"Strings",
	"Escape",
	"Symbols",
	"OpenComment",
	"CloseComment",
	"SingleLineComment",
	"NumberPrefix",
	"OpenPreProcessor",
	"ClosePreProcessor",
	"CommentColumn",
	"LineContinuation",
	"ExternalFilter",
	"CaseSensitive",
	"CaseConvert",
	"Editor"
};

const char* settingsTable[] =
{
	"Sound",
	"Search",
	"StatusTextForeColor",
	"StatusTextBackColor",
	"KeepFilesLoaded",
	"DiskTextForeColor",
	"DiskTextBackColor",
	"CurDiskTextForeColor",
	"CurDiskTextBackColor",
	"FileTextForeColor",
	"FileTextBackColor",
	"CurFileTextForeColor",
	"CurFileTextBackColor"
};

const char* colorTable[WHITE+1] =
{
	"Black",
	"Blue",
	"Green",
	"Cyan",
	"Red",
	"Magenta",
	"Brown",
	"LightGray",
	"DarkGray",
	"LightBlue",
	"LightGreen",
	"LightCyan",
	"LightRed",
	"LightMagenta",
	"Yellow",
	"White",
};

/**
 * 
 */
bool IsOnOrYes(const char* s)
{
	if (strcmpi(s, OnString) == 0 ||
		strcmpi(s, YesString) == 0)
		return true;
	else
		return false;
}

/**
 * 
 */
bool IsOffOrNo(const char* s)
{
	if (strcmpi(s, OffString) == 0 ||
		strcmpi(s, NoString) == 0)
		return true;
	else
		return false;
}

/**
 * 
 */
SettingSection GetSetting(char* setting)
{
	for (int i=0; i < SET_LAST; i++)
		{
		if (strcmpi(setting, settingsTable[i]) == 0)
			return i;
		}

	return SET_UNKNOWN;
}

/**
 * 
 */
StyleSection GetSection(char *secname)
{
	for (int i=0; i < SS_LAST; i++)
		{
		if (strcmpi(secname, styleSectionTable[i]) == 0)
			return i;
		}

	return SS_UNKNOWN;
}

/**
 * 
 */
int GetColor(char* colorName)
{
	for (int i=0; i < WHITE+1; i++)
		{
		if (strcmpi(colorName, colorTable[i]) == 0)
			return i;
		}

	return -1;
}

/**
 * 
 */
char* getString(FILE* f)
{
	static char	buf[1024];

	if (fgets(buf, sizeof(buf), f))
		{
		lineno++;
		char*	p = buf;

		char* eol = 0;

		if ((eol = strrchr(p, '\n')) != 0)
			{
			*eol = '\0';
			}

		if ((eol = strrchr(p, '\r')) != 0)
			{
			*eol = '\0';
			}

		if (eol)
			{
			// remove trailing space
			eol--;

			while (isspace(*eol))
				eol--;

			*(eol+1) = '\0';
			}

		while (*p && isspace(*p))
			p++;

		if (*p == ';' || *p == '\0')
			return 0;

		if (*p)
			{
			char* c = p;

			while (*c)
				{
				if (*c == '\t')
					*c = ' ';

				c++;
				}

			return p;
			}
		else
			return 0;
		}

	return 0;
}

/**
 * 
 */
char* getSymbol(FILE* f, int restOfLine = 0)
{
	static char*	line = 0;
	static char buf[1024];
	char* sym = buf;

	if (restOfLine && (!line || *line == '\0'))
		{
		*sym = '\0';

		return buf;
		}

	if (!line || *line == '\0')
		{
		line = 0;
		while (!line && !feof(f))
			line = getString(f);

		if (feof(f))
			line = 0;
		}

	if (line)
		{
		while (*line && isspace(*line))
			line++;

		if (restOfLine)
			{
			while (*line)
				*sym++ = *line++;

			*sym = '\0';

			return buf;
			}

		if (isalpha(*line) || *line == '.')
			{
			*sym++ = *line++;

			while (*line && !strchr(" =<>(){}.", *line))
				*sym++ = *line++;

			*sym = '\0';

			return buf;
			}
		else
			{
			if (strchr("()*{}", *line))
				{
				*sym++ = *line++;
				*sym = '\0';

				return buf;
				}
			else
			if (*line == '=' && *(line+1) == '>')
				{
				*sym++ = *line++;
				*sym++ = *line++;
				*sym = '\0';

				return buf;
				}
			else
				{
				return 0;
				}
			}
		}

	return 0;
}

/**
 * 
 */
int cstrcmpi(const ReservedWord* r1, const ReservedWord* r2)
{
	return strcmpi(r1->iKeyword, r2->iKeyword);
}

/**
 * 
 */
void saveConfig(CString& configFile)
{
	CString	bakName;

	bakName = CString(SubCString(configFile, 1, configFile.posr('.')-1)) +
				CString(".bak");

	unlink(bakName);
	rename(configFile, bakName);

	FILE*	f = fopen(configFile, "wt");

	if (f)
		{
		fprintf(f, ";*[f*--------------------------------oOo-----------------------------------\n");
		fprintf(f, ";\n");
		fprintf(f, "; Saved at %s %s\n", (const char*)systemTime().longCString(), (const char*)systemDate().longCString());
		fprintf(f, ";\n");
		fprintf(f, "; You can set a variety of OnScreen/2 items in this file.\n");
		fprintf(f, "; * Colours for status lines\n");
		fprintf(f, "; * Colours for file manager & buffer list\n");
		fprintf(f, "; * Other global OnScreen/2 settings\n");
		fprintf(f, "; * Styles, including syntax highlighting\n");
		fprintf(f, "; * Key bindings (coming soon)\n");
		fprintf(f, ";\n");
		fprintf(f, "; In this file you can specify the styles which OnScreen/2 will apply\n");
		fprintf(f, "; to files with specific extensions.\n");
		fprintf(f, ";\n");
		fprintf(f, "; Files with either no extension or with an extension which is not\n");
		fprintf(f, "; listed here will use the Default style.\n");
		fprintf(f, ";\n");
		fprintf(f, "; Comments start with a ; and end at EOL. The ; must be the first\n");
		fprintf(f, "; non whitespace character on the line\n");
		fprintf(f, ";\n");
		fprintf(f, "; The syntax for the style heading is as follows\n");
		fprintf(f, "; Keyword StyleName File-Ext-List Inherited-Styles\n");
		fprintf(f, "; Style   Pascal    (.pas)        Base Default...\n");
		fprintf(f, ";\n");
		fprintf(f, ";*f]*--------------------------------oOo-----------------------------------\n\n");

		// save the settings

		fprintf(f, "Settings\n");
		fprintf(f, "{\n");
		fprintf(f, "\tSound\t\t\t\t\t=> %s\n", 			setupInfo.soundOn ? OnString:OffString);
		fprintf(f, "\tKeepFilesLoaded\t\t\t=> %s\n", 	setupInfo.keepFilesLoaded ? OnString:OffString);
		fprintf(f, "\tSearch\t\t\t\t\t=> %s\n", 		setupInfo.regexpSearch ? "Regexp":"Plain");
		fprintf(f, "\tStatusTextForeColor\t\t=> %s\n", 	colorTable[setupInfo.statusTextForeColor]);
		fprintf(f, "\tStatusTextBackColor\t\t=> %s\n", 	colorTable[setupInfo.statusTextBackColor]);
		fprintf(f, "\tDiskTextForeColor\t\t=> %s\n", 	colorTable[setupInfo.diskTextForeColor]);
		fprintf(f, "\tDiskTextBackColor\t\t=> %s\n", 	colorTable[setupInfo.diskTextBackColor]);
		fprintf(f, "\tCurDiskTextForeColor\t=> %s\n", 	colorTable[setupInfo.curDiskTextForeColor]);
		fprintf(f, "\tCurDiskTextBackColor\t=> %s\n", 	colorTable[setupInfo.curDiskTextBackColor]);
		fprintf(f, "\tFileTextForeColor\t\t=> %s\n", 	colorTable[setupInfo.fileTextForeColor]);
		fprintf(f, "\tFileTextBackColor\t\t=> %s\n", 	colorTable[setupInfo.fileTextBackColor]);
		fprintf(f, "\tCurFileTextForeColor\t=> %s\n", 	colorTable[setupInfo.curFileTextForeColor]);
		fprintf(f, "\tCurFileTextBackColor\t=> %s\n", 	colorTable[setupInfo.curFileTextBackColor]);
		fprintf(f, "}\n\n");

		// save the styles
		SListIterator<Style>	styleIterator(styles);
		Style*					style;

		for (style = styleIterator.GoHead(); style; style = styleIterator.GetNext())
			{
			fprintf(f, "Style %s (", (const char*)style->iStyleName);

			if (strcmpi(style->iStyleName, "Default") == 0)
				{
				fprintf(f, "*");
				}
			else
			if (style->iExtensions.GetUnresolved())
				{
				SetIterator<CString>	extIterator(*style->iExtensions.GetUnresolved());
				CString*				ext;

				for (ext = extIterator.GoHead(); ext;)
					{
					CString	s = *ext;
					ext = extIterator.GetNext();

					if (ext)
						fprintf(f, "%s ", (const char*)s);
					else
						fprintf(f, "%s", (const char*)s);
					}
				}

			if (style->iBaseStyles.Size() > 0)
				{
				fprintf(f, ") ");

				SetIterator<CString>	baseStyleIterator(style->iBaseStyles);
				CString*				baseStyle;

				for (baseStyle = baseStyleIterator.GoHead(); baseStyle;)
					{
					CString	s = *baseStyle;
					baseStyle = baseStyleIterator.GetNext();

					if (baseStyle)
						fprintf(f, "%s ", (const char*)s);
					else
						fprintf(f, "%s\n", (const char*)s);
					}
				}
			else
				{
				fprintf(f, ")\n");
				}

			fprintf(f, "{\n");

			if (style->iForeGndColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_FOREGNDCOLOR], colorTable[*style->iForeGndColor.GetUnresolved()]);
				}

			if (style->iBackGndColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_BACKGNDCOLOR], colorTable[*style->iBackGndColor.GetUnresolved()]);
				}

			if (style->iSelectedForeGndColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t=> %s\n", styleSectionTable[SS_SELECTEDFOREGNDCOLOR], colorTable[*style->iSelectedForeGndColor.GetUnresolved()]);
				}

			if (style->iSelectedBackGndColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t=> %s\n", styleSectionTable[SS_SELECTEDBACKGNDCOLOR], colorTable[*style->iSelectedBackGndColor.GetUnresolved()]);
				}

			if (style->iBoldColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_BOLDCOLOR], colorTable[*style->iBoldColor.GetUnresolved()]);
				}

			if (style->iUnderlineColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_UNDERLINECOLOR], colorTable[*style->iUnderlineColor.GetUnresolved()]);
				}

			if (style->iBoldUnderlineColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t=> %s\n", styleSectionTable[SS_BOLDUNDERLINECOLOR], colorTable[*style->iBoldUnderlineColor.GetUnresolved()]);
				}

			if (style->iExpandTabs.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_EXPANDTABS], *style->iExpandTabs.GetUnresolved() ? OnString:OffString);
				}

			if (style->iHighBitFilter.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_HIGHBITFILTER], *style->iHighBitFilter.GetUnresolved() ? OnString:OffString);
				}

			if (style->iTextWithLayout.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_TEXTWITHLAYOUT], *style->iTextWithLayout.GetUnresolved() ? OnString:OffString);
				}

			if (style->iTabWidth.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %i\n", styleSectionTable[SS_TABWIDTH], *style->iTabWidth.GetUnresolved());
				}

			if (style->iDisplayMode.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_DISPLAYMODE], *style->iDisplayMode.GetUnresolved() == DM_TEXT ? "Text":"Hex");
				}

			if (style->iExternalFilterCmd.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_EXTERNALFILTER], (const char*)*style->iExternalFilterCmd.GetUnresolved());
				}

			if (style->iEditor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t\t=> %s\n", styleSectionTable[SS_EDITOR], (const char*)*style->iEditor.GetUnresolved());
				}

			if (style->iTopLineFormat.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %i\n", styleSectionTable[SS_TOPLINEFORMAT], *style->iTopLineFormat.GetUnresolved());
				}

			if (style->iWordBreak.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_WORDBREAK], *style->iWordBreak.GetUnresolved() ? OnString:OffString);
				}

			// save non-inherited reserved words, if any
			if (style->iReserved.Size() > 0)
				{
				bool	foundFirst = true;

				for (int i=0; i < style->iReserved.Size(); i++)
					{
					if (!style->iReserved[i].iInherited)
						{
						if (foundFirst)
							{
							foundFirst = false;
							fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_RESERVED], (const char*)style->iReserved[i].iKeyword);
							}
						else
							{
							fprintf(f, "\t\t\t\t\t\t\t=> %s\n", (const char*)style->iReserved[i].iKeyword);
							}
						}
					}
				}

			if (style->iSymbolsColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_SYMBOLSCOLOR], colorTable[*style->iSymbolsColor.GetUnresolved()]);
				}

			if (style->iCommentColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_COMMENTCOLOR], colorTable[*style->iCommentColor.GetUnresolved()]);
				}

			if (style->iStringColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_STRINGCOLOR], colorTable[*style->iStringColor.GetUnresolved()]);
				}

			if (style->iReservedColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_RESERVEDCOLOR], colorTable[*style->iReservedColor.GetUnresolved()]);
				}

			if (style->iPreprocessorColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t=> %s\n", styleSectionTable[SS_PREPROCESSORCOLOR], colorTable[*style->iPreprocessorColor.GetUnresolved()]);
				}

			if (style->iNumberColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_NUMBERCOLOR], colorTable[*style->iNumberColor.GetUnresolved()]);
				}

			if (style->iIdentColor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_IDENTCOLOR], colorTable[*style->iIdentColor.GetUnresolved()]);
				}

			if (style->iSymbols.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t\t=> %s\n", styleSectionTable[SS_SYMBOLS], (const char*)*style->iSymbols.GetUnresolved());
				}

			if (style->iOpenComment.GetUnresolved())
				{
				bool	foundFirst = true;
				SetIterator<CString>	openCommentIterator(*style->iOpenComment.GetUnresolved());
				CString*				openComment;

				for (openComment = openCommentIterator.GoHead(); openComment;)
					{
					CString	s = *openComment;
					openComment = openCommentIterator.GetNext();

					if (foundFirst)
						{
						foundFirst = false;
						fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_OPENCOMMENT], (const char*)s);
						}
					else
						{
						fprintf(f, "\t\t\t\t\t\t\t=> %s\n", (const char*)s);
						}
					}
				}

			if (style->iCloseComment.GetUnresolved())
				{
				bool	foundFirst = true;
				SetIterator<CString>	closeCommentIterator(*style->iCloseComment.GetUnresolved());
				CString*				closeComment;

				for (closeComment = closeCommentIterator.GoHead(); closeComment;)
					{
					CString	s = *closeComment;
					closeComment = closeCommentIterator.GetNext();

					if (foundFirst)
						{
						foundFirst = false;
						fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_CLOSECOMMENT], (const char*)s);
						}
					else
						{
						fprintf(f, "\t\t\t\t\t\t\t=> %s\n", (const char*)s);
						}
					}
				}

			if (style->iEolComment.GetUnresolved())
				{
				bool	foundFirst = true;
				SetIterator<CString>	eolCommentIterator(*style->iEolComment.GetUnresolved());
				CString*				eolComment;

				for (eolComment = eolCommentIterator.GoHead(); eolComment;)
					{
					CString	s = *eolComment;
					eolComment = eolCommentIterator.GetNext();

					if (foundFirst)
						{
						foundFirst = false;
						fprintf(f, "\t%s\t\t=> %s\n", styleSectionTable[SS_SINGLELINECOMMENT], (const char*)s);
						}
					else
						{
						fprintf(f, "\t\t\t\t\t\t\t=> %s\n", (const char*)s);
						}
					}
				}

			if (style->iString.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t\t=> %s\n", styleSectionTable[SS_STRINGS], (const char*)*style->iString.GetUnresolved());
				}

			if (style->iEscape.GetUnresolved() && *style->iEscape.GetUnresolved() != '\0')
				{
				fprintf(f, "\t%s\t\t\t\t\t=> %c\n", styleSectionTable[SS_ESCAPE], *style->iEscape.GetUnresolved());
				}

			if (style->iNumericPrefix.GetUnresolved())
				{
				bool	foundFirst = true;
				SetIterator<CString>	numericPrefixIterator(*style->iNumericPrefix.GetUnresolved());
				CString*				numericPrefix;

				for (numericPrefix = numericPrefixIterator.GoHead(); numericPrefix;)
					{
					CString	s = *numericPrefix;
					numericPrefix = numericPrefixIterator.GetNext();

					if (foundFirst)
						{
						foundFirst = false;
						fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_NUMBERPREFIX], (const char*)s);
						}
					else
						{
						fprintf(f, "\t\t\t\t\t\t\t=> %s\n", (const char*)s);
						}
					}
				}

			if (style->iCaseSensitive.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %s\n", styleSectionTable[SS_CASESENSITIVE], *style->iCaseSensitive.GetUnresolved() ? YesString:NoString);
				}

			if (style->iCaseConvert.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t\t=> %s\n", styleSectionTable[SS_CASECONVERT], *style->iCaseConvert.GetUnresolved() ? YesString:NoString);
				}

			if (style->iOpenPreprocessor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t=> %s\n", styleSectionTable[SS_OPENPREPROCESSOR], (const char*)*style->iOpenPreprocessor.GetUnresolved());
				}

			if (style->iClosePreprocessor.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t=> %s\n", styleSectionTable[SS_CLOSEPREPROCESSOR], (const char*)*style->iClosePreprocessor.GetUnresolved());
				}

			if (style->iCommentColumn.GetUnresolved())
				{
				fprintf(f, "\t%s\t\t\t=> %i\n", styleSectionTable[SS_COMMENTCOLUMN], *style->iCommentColumn.GetUnresolved());
				}

			if (style->iLineContinuation.GetUnresolved() && *style->iLineContinuation.GetUnresolved() != '\0')
				{
				fprintf(f, "\t%s\t\t=> %c\n", styleSectionTable[SS_LINECONTINUATION], *style->iLineContinuation.GetUnresolved());
				}

			fprintf(f, "}\n\n");
			}

		fprintf(f, ";------------------------------------oOo-----------------------------------\n");
		fclose(f);
		}
}

/**
 * 
 */
void loadConfig(CString& configFile)
{
	FILE*	f = fopen(configFile, "rt");

	if (f)
		{
		lineno = 0;

		while (!feof(f))
			{
			char* p = getSymbol(f);

			if (p)
				{
				if (strcmpi(p, "Style") == 0)
					{
					char* styleName = getSymbol(f);
					Style* newStyle = 0;

					if (strcmpi(styleName, "Default") != 0)
						{
						SListIterator<Style>	styleIterator(styles);
						Style*					style;

						for (style = styleIterator.GoHead(); style; style = styleIterator.GetNext())
							{
							if (strcmpi(style->iStyleName, styleName) == 0)
								{
								newStyle = style;
								break;
								}
							}

						if (!newStyle)
							{
							newStyle = new Style(styleName);
							styles.Add(newStyle);
							}
						}
					else
						{
						newStyle = gDefaultStyle;
						}

					styleSec = SS_UNKNOWN;

					if (strcmp(getSymbol(f), "(") != 0)
						{
						cprintf("%s(%i): '(' expected\n", (const char*)configFile, lineno);
						exit(3);
						}

					char* ext;

					do
						{
						ext = getSymbol(f);

						if (*ext != '.' && *ext != ')' && *ext != '*')
							{
							cprintf("%s(%i): file extension expected\n", (const char*)configFile, lineno);
							exit(3);
							}

						if (*ext == '.' || *ext == '*')
							{
							newStyle->AddExtensions(CString(ext));
							}
						}
					while (*ext != ')');

					char* baseStyles = getSymbol(f);

					if (strcmp(baseStyles, "{") != 0)
						{
						// we have base styles

						while (strcmp(baseStyles, "{") != 0)
							{
							Style*	sty;
							SListIterator<Style> styleIterator(styles);

							for (sty = styleIterator.GoHead();
								sty;
								sty = styleIterator.GetNext())
								{
								if (strcmpi(sty->iStyleName, baseStyles) == 0)
									newStyle->AddBaseStyle(sty);
								}

							baseStyles = getSymbol(f);
							}
						}

					char* sym = 0;

					do
						{
						sym = getSymbol(f);

						if (strcmp(sym, "=>") == 0)
							{
							sym = getSymbol(f, 1);

							switch (styleSec)
								{
								case SS_SYMBOLSCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iSymbolsColor.SetItem(BYTE(color));
									}
								break;
								case SS_COMMENTCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iCommentColor.SetItem(BYTE(color));
									}
								break;
								case SS_STRINGCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iStringColor.SetItem(BYTE(color));
									}
								break;
								case SS_RESERVEDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iReservedColor.SetItem(BYTE(color));
									}
								break;
								case SS_PREPROCESSORCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iPreprocessorColor.SetItem(BYTE(color));
									}
								break;
								case SS_NUMBERCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iNumberColor.SetItem(BYTE(color));
									}
								break;
								case SS_IDENTCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iIdentColor.SetItem(BYTE(color));
									}
								break;
								case SS_FOREGNDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iForeGndColor.SetItem(BYTE(color));
									}
								break;
								case SS_BACKGNDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iBackGndColor.SetItem(BYTE(color));
									}
								break;
								case SS_SELECTEDFOREGNDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iSelectedForeGndColor.SetItem(BYTE(color));
									}
								break;
								case SS_SELECTEDBACKGNDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iSelectedBackGndColor.SetItem(BYTE(color));
									}
								break;
								case SS_BOLDCOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iBoldColor.SetItem(BYTE(color));
									}
								break;
								case SS_UNDERLINECOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iUnderlineColor.SetItem(BYTE(color));
									}
								break;
								case SS_BOLDUNDERLINECOLOR:
									{
									int color = GetColor(sym);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									newStyle->iBoldUnderlineColor.SetItem(BYTE(color));
									}
								break;
								case SS_EXPANDTABS:
									{
									if (IsOnOrYes(sym))
										newStyle->iExpandTabs.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iExpandTabs.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): Tabs must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_HIGHBITFILTER:
									{
									if (IsOnOrYes(sym))
										newStyle->iHighBitFilter.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iHighBitFilter.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): Highbit filter must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_TEXTWITHLAYOUT:
									{
									if (IsOnOrYes(sym) || strcmp(sym, "1") == 0)
										newStyle->iTextWithLayout.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iTextWithLayout.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): TextWithLayout must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_TABWIDTH:
									{
									newStyle->iTabWidth.SetItem(atoi(sym));

									if (*newStyle->iTabWidth.GetItem() < 1 ||
										*newStyle->iTabWidth.GetItem() > 20)
										{
										cprintf("%s(%i): TabWidth must be between 1 and 20\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_DISPLAYMODE:
									{
									newStyle->iDisplayMode.SetItem(DM_TEXT);

									if (strcmpi(sym, "TEXT") == 0)
										newStyle->iDisplayMode.SetItem(DM_TEXT);
									else
									if (strcmpi(sym, "HEX") == 0)
										newStyle->iDisplayMode.SetItem(DM_HEX);
									else
										{
										cprintf("%s(%i): DisplayMode must be set to either TEXT or HEX\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_WORDBREAK:
									{
									if (IsOnOrYes(sym) || strcmp(sym, "1") == 0)
										newStyle->iWordBreak.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iWordBreak.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): WorkBreak must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_TOPLINEFORMAT:
									{
									newStyle->iTopLineFormat.SetItem(atoi(sym));

									if (*newStyle->iTopLineFormat.GetItem() < 0 ||
										*newStyle->iTopLineFormat.GetItem() > 2)
										{
										cprintf("%s(%i): TopLineFormat must be between 0, 1 or 2\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_RESERVED:
									{
									if (*sym)
										{
										newStyle->AddReservedWord(CString(sym));
										}
									}
								break;
								case SS_OPENCOMMENT:
									{
									if (*sym)
										newStyle->AddOpenComment(CString(sym));
									}
								break;
								case SS_STRINGS:
									{
									if (*sym)
										{
										newStyle->iString.SetItem(sym);
										}
									}
								break;
								case SS_ESCAPE:
									{
									if (*sym)
										newStyle->iEscape.SetItem(*sym);
									}
								break;
								case SS_SYMBOLS:
									{
									if (*sym)
										newStyle->iSymbols.SetItem(sym);
									}
								break;
								case SS_CLOSECOMMENT:
									{
									if (*sym)
										newStyle->AddCloseComment(CString(sym));
									}
								break;
								case SS_SINGLELINECOMMENT:
									{
									if (*sym)
										newStyle->AddEolComment(CString(sym));
									}
								break;
								case SS_NUMBERPREFIX:
									{
									if (*sym)
										newStyle->AddNumericPrefix(CString(sym));
									}
								break;
								case SS_OPENPREPROCESSOR:
									{
									if (*sym)
										newStyle->iOpenPreprocessor.SetItem(sym);
									}
								break;
								case SS_CLOSEPREPROCESSOR:
									{
									if (*sym)
										newStyle->iClosePreprocessor.SetItem(sym);
									}
								break;
								case SS_COMMENTCOLUMN:
									{
									if (*sym)
										newStyle->iCommentColumn.SetItem(atoi(sym));
									}
								break;
								case SS_LINECONTINUATION:
									{
									if (*sym)
										newStyle->iLineContinuation.SetItem(*sym);
									}
								break;
								case SS_CASESENSITIVE:
									{
									if (IsOnOrYes(sym))
										newStyle->iCaseSensitive.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iCaseSensitive.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): CaseSensitive must be set to either YES or NO\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_CASECONVERT:
									{
									if (IsOnOrYes(sym))
										newStyle->iCaseConvert.SetItem(TRUE);
									else
									if (IsOffOrNo(sym))
										newStyle->iCaseConvert.SetItem(FALSE);
									else
										{
										cprintf("%s(%i): CaseConvert must be set to either YES or NO\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SS_EXTERNALFILTER:
									{
									if (*sym)
										newStyle->iExternalFilterCmd.SetItem(sym);
									else
										newStyle->iExternalFilterCmd.SetItem("");
									}
								break;
								case SS_EDITOR:
									{
									if (*sym)
										newStyle->iEditor.SetItem(sym);
									else
										newStyle->iEditor.SetItem("");
									}
								break;
								case SS_UNKNOWN:
									{
									// just ignore => found before a section
									// has been defined
									}
								break;
								}

							sym = "";
							}
						else
							{
							styleSec = GetSection(sym);

							if (styleSec == SS_UNKNOWN && strcmp(sym, "}") != 0)
								{
								cprintf("%s(%i): undefined style element '%s'\n", (const char*)configFile, lineno, sym);
								exit(3);
								}
							}
						}
					while (strcmp(sym, "}") != 0);

					if (newStyle->iReserved.Size() > 0)
						{
						// switch syntax highlighting on for styles
						// with reserved words.
						newStyle->iSyntaxHighlightEnabled.SetItem(TRUE);

						if (!*newStyle->iCaseSensitive.GetItem())
							{
							newStyle->iReserved.sort(cstrcmpi);
							}
						else
							{
							newStyle->iReserved.sort();
							}
						}
					}
				else
				if (strcmpi(p, "Settings") == 0)
					{
					char* setting = getSymbol(f);
					char* sym;
					char* value;

					if (strcmp(setting, "{") != 0)
						{
						cprintf("%s(%i): '{' expected\n", (const char*)configFile, lineno);
						exit(3);
						}

					do
						{
						setting = getSymbol(f);

						if (strcmp(setting, "}") != 0)
							{
							SettingSection section = GetSetting(setting);
							sym = getSymbol(f);

							if (strcmp(sym, "=>") != 0)
								{
								cprintf("%s(%i): '=>' expected\n", (const char*)configFile, lineno);
								exit(3);
								}

							value = getSymbol(f);

							switch (section)
								{
								case SET_SOUND:
									{
									if (IsOffOrNo(value))
										setupInfo.soundOn = FALSE;
									else
									if (IsOnOrYes(value))
										setupInfo.soundOn = TRUE;
									else
										{
										cprintf("%s(%i): Sound must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SET_SEARCH:
									{
									if (strcmpi(value, "regexp") == 0)
										setupInfo.regexpSearch = TRUE;
									else
									if (strcmpi(value, "plain") == 0)
										setupInfo.regexpSearch = FALSE;
									else
										{
										cprintf("%s(%i): Search must be set to either REGEXP or PLAIN\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SET_KEEPFILESLOADED:
									{
									if (IsOffOrNo(value))
										setupInfo.keepFilesLoaded = FALSE;
									else
									if (IsOnOrYes(value))
										setupInfo.keepFilesLoaded = TRUE;
									else
										{
										cprintf("%s(%i): KeepFilesLoaded must be set to either ON or OFF\n", (const char*)configFile, lineno);
										exit(3);
										}
									}
								break;
								case SET_STATUSTEXTFORECOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.statusTextForeColor = color;
									}
								break;
								case SET_STATUSTEXTBACKCOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.statusTextBackColor = color;
									}
								break;
								case SET_DISKTEXTFORECOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.diskTextForeColor = color;
									}
								break;
								case SET_DISKTEXTBACKCOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.diskTextBackColor = color;
									}
								break;
								case SET_CURDISKTEXTFORECOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.curDiskTextForeColor = color;
									}
								break;
								case SET_CURDISKTEXTBACKCOLOR :
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.curDiskTextBackColor = color;
									}
								break;
								case SET_FILETEXTFORECOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.fileTextForeColor = color;
									}
								break;
								case SET_FILETEXTBACKCOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.fileTextBackColor = color;
									}
								break;
								case SET_CURFILETEXTFORECOLOR :
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.curFileTextForeColor = color;
									}
								break;
								case SET_CURFILETEXTBACKCOLOR:
									{
									int color = GetColor(value);

									if (color == -1)
										{
										cprintf("%s(%i): Invalid color '%s'\n", (const char*)configFile, lineno, sym);
										exit(3);
										}

									setupInfo.curFileTextBackColor = color;
									}
								break;
								default:
									cprintf("%s(%i): Unknown setting '%s'\n", (const char*)configFile, lineno, setting);
									exit(3);
								}
							}
						}
					while (strcmp(setting, "}") != 0);
					}
				else
					{
					cprintf("%s(%i): 'Style' expected\n", (const char*)configFile, lineno);
					exit(3);
					}
				}
			}

		fclose(f);
		}
}

/**
 * 
 */
Style::Style(CString styleName):
	iStyleName(styleName)
{
}

/**
 * 
 */
Style::~Style()
{
}

/**
 * 
 */
void Style::AddReservedWord(CString& reservedWord, BOOL inherited)
{
	for (int i=0; i < iReserved.Size(); i++)
		{
		if (strcmpi(iReserved[i].iKeyword, reservedWord) == 0)
			{
			iReserved[i].iKeyword = reservedWord;
			return;
			}
		}

	iReserved.SetDynamic(TRUE);
	iReserved[iReserved.Size()] = ReservedWord(inherited, reservedWord);
	iReserved.SetDynamic(FALSE);
}

/**
 * 
 */
void Style::AddOpenComment(CString& openComment)
{
	Set<CString>*	list = iOpenComment.GetUnresolved();

	if (!list)
		{
		iOpenComment.SetPtr(list = new Set<CString>);
		}

	list->Add(new CString(openComment));
}

/**
 * 
 */
void Style::AddCloseComment(CString& closeComment)
{
	Set<CString>*	list = iCloseComment.GetUnresolved();

	if (!list)
		{
		iCloseComment.SetPtr(list = new Set<CString>);
		}

	list->Add(new CString(closeComment));
}

/**
 * 
 */
void Style::AddEolComment(CString& eolComment)
{
	Set<CString>*	list = iEolComment.GetUnresolved();

	if (!list)
		{
		iEolComment.SetPtr(list = new Set<CString>);
		}

	list->Add(new CString(eolComment));
}

/**
 * 
 */
void Style::AddNumericPrefix(CString& numericPrefix)
{
	Set<CString>*	list = iNumericPrefix.GetUnresolved();

	if (!list)
		{
		iNumericPrefix.SetPtr(list = new Set<CString>);
		}

	list->Add(new CString(numericPrefix));
}

/**
 * 
 */
void Style::AddExtensions(CString& ext)
{
	Set<CString>*	list = iExtensions.GetUnresolved();

	if (!list)
		{
		iExtensions.SetPtr(list = new Set<CString>);
		}

	list->Add(new CString(ext));
}

/**
 * 
 */
void Style::AddBaseStyle(Style* style)
{
	iBaseStyles.Add(new CString(style->iStyleName));

	iForeGndColor.AddBaseItem(&style->iForeGndColor);
	iBackGndColor.AddBaseItem(&style->iBackGndColor);
	iSelectedForeGndColor.AddBaseItem(&style->iSelectedForeGndColor);
	iSelectedBackGndColor.AddBaseItem(&style->iSelectedBackGndColor);
	iBoldColor.AddBaseItem(&style->iBoldColor);
	iUnderlineColor.AddBaseItem(&style->iUnderlineColor);
	iBoldUnderlineColor.AddBaseItem(&style->iBoldUnderlineColor);
	iExpandTabs.AddBaseItem(&style->iExpandTabs);
	iHighBitFilter.AddBaseItem(&style->iHighBitFilter);
	iTextWithLayout.AddBaseItem(&style->iTextWithLayout);
	iTabWidth.AddBaseItem(&style->iTabWidth);
	iDisplayMode.AddBaseItem(&style->iDisplayMode);
	iExternalFilterCmd.AddBaseItem(&style->iExternalFilterCmd);
	iFilterEnabled.AddBaseItem(&style->iFilterEnabled);
	iEditor.AddBaseItem(&style->iEditor);
	iTopLineFormat.AddBaseItem(&style->iTopLineFormat);
	iWordBreak.AddBaseItem(&style->iWordBreak);
	iSyntaxHighlightEnabled.AddBaseItem(&style->iSyntaxHighlightEnabled);
	iSymbolsColor.AddBaseItem(&style->iSymbolsColor);
	iCommentColor.AddBaseItem(&style->iCommentColor);
	iStringColor.AddBaseItem(&style->iStringColor);
	iReservedColor.AddBaseItem(&style->iReservedColor);
	iPreprocessorColor.AddBaseItem(&style->iPreprocessorColor);
	iNumberColor.AddBaseItem(&style->iNumberColor);
	iIdentColor.AddBaseItem(&style->iIdentColor);
	iSymbols.AddBaseItem(&style->iSymbols);
	iString.AddBaseItem(&style->iString);
	iEscape.AddBaseItem(&style->iEscape);
	iNumericPrefix.AddBaseItem(&style->iNumericPrefix);
	iCaseSensitive.AddBaseItem(&style->iCaseSensitive);
	iCaseConvert.AddBaseItem(&style->iCaseConvert);
	iCommentColumn.AddBaseItem(&style->iCommentColumn);
	iLineContinuation.AddBaseItem(&style->iLineContinuation);

	// extensions don't inherit
	// iExtensions.AddBaseItem(&style->iExtensions);

	iOpenComment.AddBaseItem(&style->iOpenComment);
	iCloseComment.AddBaseItem(&style->iCloseComment);
	iEolComment.AddBaseItem(&style->iEolComment);
	iOpenPreprocessor.AddBaseItem(&style->iOpenPreprocessor);
	iClosePreprocessor.AddBaseItem(&style->iClosePreprocessor);

	// Reserved words have to use a special technique to keep recognition fast.
	// All reserved words are directly copied into the inheriting styles
	// array but inherited ones are marked as such.

	if (style->iReserved.Size() > 0)
		{
		for (int i=0; i < style->iReserved.Size(); i++)
			AddReservedWord(style->iReserved[i].iKeyword, TRUE);
		}
}
