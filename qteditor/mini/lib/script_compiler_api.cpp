/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Qt port of the Win32 ScriptCompilerAPI.cpp.  The process-pipe based child
* invocation is replaced with QProcess, and paths use std::filesystem.
*/

#include "ScriptCompilerAPI.h"

#include "manage.h"
#include "script_compiler_settings.h"

#include <QProcess>
#include <QSettings>
#include <QStringList>

#include <filesystem>
#include <fstream>
#include <string>

namespace {
const int kDefaultWarningLevel = 3;
const int kDefaultDebugType = 2;
} // namespace

static void readCompilerSettings(std::string &compilerPath, int &warningLevel, int &debugType) {
  QSettings settings;
  QString compiler;
  ScriptCompilerSettings::load(settings, compiler, warningLevel, debugType);
  compilerPath = compiler.toStdString();
}

int ScriptCompile(tCompilerInfo *ci) {
  if (!ci)
    return CERR_SOURCENOEXIST;

  if (!std::filesystem::exists(ci->source_filename))
    return CERR_SOURCENOEXIST;

  std::string compilerPath;
  int warningLevel = -1;
  int debugType = -1;
  readCompilerSettings(compilerPath, warningLevel, debugType);

  if (compilerPath.empty())
    return CERR_NOCOMPILERDEFINED;
  if (!std::filesystem::exists(compilerPath))
    return CERR_COMPILERMISSING;

  if (warningLevel < 0 || warningLevel > 4)
    warningLevel = kDefaultWarningLevel;
  if (debugType < 0 || debugType > 2)
    debugType = kDefaultDebugType;

  QStringList args;
  args << QStringLiteral("-f") << QString::fromStdString(ci->source_filename);
  args << QStringLiteral("-dir") << QString::fromStdString(LocalScriptDir.string());
  if (ci->script_type == ST_LEVEL)
    args << QStringLiteral("-level");
  args << QStringLiteral("-w") << QString::number(warningLevel);
  switch (debugType) {
  case 1:
    args << QStringLiteral("-d") << QStringLiteral("coff");
    break;
  case 2:
    args << QStringLiteral("-d") << QStringLiteral("c7");
    break;
  default:
    break;
  }

  QProcess proc;
  proc.setProcessChannelMode(QProcess::MergedChannels);
  proc.start(QString::fromStdString(compilerPath), args);
  if (!proc.waitForStarted(5000))
    return CERR_COMPILERMISSING;

  // Stream output to the callback as it becomes available.
  if (ci->callback) {
    while (proc.waitForReadyRead(50) || proc.state() != QProcess::NotRunning) {
      QByteArray chunk = proc.readAllStandardOutput();
      if (chunk.isEmpty()) {
        if (proc.state() == QProcess::NotRunning)
          break;
        continue;
      }
      // Normalize newlines like the Win32 version (\n -> \r\n, drop lone \r).
      QByteArray out;
      out.reserve(chunk.size());
      for (int i = 0; i < chunk.size(); ++i) {
        if (chunk[i] == '\n') {
          out.append('\r');
          out.append('\n');
        } else if (chunk[i] != '\r') {
          out.append(chunk[i]);
        }
      }
      out.append('\0');
      ci->callback(out.data());
    }
  }

  proc.waitForFinished(30000);
  return CERR_NOERR;
}

