using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Modules.Storage
{
    class AsyncStorageModule : NativeModuleBase, ILifecycleEventListener
    {
        private const string DirectoryName = "AsyncStorage";
        private const string FileExtension = ".data";
        
        private static readonly IDictionary<char, string> s_charToString = new Dictionary<char, string>
        {
            { '\\', "{bsl}" },
            { '/', "{fsl}" },
            { ':', "{col}" },
            { '*', "{asx}" },
            { '?', "{q}" },
            { '<', "{gt}" },
            { '>', "{lt}" },
            { '|', "{bar}" },
            { '\"', "{quo}" },
            { '.', "{dot}" },
            { '{', "{ocb}" },
            { '}', "{ccb}" },
        };

        private static readonly IDictionary<string, char> s_stringToChar = s_charToString.ToDictionary(kv => kv.Value, kv => kv.Key);

        private static readonly int s_maxReplace = s_charToString.Values.Select(s => s.Length).Max();

        private readonly SemaphoreSlim _mutex = new SemaphoreSlim(1, 1);

        private StorageFolder _cachedFolder;

        public override string Name
        {
            get
            {
                return "AsyncLocalStorage";
            }
        }

        [ReactMethod]
        public async void multiGet(string[] keys, ICallback callback)
        {
            if (keys == null)
            {
                callback.Invoke(AsyncStorageErrorHelpers.GetInvalidKeyError(null), null);
                return;
            }

            var error = default(JObject);
            var data = new JArray();

            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                foreach (var key in keys)
                {
                    if (key == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidKeyError(null);
                        break;
                    }

                    var value = await GetAsync(key).ConfigureAwait(false);
                    data.Add(new JArray(key, value));
                }
            }
            finally
            {
                _mutex.Release();
            }

            if (error != null)
            {
                callback.Invoke(error);
            }
            else
            {
                callback.Invoke(null, data);
            }
        }

        [ReactMethod]
        public async void multiSet(string[][] keyValueArray, ICallback callback)
        {
            if (keyValueArray == null || keyValueArray.Length == 0)
            {
                callback.Invoke(AsyncStorageErrorHelpers.GetInvalidKeyError(null));
                return;
            }

            var error = default(JObject);

            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                foreach (var pair in keyValueArray)
                {
                    if (pair.Length != 2)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidValueError(null);
                        break;
                    }

                    if (pair[0] == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidKeyError(null);
                        break;
                    }

                    if (pair[1] == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidValueError(pair[0]);
                        break;
                    }

                    error = await SetAsync(pair[0], pair[1]).ConfigureAwait(false);
                    if (error != null)
                    {
                        break;
                    }
                }
            }
            finally
            {
                _mutex.Release();
            }

            if (error != null)
            {
                callback.Invoke(error);
            }
            else
            {
                callback.Invoke();
            }
        }

        [ReactMethod]
        public async void multiRemove(string[] keys, ICallback callback)
        {
            if (keys == null || keys.Length == 0)
            {
                callback.Invoke(AsyncStorageErrorHelpers.GetInvalidKeyError(null));
                return;
            }

            var error = default(JObject);

            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                foreach (var key in keys)
                {
                    if (key == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidKeyError(null);
                        break;
                    }

                    error = await RemoveAsync(key).ConfigureAwait(false);
                    if (error != null)
                    {
                        break;
                    }
                }
            }
            finally
            {
                _mutex.Release();
            }

            if (error != null)
            {
                callback.Invoke(error);
            }
            else
            {
                callback.Invoke();
            }
        }

        [ReactMethod]
        public async void multiMerge(string[][] keyValueArray, ICallback callback)
        {
            if (keyValueArray == null || keyValueArray.Length == 0)
            {
                callback.Invoke(AsyncStorageErrorHelpers.GetInvalidKeyError(null));
                return;
            }

            var error = default(JObject);

            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                foreach (var pair in keyValueArray)
                {
                    if (pair.Length != 2)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidValueError(null);
                        break;
                    }

                    if (pair[0] == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidKeyError(null);
                        break;
                    }

                    if (pair[1] == null)
                    {
                        error = AsyncStorageErrorHelpers.GetInvalidValueError(pair[0]);
                        break;
                    }

                    error = await MergeAsync(pair[0], pair[1]).ConfigureAwait(false);
                    if (error != null)
                    {
                        break;
                    }
                }
            }
            finally
            {
                _mutex.Release();
            }

            if (error != null)
            {
                callback.Invoke(error);
            }
            else
            {
                callback.Invoke();
            }
        }

        [ReactMethod]
        public async void clear(ICallback callback)
        {
            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                var storageFolder = await GetAsyncStorageFolder(false).ConfigureAwait(false);
                if (storageFolder != null)
                {
                    await storageFolder.DeleteAsync().AsTask().ConfigureAwait(false);
                    _cachedFolder = null;
                }
            }
            finally
            {
                _mutex.Release();
            }

            callback.Invoke();
        }

        [ReactMethod]
        public async void getAllKeys(ICallback callback)
        {
            var keys = new JArray();

            await _mutex.WaitAsync().ConfigureAwait(false);
            try
            {
                var storageFolder = await GetAsyncStorageFolder(false).ConfigureAwait(false);
                if (storageFolder != null)
                {
                    var items = await storageFolder.GetItemsAsync().AsTask().ConfigureAwait(false);
                    foreach (var item in items)
                    {
                        var itemName = item.Name;
                        if (itemName.EndsWith(FileExtension))
                        {
                            keys.Add(GetKeyName(itemName));
                        }
                    }
                }
            }
            finally
            {
                _mutex.Release();
            }

            callback.Invoke(null, keys);
        }

        public void OnSuspend()
        {
        }

        public void OnResume()
        {
        }

        public void OnDestroy()
        {
            _mutex.Dispose();
        }

        private async Task<string> GetAsync(string key)
        {
            var storageFolder = await GetAsyncStorageFolder(false).ConfigureAwait(false);
            if (storageFolder != null)
            {
                var fileName = GetFileName(key);
                var storageItem = await storageFolder.TryGetItemAsync(fileName).AsTask().ConfigureAwait(false);
                if (storageItem != null)
                {
                    var file = await storageFolder.GetFileAsync(fileName).AsTask().ConfigureAwait(false);
                    return await FileIO.ReadTextAsync(file).AsTask().ConfigureAwait(false);
                }
            }

            return null;
        }

        private async Task<JObject> MergeAsync(string key, string value)
        {
            var oldValue = await GetAsync(key).ConfigureAwait(false);

            var newValue = default(string);
            if (oldValue == null)
            {
                newValue = value;
            }
            else
            {
                var oldJson = JObject.Parse(oldValue);
                var newJson = JObject.Parse(value);
                DeepMergeInto(oldJson, newJson);
                newValue = oldJson.ToString(Formatting.None);
            }

            return await SetAsync(key, newValue).ConfigureAwait(false);
        }

        private async Task<JObject> RemoveAsync(string key)
        {
            var storageFolder = await GetAsyncStorageFolder(false).ConfigureAwait(false);
            if (storageFolder != null)
            {
                var fileName = GetFileName(key);
                var storageItem = await storageFolder.TryGetItemAsync(fileName).AsTask().ConfigureAwait(false);
                if (storageItem != null)
                {
                    await storageItem.DeleteAsync().AsTask().ConfigureAwait(false);
                }
            }

            return null;
        }

        private async Task<JObject> SetAsync(string key, string value)
        {
            var storageFolder = await GetAsyncStorageFolder(true).ConfigureAwait(false);
            var file = await storageFolder.CreateFileAsync(GetFileName(key), CreationCollisionOption.ReplaceExisting).AsTask().ConfigureAwait(false);
            await FileIO.WriteTextAsync(file, value).AsTask().ConfigureAwait(false);
            return default(JObject);
        }

        private async Task<StorageFolder> GetAsyncStorageFolder(bool createIfNotExists)
        {
            if (_cachedFolder == null)
            {
                var localFolder = ApplicationData.Current.LocalFolder;
                var storageFolderItem = await localFolder.TryGetItemAsync(DirectoryName);
                _cachedFolder = storageFolderItem != null || createIfNotExists
                    ? await localFolder.CreateFolderAsync(DirectoryName, CreationCollisionOption.OpenIfExists)
                    : null;
            }

            return _cachedFolder;
        }

        private static string GetFileName(string key)
        {
            var sb = default(StringBuilder);
            for (var i = 0; i < key.Length; ++i)
            {
                var ch = key[i];
                var replacement = default(string);
                if (s_charToString.TryGetValue(ch, out replacement))
                {
                    if (sb == null)
                    {
                        sb = new StringBuilder();
                        sb.Append(key, 0, i);
                    }

                    sb.Append(replacement);
                }
                else if (sb != null)
                {
                    sb.Append(ch);
                }
            }
            
            if (sb == null)
            {
                return string.Concat(key, FileExtension);
            }
            else
            {
                sb.Append(FileExtension);
                return sb.ToString();
            }
        }

        private static string GetKeyName(string fileName)
        {
            var length = fileName.Length - FileExtension.Length;
            var sb = default(StringBuilder);
            for (var i = 0; i < length; ++i)
            {
                var start = fileName[i];
                if (start == '{')
                {
                    var j = i;
                    while (j < length && (j - i) < s_maxReplace)
                    {
                        var end = fileName[++j];
                        if (end == '}')
                        {
                            break;
                        }
                    }

                    if (j < length && (j - i) < s_maxReplace)
                    {
                        var substring = fileName.Substring(i, j - i + 1);
                        var replacement = default(char);
                        if (s_stringToChar.TryGetValue(substring, out replacement))
                        {
                            if (sb == null)
                            {
                                sb = new StringBuilder();
                                sb.Append(fileName, 0, i);
                            }

                            sb.Append(replacement);
                            i = j;
                        }
                    }
                }
                else if (sb != null)
                {
                    sb.Append(start);
                }
            }

            return sb == null
                ? fileName.Substring(0, length)
                : sb.ToString();
        }

        private static void DeepMergeInto(JObject oldJson, JObject newJson)
        {
            foreach (var property in newJson)
            {
                var key = property.Key;
                var value = property.Value;
                var newInner = value as JObject;
                var oldInner = oldJson[key] as JObject;
                if (newInner != null && oldInner != null)
                {
                    DeepMergeInto(oldInner, newInner);
                }
                else
                {
                    oldJson[key] = value;
                }
            }
        }
    }
}
