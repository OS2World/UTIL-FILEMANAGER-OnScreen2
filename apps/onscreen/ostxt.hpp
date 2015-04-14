/**
 * $Id$
 * 
 * Copyright 1993-2006, John J. Allen.
 * Released under the GNU GPL v2 or later.
 */

#if !defined(OSTXT_HPP)
#define OSTXT_HPP

#include <os.hpp>
#if defined(__GO32__)
	#include <go32.h>
	#define _popen popen
	#define _pclose pclose
#endif

#include <os.hpp>

/**
 * Alt+key values
 */
#define VKALT_A	0xFF1E
#define VKALT_B	0xFF30
#define VKALT_C	0xFF2E
#define VKALT_D	0xFF20
#define VKALT_E	0xFF12
#define VKALT_F	0xFF21
#define VKALT_G	0xFF22
#define VKALT_H	0xFF23
#define VKALT_I	0xFF17
#define VKALT_J	0xFF24
#define VKALT_K	0xFF25
#define VKALT_L	0xFF26
#define VKALT_M	0xFF32
#define VKALT_N	0xFF31
#define VKALT_O	0xFF18
#define VKALT_P	0xFF19
#define VKALT_Q	0xFF10
#define VKALT_R	0xFF13
#define VKALT_S	0xFF1F
#define VKALT_T	0xFF14
#define VKALT_U	0xFF16
#define VKALT_V	0xFF2F
#define VKALT_W	0xFF11
#define VKALT_X	0xFF2D
#define VKALT_Y	0xFF15
#define VKALT_Z	0xFF2C

#define VKALT_1 0xFF78
#define VKALT_2 0xFF79
#define VKALT_3 0xFF7A
#define VKALT_4 0xFF7B
#define VKALT_5 0xFF7C
#define VKALT_6 0xFF7D
#define VKALT_7 0xFF7E
#define VKALT_8 0xFF7F
#define VKALT_9 0xFF80

#define VKCTRL_A	(1)
#define VKCTRL_B	(2)
#define VKCTRL_C	(3)
#define VKCTRL_D	(4)
#define VKCTRL_E	(5)
#define VKCTRL_F	(6)
#define VKCTRL_G	(7)
#define VKCTRL_H	(8)
#define VKCTRL_I	(9)
#define VKCTRL_J	(10)
#define VKCTRL_K	(11)
#define VKCTRL_L	(12)
#define VKCTRL_M	(13)
#define VKCTRL_N	(14)
#define VKCTRL_O	(15)
#define VKCTRL_P	(16)
#define VKCTRL_Q	(17)
#define VKCTRL_R	(18)
#define VKCTRL_S	(19)
#define VKCTRL_T	(20)
#define VKCTRL_U	(21)
#define VKCTRL_V	(22)
#define VKCTRL_W	(23)
#define VKCTRL_X	(24)
#define VKCTRL_Y	(25)
#define VKCTRL_Z	(26)

#define VKESCAPE	(27)

/**
 * 
 */
struct DisplayBufferInfo
{
	int	screenheight;
	int	screenwidth;
	int	attribute;
	int	x;
	int	y;
};

class Viewer;

extern BOOL				gSearchForward;
extern Style*			gDefaultStyle;
extern Style*			gCurrStyle;
extern int				gCurrFile;

#if defined(__OS2__)
struct DisplayCell
{
	union {
		char	AsciiChar;
	} Char;

	unsigned char Attributes;
};
#elif defined(__DPMI32__)
struct DisplayCell
{
	union {
		char	AsciiChar;
	} Char;

	unsigned char Attributes;
};
#elif defined(__WIN32__)
typedef CHAR_INFO	DisplayCell;
#else
struct DisplayCell
{
	union {
		char	AsciiChar;
	} Char;

	unsigned char Attributes;
};
#endif