bool ScriptCreateEmptyScript(const std::string &filename, uint8_t script_type) {
  if (filename.empty())
    return false;

  if (script_type != ST_LEVEL && script_type != ST_GAME)
    return false;

  std::filesystem::path fullpath = LocalScriptDir / filename;
  if (std::filesystem::exists(fullpath))
    return false;

  std::ofstream file(fullpath);
  if (!file)
    return false;

  auto w = [&file](const std::string &line) {
    file.write(line.data(), static_cast<std::streamsize>(line.size()));
    file.put('\n');
  };

  w("// " + filename);
  w("// 0.1");
  w("#include <stdio.h>");
  w("#include <stdlib.h>");
  w("#include <string.h>");
  w("#include \"osiris_import.h\"");
  w("#include \"osiris_common.h\"");
  w("");

  w("#ifdef _MSC_VER		//Visual C++ Build");
  w("#define STDCALL		__stdcall");
  w("#define STDCALLPTR	*STDCALL");
  w("#else				//Non-Visual C++ Build");
  w("#define STDCALL __attribute__((stdcall))");
  w("#define STDCALLPTR	STDCALL*");
  w("#endif");
  w("");

  w("#ifdef __cplusplus");
  w("extern \"C\"{");
  w("#endif");

  w("char	STDCALL InitializeDLL(tOSIRISModuleInit *func_list);");
  w("void	STDCALL ShutdownDLL(void);");
  w("int     STDCALL GetGOScriptID(const char *name,uint8_t is_door);");
  w("void	STDCALLPTR CreateInstance(int id);");
  w("void	STDCALL DestroyInstance(int id,void *ptr);");
  w("int16_t	STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data);");
  w("int		STDCALL SaveRestoreState( void *file_ptr, uint8_t saving_state );");
  if (script_type == ST_LEVEL) {
    w("int		STDCALL GetTriggerScriptID(int trigger_room, int trigger_face );");
    w("int		STDCALL GetCOScriptList( int **list, int **id_list );");
  }
  w("#ifdef __cplusplus");
  w("}");
  w("#endif");
  w("");

  w("int String_table_size = 0;");
  w("char **String_table = NULL;");
  w("static const char *_Error_string = \"!!ERROR MISSING STRING!!\";");
  w("static const char *_Empty_string = \"\";");
  w("const char *GetStringFromTable(int index)");
  w("{");
  w("	if( (index<0) || (index>=String_table_size) )");
  w("		return _Error_string;");
  w("	if(!String_table[index])");
  w("		return _Empty_string;");
  w("		return String_table[index];");
  w("}");
  w("#define TXT(x) GetStringFromTable(x)");
  w("");

  w("//	InitializeDLL");
  w("//	Purpose:");
  w("//		This function gets called when the DLL first gets loaded.  It will only be called once (until the");
  w("//	DLL is unloaded).  Passed in is a struct of data passed from the game needed for the DLL to interact");
  w("//	with D3.  Usually this function will just call osicommon_Initialize(), which sets up the imported");
  w("//	functions.  However, you can alloc some memory or whatever in this function, and free it in ShutdownDLL().");
  w("//	Note: You cannot call any imported functions until osicommon_Initialize() is called.");
  w("//	Returns 1 if initialization went ok, 0 if there was an error and the DLL should not be loaded.");
  w("char STDCALL InitializeDLL(tOSIRISModuleInit *func_list)");
  w("{");
  w("	osicommon_Initialize(func_list);");
  w("	String_table_size = func_list->string_count;");
  w("	String_table = func_list->string_table;");
  w("	if(func_list->game_checksum!=CHECKSUM)");
  w("	{ ");
  w("		mprintf(0,\"Game-Checksum FAIL!!! (%%ul!=%%ul)\\n\",func_list->game_checksum,CHECKSUM);");
  w("		mprintf(0,\"RECOMPILE YOUR SCRIPTS!!!\\n\");");
  w("		return 0;");
  w("	}");
  w("	return 1;");
  w("}");
  w("");

  w("//	ShutdownDLL");
  w("//	Purpose:");
  w("//		This function is called right before a DLL is about to be unloaded from memory.  You can free");
  w("//	any unfree'd memory, or anything else you need to do.  Don't worry about destroying any instances");
  w("//	of scripts, as they will all be automatically destroyed before this function is called.  The");
  w("//	same goes for any memory allocated with Scrpt_MemAlloc(), as this will automatically be freed");
  w("//	when a scripts instance is destroyed.");
  w("void STDCALL ShutdownDLL(void)");
  w("{");
  w("}");
  w("");

  w("//	GetGOScriptID");
  w("//	Purpose:");
  w("//		Given the name of the object (from its pagename), this function will search through its");
  w("//	list of General Object Scripts for a script with a matching name (to see if there is a script");
  w("//	for that type/id of object within this DLL).  If a matching scriptname is found, a UNIQUE ID");
  w("//	is to be returned back to Descent 3.  This ID will be used from here on out for all future");
  w("//	interaction with the DLL.  Since doors are not part of the generic object's, it's possible");
  w("//	for a door to have the same name as a generic object (OBJ_POWERUP, OBJ_BUILDING, OBJ_CLUTTER");
  w("//	or OBJ_ROBOT), therefore, a 1 is passed in for isdoor if the given object name refers to a");
  w("//	door, else it is a 0.  The return value is the unique identifier, else -1 if the script");
  w("//	does not exist in the DLL.");
  w("int STDCALL GetGOScriptID(const char *name,uint8_t isdoor)");
  w("{");
  w("	return -1;");
  w("}");
  w("");

  w("//	CreateInstance");
  w("//	Purpose:");
  w("//		Given an ID from a call to GetGOScriptID(), GetTriggerScriptID() or GetCOScriptList(), this");
  w("//	function will create a new instance for that particular script (by allocating and initializing");
  w("//	memory, etc.).  A pointer to this instance is to be returned back to Descent 3.  This pointer will");
  w("//	be passed around, along with the ID for CallInstanceEvent() and DestroyInstance().  Return NULL");
  w("//	if there was an error.");
  w("//	The only reserved ID is 0, which must be used for the level script");
  w("void STDCALLPTR CreateInstance(int id)");
  w("{");
  w("	return NULL;");
  w("}");
  w("");

  w("//	DestroyInstance");
  w("//	Purpose:");
  w("//		Given an ID, and a pointer to a particular instance of a script, this function will delete and");
  w("//	destruct all information associated with that script, so it will no longer exist.");
  w("void STDCALL DestroyInstance(int id,void *ptr)");
  w("{");
  w("}");
  w("");

  w("//	CallInstanceEvent");
  w("//	Purpose:");
  w("//		Given an ID, a pointer to a script instance, an event and a pointer to the struct of");
  w("//	information about the event, this function will translate who this event belongs to and");
  w("//	passes the event to that instance of the script to be handled.  Return a combination of");
  w("//	CONTINUE_CHAIN and CONTINUE_DEFAULT, to give instructions on what to do based on the");
  w("//	event. CONTINUE_CHAIN means to continue through the chain of scripts (custom script, level");
  w("//	script, mission script, and finally default script).  If CONTINUE_CHAIN is not specified,");
  w("//	than the chain is broken and those scripts of lower priority will never get the event.  Return");
  w("//	CONTINUE_DEFAULT in order to tell D3 if you want process the normal action that is built into");
  w("//	the game for that event.  This only pertains to certain events.  If the chain continues");
  w("//	after this script, than the CONTINUE_DEFAULT setting will be overridden by lower priority");
  w("//	scripts return value.");
  w("//	The only reserved ID is 0, which must be used for the level script");
  w("int16_t STDCALL CallInstanceEvent(int id,void *ptr,int event,tOSIRISEventInfo *data)");
  w("{");
  w("	return CONTINUE_CHAIN|CONTINUE_DEFAULT;");
  w("}");
  w("");

  if (script_type == ST_LEVEL) {
    w("//	GetTriggerScriptID");
    w("//	Purpose:");
    w("//		Given a room and face number, this function will return a unique ID (global DLL unique)");
    w("//	in which Descent 3 should use for all future interaction will the DLL when refering to");
    w("//	this trigger.  Return -1 if there is no trigger script available for the specified trigger.");
    w("//	The only reserved ID is 0, which must be used for the level script");
    w("int	STDCALL GetTriggerScriptID(int trigger_room,int trigger_face)");
    w("{");
    w("	return -1;");
    w("}");
    w("");

    w("//	GetCOScriptList");
    w("//	Purpose:");
    w("//		This function returns the pointers to the 2 arrays that Descent 3 should use to determine");
    w("//	what object's have custom scripts in this level DLL.  list should be set to point to the array");
    w("//	of object handles that have custom scripts in this DLL.  id_list should be set to point to the");
    w("//	corresponding array of unique IDs that match with the object handle list. This function should");
    w("//	return the size of the arrays returned...if no custom scripts are available, than this function");
    w("//	returns 0.");
    w("//	The only reserved ID is 0, which must be used for the level script");
    w("int	STDCALL GetCOScriptList( int **list, int **id_list )");
    w("{");
    w("	return 0;");
    w("}");
    w("");
  }

  w("//	SaveRestoreState");
  w("//	Purpose:");
  w("//		This function is called when Descent 3 is saving or restoring the game state.  In this function");
  w("//	you should save/restore any global data that you want preserved through load/save (which includes");
  w("//	demos).  You must be very careful with this function, corrupting the file (reading or writing too");
  w("//	much or too little) may be hazardous to the game (possibly making it impossible to restore the");
  w("//	state).  It would be best to use version information to keep older versions of saved states still");
  w("//	able to be used.  IT IS VERY IMPORTANT WHEN SAVING THE STATE TO RETURN THE NUMBER OF _BYTES_ WROTE");
  w("//	TO THE FILE.  When restoring the data, the return value is ignored.  saving_state is 1 when you should");
  w("//	write data to the file_ptr, 0 when you should read in the data.");
  w("int STDCALL SaveRestoreState( void *file_ptr, uint8_t saving_state )");
  w("{");
  w("	return 0;");
  w("}");
  w("");

  file.flush();
  return file.good();
}
