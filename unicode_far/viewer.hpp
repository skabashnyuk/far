#pragma once

/*
viewer.hpp

Internal viewer
*/
/*
Copyright (c) 1996 Eugene Roshal
Copyright (c) 2000 Far Group
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "scrobj.hpp"
#include "namelist.hpp"
#include "plugin.hpp"
#include "poscache.hpp"
#include "config.hpp"
#include "cache.hpp"

/* $ 10.07.2000 tran
   ! modified MAXSCRY from 120 to 300
   on win200, with Console height FAR work, but trap on viewer... */
#define  MAXSCRY     300

/* $ 12.07.2000 SVS
  - ��-�� ���������� ����� ������ �� 0x800 ������� FAR
    �� Alt-F7. �������� MAX_VIEWLINE �� 1024 (0x400)
*/
#define MAX_VIEWLINE  0x800 // 0x400
#define MAX_VIEWLINEB 0x80f // 0x40f

#define VIEWER_UNDO_COUNT   64

enum {VIEW_UNWRAP=0,VIEW_WRAP=1, VIEW_WORDWRAP=2};

class FileViewer;
class KeyBar;

struct ViewerString
{
	wchar_t *lpData /*[MAX_VIEWLINEB]*/;
	__int64 nFilePos;
	__int64 nSelStart;
	__int64 nSelEnd;
	bool bSelection;
};

struct InternalViewerBookMark
{
	DWORD64 SavePosAddr[BOOKMARK_COUNT];
	DWORD64 SavePosLeft[BOOKMARK_COUNT];
};

struct ViewerUndoData
{
	__int64 UndoAddr;
	__int64 UndoLeft;
};

enum SEARCH_FLAGS
{
	SEARCH_MODE2   = 0x00000001,
	REVERSE_SEARCH = 0x00000002
};

enum SHOW_MODES
{
	SHOW_RELOAD,
	SHOW_HEX,
	SHOW_UP,
	SHOW_DOWN
};

class Viewer:public ScreenObject
{
		friend class FileViewer;

	private:

		BitFlags SearchFlags;

		struct ViewerOptions ViOpt;

		bool Signature;

		NamesList ViewNamesList;
		KeyBar *ViewKeyBar;

		ViewerString *Strings[MAXSCRY+1];

		string strFileName;
		string strFullFileName;

		File ViewFile;
		CachedRead Reader;

		FAR_FIND_DATA_EX ViewFindData;

		string strTempViewName;

		BOOL DeleteFolder;

		string strLastSearchStr;
		int LastSearchCase,LastSearchWholeWords,LastSearchReverse,LastSearchHex,LastSearchRegexp;

		struct ViewerMode VM;

		__int64 FilePos;
		__int64 SecondPos;
		__int64 LastScrPos;
		__int64 FileSize;
		__int64 LastSelPos;

		__int64 LeftPos;
		__int64 LastPage;
		int CRSym;
		__int64 SelectPos,SelectSize;
		DWORD SelectFlags;
		__int64 SelectPosOffSet; // ������������ ��� ��������� ������� ��������� � ��������� ������
		int ShowStatusLine,HideCursor;

		string strTitle;

		string strPluginData;
		int CodePageChangedByUser;
		int ReadStdin;
		int InternalKey;

		struct InternalViewerBookMark BMSavePos;
		struct ViewerUndoData UndoData[VIEWER_UNDO_COUNT];

		int LastKeyUndo;
		int Width,XX2;  // , ������������ ��� ������� ������ ��� ���������
		int ViewerID;
		bool OpenFailed;
		bool bVE_READ_Sent;
		FileViewer *HostFileViewer;
		bool AdjustSelPosition;

		bool m_bQuickView;

		UINT DefCodePage;
	private:
		virtual void DisplayObject();

		void ShowPage(int nMode);

		void Up();
		void ShowHex();
		void ShowStatus();
		/* $ 27.04.2001 DJ
		   ������� ��� ��������� ����������, ��� ������������� ������ �
		   ����������� �� ������� ���������� � ��� ������������� ������� �����
		   �� ������� ������
		*/
		void DrawScrollbar();
		void AdjustWidth();
		void AdjustFilePos();