void PutTextBuf(int x, int y, int wx, int wy, DisplayCell* buf);
void GetTextBuf(int x, int y, int wx, int wy, DisplayCell* buf);
void MoveTextBuf(int left, int top, int right, int bottom, int destleft, int desttop);
void MoveToXY(int x, int y);

#if defined(__OS2__)
extern volatile TID		gTimeThread;
extern volatile BOOL		gTimeThreadEnd;
#elif defined(__WIN32__)
extern volatile HANDLE	gTimeThread;
extern volatile BOOL		gTimeThreadEnd;
#endif

#define OFF (0)
#define ON  (1)
#define YES (1)
#define NO  (0)

//#CLASS#-----------------------------oOo-----------------------------------//
// App
//#END#-------------------------------oOo-----------------------------------//

class App
{
public:
	App(int argc, char* argv[]);
	~App();

	int Init();
	int Run();

	static SList<Viewer>&	GetViewedFiles();

private:
	int		iArgc;
	char**  iArgv;
	BOOL	iStartedBrowsing;
	BOOL	iAbort;

	static SList<Viewer> cViewedFiles;
};

/**
 * 
 */
struct SetupInfo
{
	BOOL  	soundOn;
	BOOL  	regexpSearch;
	int   	statusTextForeColor;
	int   	statusTextBackColor;
	BOOL    caseSensitive;
	BOOL    keepFilesLoaded;
	int   	diskTextForeColor;
	int   	diskTextBackColor;
	int   	curDiskTextForeColor;
	int   	curDiskTextBackColor;
	int   	fileTextForeColor;
	int   	fileTextBackColor;
	int   	curFileTextForeColor;
	int   	curFileTextBackColor;
};

extern struct SetupInfo	setupInfo;

//#CLASS#-----------------------------oOo-----------------------------------//
// PickListItem
//#END#-------------------------------oOo-----------------------------------//

class PickListItem
{
public:
	PickListItem(CString& s): iStr(s), iSelected(FALSE) {}

	CString	GetString() { return iStr; }
	BOOL		IsSelected() const { return iSelected; }
	void		Select()  { iSelected = TRUE; }
	void		Deselect()  { iSelected = FALSE; }

private:
	CString	iStr;
	BOOL		iSelected;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// PickList
//#END#-------------------------------oOo-----------------------------------//

class PickList
{
public:
	PickList(BOOL multiSelect);

	void 				Add(CString s);
	int				Pick(CString msg, int& startPos, int* count = 0);
	PickListItem&  GetPickListItem(int i);
	int				GetCurrItem();
	int				Size();

private:
	void	displayList();
	void 	displayItemAt(int itemNo, int row);

	BOOL 						iMultiSelect;
	int						iFirstItem;
	int						iCurrItem;
	SList<PickListItem>	iListItems;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// FileInfo
//#END#-------------------------------oOo-----------------------------------//

class FileInfo
{
public:
	FileInfo(CString dirName, CString name, CString fileName, long fileSize,
				Time fileTime, Date fileDate, BOOL fromStdin = FALSE);
	~FileInfo();

	CString	iName;
	CString	iDirName;
	CString	iFileName;
	long		iFileSize;
	Time		iFileTime;
	Date		iFileDate;
	BOOL		iFromStdin;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// BookMark
//#END#-------------------------------oOo-----------------------------------//

class BookMark
{
public:
	BookMark(): iLine(-1), iCol(-1) {}
	BookMark(long line, long col): iLine(line), iCol(col) {}

	long iLine, iCol;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// OverWrite
//#END#-------------------------------oOo-----------------------------------//

class OverWrite
{
public:
	OverWrite() { MFail("Unimplemented"); }
	OverWrite(PCHAR p): iP(p), iCh(*p) {}

	PCHAR	iP;
	char	iCh;
};

/**
 * 
 */
enum LineStatus
{
	LS_NONE,
	LS_BOLD,
	LS_UNDERLINED,
	LS_BOLD_UNDERLINED,
	LS_INCOMMENT,
	LS_INPREPROCESSOR
};

//#CLASS#-----------------------------oOo-----------------------------------//
// LinePtr
//#END#-------------------------------oOo-----------------------------------//

class LinePtr
{
public:
	LinePtr();

