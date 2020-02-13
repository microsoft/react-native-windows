#include <pch.h>

#include "AsyncStorageModuleWin32.h"

/// Implements ASM using winsqlite3.dll (requires Windows version 10.0.10586)

namespace facebook {
namespace react {

AsyncStorageModuleWin32::AsyncStorageModuleWin32(PCSTR dbPath)
{
  sqlite3_open_v2(dbPath, &m_dbHandle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);

  int userVersion = 0;
  auto userVersionCallback = [](void* pUserVersion, int cCol, char** rgszColText, char** rgszColName)
  {
    *static_cast<int*>(pUserVersion) = atoi(rgszColText[0]);
    return SQLITE_OK;
  };

  sqlite3_config(SQLITE_CONFIG_LOG, &AsyncStorageModuleWin32::errorLogCallback, this);

  sqlite3_exec(m_dbHandle, u8"PRAGMA user_version", userVersionCallback, &userVersion, nullptr);

  if (userVersion == 0) {
    sqlite3_exec(m_dbHandle, u8"CREATE TABLE IF NOT EXISTS AsyncLocalStorage(key TEXT, value TEXT); PRAGMA user_version=1", nullptr, nullptr, nullptr);
  }
}

AsyncStorageModuleWin32::~AsyncStorageModuleWin32()
{
  sqlite3_close(m_dbHandle);
}

std::string AsyncStorageModuleWin32::getName() {
  return "AsyncLocalStorage";
}

std::map<std::string, dynamic> AsyncStorageModuleWin32::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> AsyncStorageModuleWin32::getMethods() {
  return {
      Method("multiGet", this, &AsyncStorageModuleWin32::multiGet),
      Method("multiSet", this, &AsyncStorageModuleWin32::multiGet),
      Method("multiRemove", this, &AsyncStorageModuleWin32::multiRemove),
      // The 'multiMerge' method is currently not implemented. We assume that
      // the ReactNative framework translates its absence into the AsyncStorage
      // JS object (see Libraries\Storage\AsyncStorage.js around line 485) which
      // in turn allows JS user code to test for support of merge methods.
      Method("clear", this, &AsyncStorageModuleWin32::clear),
      Method("getAllKeys",this, &AsyncStorageModuleWin32::getAllKeys)
  };
}

void AsyncStorageModuleWin32::multiGet(folly::dynamic args, Callback jsCallback)
{
  folly::dynamic result = folly::dynamic::array;
  if (args.size() != 0)
  {
    sqlite3_exec(m_dbHandle, u8"BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    sqlite3_stmt* pStmt = nullptr;
    sqlite3_prepare_v2(m_dbHandle, u8"SELECT (key, value) FROM AsyncLocalStorage WHERE key = ?", -1, &pStmt, nullptr);
    for (auto&& arg : args)
    {
      sqlite3_bind_text(pStmt, 1, arg.getString().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_step(pStmt);
      result.push_back(folly::dynamic::array(sqlite3_column_text(pStmt, 0), sqlite3_column_text(pStmt, 1)));
      sqlite3_reset(pStmt);
    }
    sqlite3_finalize(pStmt);

    sqlite3_exec(m_dbHandle, u8"COMMIT", nullptr, nullptr, nullptr);

  }
  jsCallback({folly::dynamic(), result });
}

void AsyncStorageModuleWin32::mulitSet(folly::dynamic args, Callback jsCallback)
{
  if (args.size() != 0)
  {
    sqlite3_exec(m_dbHandle, u8"BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    sqlite3_stmt* pStmt = nullptr;
    sqlite3_prepare_v2(m_dbHandle, u8"INSERT OR REPLACE INTO AsyncLocalStorage VALUES(?, ?)", -1, &pStmt, nullptr);
    for (auto&& arg : args)
    {
      sqlite3_bind_text(pStmt, 1, arg[0].getString().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(pStmt, 2, arg[1].getString().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_step(pStmt);
      sqlite3_reset(pStmt);
    }
    sqlite3_finalize(pStmt);

    sqlite3_exec(m_dbHandle, u8"COMMIT", nullptr, nullptr, nullptr);

  }
  jsCallback({});
}

void AsyncStorageModuleWin32::multiRemove(folly::dynamic args, Callback jsCallback)
{
  if (args.size() != 0) {
    sqlite3_exec(m_dbHandle, u8"BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    sqlite3_stmt* pStmt = nullptr;
    sqlite3_prepare_v2(m_dbHandle, u8"DELETE FROM AsyncLocalStorage WHERE key = ?", -1, &pStmt, nullptr);
    for (auto&& arg : args)
    {
      sqlite3_bind_text(pStmt, 1, arg.getString().c_str(), -1, SQLITE_TRANSIENT);
      sqlite3_step(pStmt);
      sqlite3_reset(pStmt);
    }
    sqlite3_finalize(pStmt);

    sqlite3_exec(m_dbHandle, u8"COMMIT", nullptr, nullptr, nullptr);
  }
  jsCallback({});
}

void AsyncStorageModuleWin32::clear(folly::dynamic, Callback jsCallback)
{
  sqlite3_exec(m_dbHandle, u8"DELETE FROM AsyncLocalStorage", nullptr, nullptr, nullptr);
  jsCallback({});
}

void AsyncStorageModuleWin32::getAllKeys(folly::dynamic, Callback jsCallback)
{
  folly::dynamic jsRetVal = folly::dynamic::array;
  auto getAllKeysCallback = [](void* pUserVersion, int cCol, char** rgszColText, char** rgszColName)
  {
    folly::dynamic& retVal = *static_cast<folly::dynamic*>(pUserVersion);
    retVal.push_back(rgszColText[0]);
    return SQLITE_OK;
  };

  if (jsRetVal.size() == 0)
  {
    jsCallback({ "AsyncStorageError - No Keys Found", {}});
  }

  sqlite3_config(SQLITE_CONFIG_LOG, &AsyncStorageModuleWin32::errorLogCallback, this);

  sqlite3_exec(m_dbHandle, u8"SELECT key FROM AsyncLocalStorage", getAllKeysCallback, &jsRetVal, nullptr);


}

void AsyncStorageModuleWin32::errorLogCallbackImpl(int iErrCode, const char* msg)
{
  char errCode[32];
  sprintf_s(errCode, "Error: %d", iErrCode);

  ::OutputDebugStringA(errCode);
  ::OutputDebugStringA(msg);
}

} // namespace react
} // namespace facebook