		void ReadString(ViewerString *pString, int MaxSize, int StrSize);
		int CalcStrSize(const wchar_t *Str,int Length);
		void ChangeViewKeyBar();
		void SetCRSym();
		void Search(int Next,int FirstChar);
		void ConvertToHex(char *SearchStr,int &SearchLength);
		int HexToNum(int Hex);
		int vread(wchar_t *Buf,int Count, bool Raw=false);
		int vseek(__int64 Offset,int Whence);
		__int64 vtell();
		bool vgetc(WCHAR& C);
		void SetFileSize();
		int GetStrBytesNum(const wchar_t *Str, int Length);

	public:
		Viewer(bool bQuickView = false, UINT aCodePage = CP_AUTODETECT);
		virtual ~Viewer();


	public:
		int OpenFile(const wchar_t *Name,int warning);
		void SetViewKeyBar(KeyBar *ViewKeyBar);

		virtual int ProcessKey(int Key);
		virtual int ProcessMouse(MOUSE_EVENT_RECORD *MouseEvent);
		virtual __int64 VMProcess(int OpCode,void *vParam=nullptr,__int64 iParam=0);

		void SetStatusMode(int Mode);
		void EnableHideCursor(int HideCursor);
		int GetWrapMode();
		void SetWrapMode(int Wrap);
		int GetWrapType();
		void SetWrapType(int TypeWrap);
		void KeepInitParameters();
		void GetFileName(string &strName);
		virtual void ShowConsoleTitle();

		void SetTempViewName(const wchar_t *Name, BOOL DeleteFolder);

		void SetTitle(const wchar_t *Title);
		string &GetTitle(string &Title,int SubLen=-1,int TruncSize=0);

		void SetFilePos(__int64 Pos); // $ 18.07.2000 tran - change 'long' to 'unsigned long'
		__int64 GetFilePos() const { return FilePos; };
		__int64 GetViewFilePos() const { return FilePos; };
		__int64 GetViewFileSize() const { return FileSize; };

		void SetPluginData(const wchar_t *PluginData);
		void SetNamesList(NamesList *List);

		int  ViewerControl(int Command,void *Param);
		void SetHostFileViewer(FileViewer *Viewer) {HostFileViewer=Viewer;};

		void GoTo(int ShowDlg=TRUE,__int64 NewPos=0,DWORD Flags=0);
		void GetSelectedParam(__int64 &Pos, __int64 &Length, DWORD &Flags);
		// ������� ��������� - ��� ��������������� �������
		void SelectText(const __int64 &MatchPos,const __int64 &SearchLength, const DWORD Flags=0x1);

		int GetTabSize() const { return ViOpt.TabSize; }
		void SetTabSize(int newValue) { ViOpt.TabSize=newValue; }

		int GetAutoDetectCodePage() const { return ViOpt.AutoDetectCodePage; }
		void SetAutoDetectCodePage(int newValue) { ViOpt.AutoDetectCodePage=newValue; }

		int GetShowScrollbar() const { return ViOpt.ShowScrollbar; }
		void SetShowScrollbar(int newValue) { ViOpt.ShowScrollbar=newValue; }

		int GetShowArrows() const { return ViOpt.ShowArrows; }
		void SetShowArrows(int newValue) { ViOpt.ShowArrows=newValue; }
		/* IS $ */
		int GetPersistentBlocks() const { return ViOpt.PersistentBlocks; }
		void SetPersistentBlocks(int newValue) { ViOpt.PersistentBlocks=newValue; }

		int GetHexMode() const { return VM.Hex; }

		UINT GetCodePage() const { return VM.CodePage; }

		NamesList *GetNamesList() { return &ViewNamesList; }

		/* $ 08.12.2001 OT
		  ���������� ������� ����, �������� �� ���� ���������
		  ������������ ��� �������� ������� ���������� � ������� �� */
		BOOL isTemporary();

		int ProcessHexMode(int newMode, bool isRedraw=TRUE);
		int ProcessWrapMode(int newMode, bool isRedraw=TRUE);
		int ProcessTypeWrapMode(int newMode, bool isRedraw=TRUE);
};
