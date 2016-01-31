﻿#ifndef CMDLINE_HPP_7E68C776_4AA9_4A24_BE9F_7F7FA6D50F30
#define CMDLINE_HPP_7E68C776_4AA9_4A24_BE9F_7F7FA6D50F30
#pragma once

/*
cmdline.hpp

Командная строка
*/
/*
Copyright © 1996 Eugene Roshal
Copyright © 2000 Far Group
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
#include "editcontrol.hpp"

struct execute_info
{
	enum wait_mode { no_wait, wait_idle, wait_finish };

	string Command;
	wait_mode WaitMode;
	bool NewWindow;
	bool DirectRun;
	bool RunAs;
};

class CommandLine:public SimpleScreenObject
{
public:
	CommandLine(window_ptr Owner);
	virtual ~CommandLine() {};

	virtual int ProcessKey(const Manager::Key& Key) override;
	virtual int ProcessMouse(const MOUSE_EVENT_RECORD *MouseEvent) override;
	virtual __int64 VMProcess(int OpCode,void *vParam=nullptr,__int64 iParam=0) override;

	const string& GetCurDir() const { return m_CurDir; }
	void SetCurDir(const string& CurDir);

	const string& GetString() const { return CmdStr.GetString(); }
	void SetString(const string& Str, bool Redraw);
	void InsertString(const string& Str);
	void ExecString(execute_info& Info);
	void ShowViewEditHistory();
	void SetCurPos(int Pos, int LeftPos=0, bool Redraw=true);
	int GetCurPos() const { return CmdStr.GetCurPos(); }
	int GetLeftPos() const { return CmdStr.GetLeftPos(); }
	void SetPersistentBlocks(bool Mode);
	void SetDelRemovesBlocks(bool Mode);
	void SetAutoComplete(int Mode);
	void GetSelection(intptr_t &Start,intptr_t &End) const { CmdStr.GetSelection(Start,End); }
	void Select(int Start, int End) { CmdStr.Select(Start,End); CmdStr.AdjustMarkBlock(); }
	void LockUpdatePanel(bool Mode);
	int GetPromptSize() const {return PromptSize;}
	void SetPromptSize(int NewSize);
	void DrawFakeCommand(const string& FakeCommand);

private:
	virtual void DisplayObject() override;
	size_t DrawPrompt();
	bool ProcessOSCommands(const string& CmdLine, class execution_context& ExecutionContext);
	std::list<std::pair<string, FarColor>> GetPrompt();
	static bool IntChDir(const string& CmdLine,int ClosePanel,bool Selent=false);

	friend class SetAutocomplete;

	int PromptSize;
	EditControl CmdStr;
	string m_CurDir;
	string strLastCmdStr;
	int LastCmdPartLength;
	string m_CurCmdStr;
	std::stack<string> ppstack;
};

#endif // CMDLINE_HPP_7E68C776_4AA9_4A24_BE9F_7F7FA6D50F30
