// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <AsyncStorage/KeyValueStorage.h>

using namespace std;

namespace facebook {
  namespace react {

    KeyValueStorage::KeyValueStorage(const WCHAR* storageFileName) :
      m_fileIOHelper{ make_unique<StorageFileIO>(storageFileName) },
      m_kvMap{ map<string, string>() }
    {
      // start the load procedure
      m_storageFileLoaded = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, SYNCHRONIZE | EVENT_MODIFY_STATE);
      if (m_storageFileLoaded == NULL)
        StorageFileIO::throwLastErrorMessage();

      m_storageFileLoader = async(launch::async, &KeyValueStorage::load, this);
    }

    void KeyValueStorage::load()
    {
      bool saveRequired = false;  // this flag is used to indicate whether the file needs to be cleaned up.

      string currentKey;
      string unescapedLine;

      std::string line;
      line.reserve(EstimatedValueSize);

      m_fileIOHelper->resetLine();

      while (m_fileIOHelper->getLine(line))
      {
        if (line.size() > 0)
        {
          char prefix = line.at(0);
          line.erase(0, 1);
          unescapeString(line);      // get the line without the prefix ($, %, R) and unescapes the \n and \ chars
          switch (prefix) {          // switch on first char in line
          case KeyPrefix:
            currentKey = line;
            break;

          case ValuePrefix:
            if (m_kvMap.count(currentKey))
              saveRequired = true;
            m_kvMap[currentKey] = line;
            break;

          case RemovePrefix:
            saveRequired = true;
            m_kvMap.erase(currentKey);
            break;

          default:
            throw std::exception("Corrupt storage file. Unexpected prefix on line.");
            break;
          }
        }
      }

      if (saveRequired) // cleanup the AOF by dumping the in memory map
      {
        m_fileIOHelper->clear();
        stringstream cleanedUpFile;

        for (auto const& entry : m_kvMap) // convert in memory map to a string
        {
          string key = entry.first;
          string value = entry.second;

          escapeString(key);
          escapeString(value);

          cleanedUpFile << KeyPrefix << key << '\n' << ValuePrefix << value << '\n';
        }

        m_fileIOHelper->append(cleanedUpFile.str());
      }

      if (!SetEvent(m_storageFileLoaded))
        StorageFileIO::throwLastErrorMessage();
    }

    void KeyValueStorage::waitForStorageLoadComplete()
    {
      using namespace std::chrono;
      using namespace std::chrono_literals;

      const auto dwMilliseconds = static_cast<DWORD>(duration_cast<milliseconds>(30s).count());
      if (WaitForSingleObject(m_storageFileLoaded, dwMilliseconds) != WAIT_OBJECT_0)
        StorageFileIO::throwLastErrorMessage();

      if (m_storageFileLoader.valid())
        m_storageFileLoader.get();
    }

    vector<tuple<string, string>> KeyValueStorage::multiGet(const vector<string>& keys)
    {
      waitForStorageLoadComplete();

      vector<tuple<string, string>> result;
      for (auto const& k : keys)
      {
        if (m_kvMap.find(k) != m_kvMap.end())
        {
          result.emplace_back(k, m_kvMap[k]);
        }
      }

      return result;
    }

    void KeyValueStorage::multiSet(const vector<tuple<string, string>>& keyValuePairs)
    {
      waitForStorageLoadComplete();

      stringstream appendEntry;
      bool fUpdateStorageFile = false;

      for (auto const& kvTuple : keyValuePairs)
      {
        string key = get<0>(kvTuple);
        string value = get<1>(kvTuple);

        // check if we need to modify the storage file
        // 1. if key does not exist
        // 2. if keys exists and value is different
        if (m_kvMap.find(key) == m_kvMap.end() || m_kvMap[key] != value)
        {
          // update the in-memory map
          m_kvMap[key] = value;
          fUpdateStorageFile = true;
          escapeString(key);
          escapeString(value);
          appendEntry << KeyPrefix << key << '\n' << ValuePrefix << value << '\n';
        }
      }

      if (fUpdateStorageFile)
      {
        // write the new keys to the file
        m_fileIOHelper->append(appendEntry.str());
      }
    }

    void KeyValueStorage::multiRemove(const vector<string>& keys)
    {
      waitForStorageLoadComplete();
      stringstream ss;

      for (auto const& k : keys)
      {
        ss << KeyPrefix << k << '\n' << RemovePrefix << '\n';
        m_kvMap.erase(k);
      }

      m_fileIOHelper->append(ss.str());
    }

    void KeyValueStorage::multiMerge(const vector<tuple<string, string>>& keyValuePairs)
    {
      throw std::exception("Error: not implemented.");
    }

    void KeyValueStorage::clear()
    {
      waitForStorageLoadComplete();

      m_kvMap.clear();
      m_fileIOHelper->clear();
    }

    vector<string> KeyValueStorage::getAllKeys()
    {
      waitForStorageLoadComplete();

      vector<string> keys;
      for (auto const& i : m_kvMap)
      {
        keys.push_back(i.first);
      }
      return keys;
    }

    void KeyValueStorage::escapeString(string& rawString)
    {
      int cSpecialChars = 0;
      for (auto const& c : rawString)
      {
        if (c == '\n' || c == '\\')
          cSpecialChars++;
      }

      if (cSpecialChars == 0)
        return;

      auto originalLength = rawString.length();
      string::size_type spaceToReserve{ originalLength + cSpecialChars };
      rawString.reserve(spaceToReserve);
      rawString.resize(spaceToReserve);

      char* read = &rawString[originalLength];
      char* write = &rawString[originalLength] + cSpecialChars;

      for (size_t i = 0; i < originalLength + 1; i++)
      {
        if (*read == '\\')
        {
          *write = '\\';
          write--;
          *write = '\\';
        }
        else if (*read == '\n')
        {
          *write = 'n';
          write--;
          *write = '\\';
        }
        else
        {
          *write = *read;
        }

        write--;
        read--;
      }
    }

    void KeyValueStorage::unescapeString(string& escapedString)
    {
      char* read = &escapedString[0];
      char* write = read;
      int cEscapedChars = 0;

      for (; *read; read++, write++)
      {
        if (*read != '\\')
        {
          *write = *read;
          continue;
        }

        read++;
        cEscapedChars++;

        switch (*read)
        {
        case '\\':
          *write = '\\';
          break;
        case 'n':
          *write = '\n';
          break;
        default:
          throw std::exception("Corrupt storage file. Found unexpected backslash.");
        }
      }
      *write = '\0';

      escapedString.resize(escapedString.length() - cEscapedChars);
    }
  }
}