	LineStatus	iLineStatus;
	PCHAR	iText;
	PCHAR	iNullByte;
	char	iOriginalChar;
};

/**
 * 
 */
#define BOLD_CODE			(('B'-'A')+1)	// character code which toggles bold
#define UNDERLINE_CODE	(('S'-'A')+1)	// character code which toggles underline

#define BYTES_PER_LINE_HEX_MODE 16

//#CLASS#-----------------------------oOo-----------------------------------//
// Viewer
//#END#-------------------------------oOo-----------------------------------//

class Viewer
{
public:
	Viewer() { MFail("Unimplemented"); }
	Viewer(FileInfo* dirent);
	~Viewer();

	void loadFile();
	void scanData();
	void dumpFile(BOOL bDump = !setupInfo.keepFilesLoaded);
	void handleKey(int key);
	void displayFileInfo(BOOL lineInfoOnly = FALSE);
	void displayData();
	void displayArrows();
	void clearSelected();
	BOOL fileHasChanged();
	BOOL shouldWrap(char* p, int len, int width);
	DisplayMode getDisplayMode();
	BOOL			getWordBreak();

	FileInfo* 	iFileInfo;

private:
	void handleKeyInTextMode(int key);
	void handleKeyInHexMode(int key);
	inline void displayChar(char ch, int& dispCol, BYTE defAttrib, BYTE useAttrib, BYTE curColor, DisplayCell*& bp, int& bufPos, Style* currStyle);
	inline void displayTab(int& dispCol, Style* currStyle, BYTE defAttrib, DisplayCell*& bp, int& bufPos);
	void displayTextLine(int line, int atY);
	void displayHexLine(int line, int atY);
	void displayFileInfo0(BOOL lineInfoOnly);
	void displayFileInfo1(BOOL lineInfoOnly);
	void displayFileInfo2(BOOL lineInfoOnly);
	void AdjustRowAndColumn(long& row, long newRow, long& col);
	void displayDataAsText();
	void displayDataAsHex();
	void replaceNULLS();
	void insertNULLS();
	long hexLineCount();
	void calcHexTopLine(long offset);
	void calcNearestHexTopLine(long topLine);
	void switchToHexMode();
	void switchToTextMode();

	PCHAR       		iData;
	LinePtr*      		iLinePtr;
	long        		iLineCount;
	BookMark			iBookMark[10];
	BOOL				iBinary;				// is this viewing a binary file
	BOOL				iIsBold;
	BOOL				iIsUnderlined;
	BOOL				iInComment;
	BOOL				iInPreprocessorStatement;
	Style*				iCurrStyle;
	BOOL				iChanged;

	// display related info
	long				iHexTopLine;
	long				iTopLine;
	long        		iColumn;          // column number at left of display
	long        		iSelectedLine;
	long        		iSelectedPos;
	long				iSelectedOffset;
	long              	iSelectedCount;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// LineEdit
//#END#-------------------------------oOo-----------------------------------//

class LineEdit
{
public:
	LineEdit(int len, char* prompt, char* buffer,
				SList<CString>* history, BOOL updateHistory = TRUE);
	~LineEdit();

	int Go();
private:
	int 					iX;
	int 					iY;
	int 					iLen;
	int 					iCurPos;
	CString				iPrompt;
	char*					iBuffer;
	char*					iCurText;
	SList<CString>*	iHistory;
	BOOL					iUpdateHistory;

	static BOOL		cInsertMode;
};

//#CLASS#-----------------------------oOo-----------------------------------//
// FileManDir
//#END#-------------------------------oOo-----------------------------------//

class FileManDir: public Directory
{
public:
	FileManDir(CPCHAR dirName = 0);
	virtual ~FileManDir();

