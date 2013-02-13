(*
  "Hello, World!" - ���������������� ������.
  Copyright (c) 2000-2006, [ FAR group ]
  Delphi version copyright (c) 2000, Vasily V. Moshninov
*)

{$IFNDEF VIRTUALPASCAL}
  {$APPTYPE CONSOLE}
  {$MINENUMSIZE 4}
{$ENDIF}

library HelloWorld;

uses windows, plugin;

type
  TMessage = (MTitle, MMessage1, MMessage2, MMessage3, MMessage4, MButton);

var
  FARAPI: TPluginStartupInfo;

(*
 ������� GetMsg ���������� ������ ��������� �� ��������� �����.
 � ��� ���������� ��� Info.GetMsg ��� ���������� ���� :-)
*)
function GetMsg(MsgId: TMessage): PChar;
begin
  result:= FARAPI.GetMsg(FARAPI.ModuleNumber, integer(MsgId));
end;

(*
������� SetStartupInfo ���������� ���� ���, ����� �����
������� ���������. ��� ���������� ������� ����������,
����������� ��� ���������� ������.
*)
procedure SetStartupInfo(var psi: TPluginStartupInfo); stdcall;
begin
  Move(psi, FARAPI, SizeOf(FARAPI));
end;

(*
������� GetPluginInfo ���������� ��� ��������� ��������
  (general) ���������� � �������
*)
var
  PluginMenuStrings: array[0..0] of PChar;

procedure GetPluginInfo(var pi: TPluginInfo); stdcall;
begin
  pi.StructSize:= SizeOf(pi);
  pi.Flags:= PF_EDITOR;

  PluginMenuStrings[0]:= GetMsg(MTitle);
  pi.PluginMenuStrings:= @PluginMenuStrings;
  pi.PluginMenuStringsNumber:= 1;
end;

(*
  ������� OpenPlugin ���������� ��� �������� ����� ����� �������.
*)
function OpenPlugin(OpenFrom: integer; Item: integer): THandle; stdcall;
var
  Msg: array[0..6] of PChar;
begin
  Msg[0]:= GetMsg(MTitle);
  Msg[1]:= GetMsg(MMessage1);
  Msg[2]:= GetMsg(MMessage2);
  Msg[3]:= GetMsg(MMessage3);
  Msg[4]:= GetMsg(MMessage4);
  Msg[5]:= #01#00;                   // separator line
  Msg[6]:= GetMsg(MButton);

  FARAPI.Message(FARAPI.ModuleNumber,             // PluginNumber
                 FMSG_WARNING or FMSG_LEFTALIGN,  // Flags
                'Contents',                       // HelpTopic
                 @Msg,                            // Items
                 7,                               // ItemsNumber
                 1);                              // ButtonsNumber

  result:= INVALID_HANDLE_VALUE;
end;

exports
  SetStartupInfo,
  GetPluginInfo,
  OpenPlugin;

begin
end.