	int 	fill(CPCHAR fileSpec = "*");

};

//#CLASS#-----------------------------oOo-----------------------------------//
// FileManager
//#END#-------------------------------oOo-----------------------------------//

class FileManager
{
public:
	FileManager();
	FileManager(CString& dir);
	~FileManager();

	void Activate(SList<Viewer>& viewedFiles,
						int& currFile,
						BOOL escapeAbort = FALSE);

private:
	void	DisplayDisks();
	void	DisplayDirectoryInfo();
	void	DisplayDirectory();
	BOOL	IsValidDisk(int disk);
	void	DisplayDirent(const Dirent& d, int x, int y);
	void	Hilight(int whichFile, BOOL bHilight = TRUE);
	void	Refresh();
	BOOL	MatchSelect(BOOL forward = TRUE, BOOL directories = FALSE);

	// user driven commands
	void	ChooseSortBy();
	void	ChangeDirectory();
	void	MakeDirectory();
	void	ChangeFileSpec();

	// File related user commands
	BOOL	EditFile(SList<Viewer>& viewedFiles, int& currFile);
	BOOL	ViewFile(SList<Viewer>& viewedFiles, int& currFile);
	BOOL	CopyFile(SList<Viewer>& viewedFiles, int& currFile);
	BOOL	DeleteFile(SList<Viewer>& viewedFiles, int& currFile);
	BOOL	RenameFile(SList<Viewer>& viewedFiles, int& currFile);
	BOOL	MoveFile(SList<Viewer>& viewedFiles, int& currFile);

	FileManDir*	iDirectory;
	CString		iCurDir;
	CString		iFileSpec;
	int			iCurFile;
	int			iCurColumn;
	int			iLinesPerColumn;
	int			iColumns;
	int			iColumnWidth;
	ULONG		iDriveMap;
	CString		iCurTextFile;
	CString		iCurTextDir;
	CString		iCurText;
};

/**
 * 
 */
#if defined(__OS2__)
extern HMTX        		hMtxListSync;
extern PSZ        		listSyncSemName;
#elif defined(__WIN32__)
extern HANDLE        	hMtxListSync;
extern const char*		listSyncSemName;
#endif

extern CString  			prevSrch;
extern CString				helpFile;
extern CString				iniFile;
extern CPCHAR 				stdinName;
extern DisplayCell*			screenBuf;
extern DisplayBufferInfo 		di;
extern int        		messageTime;
extern SList<Style>		styles;

extern Style*				gCurrStyle;
extern int					gCurrFile;
extern FileManager*			gFileManager;
extern CString				gStartupDir;

/**
 * function prototypes
 */
int nextTabStop(int col, Style* style = 0);
int ctrlbrk_ignore(void);
int getKey();
void WaitSync();
void ClearSync();
void dispTime(void* /* vp */);
void displayPrompt(CPCHAR msg);
void displayMsg(CPCHAR msg);
void displayErrMsg(CPCHAR msg);
void displayStdStatusLine();
void clearMsg();
BOOL isDupe(SList<Viewer>& viewedFiles, const Dirent& d);
int findDirent(SList<Viewer>& viewedFiles, const Dirent& d);
int findFile(SList<Viewer>& viewedFiles, const CString& s);
void chooseBuffer(SList<Viewer>& viewedFiles, int& currFile);
void cleanup(int key);
void AddFileList(CPCHAR fileSpec, SList<Viewer>& viewedFiles);
void setTextAttr(int foreground, int background);
void clrline();
void loadConfig(CString& configFile);
void saveConfig(CString& configFile);
BOOL Edit(CString fileName);
Style* findStyle(const CString styleName);
Style* findStyleForFile(const CString fileName);
char* GenerateSerialNo(unsigned short int i);
int IsValidSerialNo(char* serialNo);
void GetDisplayBufferInfo(DisplayBufferInfo* di);
void movetoxy(int x, int y);
void tprintf(const char* format, ...);

#endif
