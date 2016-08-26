using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Modules.Storage
{
    class AsyncStorageModule : NativeModuleBase, ILifecycleEventListener
    {
        private const string DirectoryName = "AsyncStorage\\";
        private const string FileExtension = ".data";

        private readonly SemaphoreSlim _mutex = new SemaphoreSlim(1, 1);

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
                var localFolder = ApplicationData.Current.LocalFolder;
                var storageItem = await localFolder.TryGetItemAsync(DirectoryName).AsTask().ConfigureAwait(false);
                if (storageItem != null)
                {
                    await storageItem.DeleteAsync().AsTask().ConfigureAwait(false);
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
                var localFolder = ApplicationData.Current.LocalFolder;
                var storageItem = await localFolder.TryGetItemAsync(DirectoryName).AsTask().ConfigureAwait(false);
                if (storageItem != null)
                {
                    var directory = await localFolder.GetFolderAsync(DirectoryName).AsTask().ConfigureAwait(false);
                    var items = await directory.GetItemsAsync().AsTask().ConfigureAwait(false);
                    foreach (var item in items)
                    {
                        var itemName = item.Name;
                        var itemLength = itemName.Length;
                        var extLength = FileExtension.Length;
                        if (itemName.EndsWith(FileExtension) && itemLength > extLength)
                        {
                            keys.Add(item.Name.Substring(0, itemLength - extLength));
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
            var localFolder = ApplicationData.Current.LocalFolder;
            var fileName = GetFileName(key);

            var storageItem = await localFolder.TryGetItemAsync(fileName).AsTask().ConfigureAwait(false);
            if (storageItem != null)
            {
                var file = await localFolder.GetFileAsync(fileName).AsTask().ConfigureAwait(false);
                return await FileIO.ReadTextAsync(file).AsTask().ConfigureAwait(false);
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
            var localFolder = ApplicationData.Current.LocalFolder;
            var fileName = GetFileName(key);
            var storageItem = await localFolder.TryGetItemAsync(fileName).AsTask().ConfigureAwait(false);
            if (storageItem != null)
            {
                await storageItem.DeleteAsync().AsTask().ConfigureAwait(false);
            }

            return null;
        }

        private async Task<JObject> SetAsync(string key, string value)
        {
            var localFolder = ApplicationData.Current.LocalFolder;
            var file = await localFolder.CreateFileAsync(GetFileName(key), CreationCollisionOption.ReplaceExisting).AsTask().ConfigureAwait(false);
            await FileIO.WriteTextAsync(file, value).AsTask().ConfigureAwait(false);
            return default(JObject);
        }

        private static string GetFileName(string key)
        {
            var sb = new StringBuilder();
            sb.Append(DirectoryName);
            foreach (var ch in key)
            {
                switch (ch)
                {
                    case '\\':
                        sb.Append("{bsl}");
                        break;
                    case '/':
                        sb.Append("{fsl}");
                        break;
                    case ':':
                        sb.Append("{col}");
                        break;
                    case '*':
                        sb.Append("{asx}");
                        break;
                    case '?':
                        sb.Append("{q}");
                        break;
                    case '<':
                        sb.Append("{lt}");
                        break;
                    case '>':
                        sb.Append("{gt}");
                        break;
                    case '|':
                        sb.Append("{bar}");
                        break;
                    case '"':
                        sb.Append("{quo}");
                        break;
                    case '.':
                        sb.Append("{dot}");
                        break;
                    case '{':
                        sb.Append("{ocb}");
                        break;
                    case '}':
                        sb.Append("{ccb}");
                        break;
                    default:
                        sb.Append(ch);
                        break;
                }
            }

            sb.Append(FileExtension);

            return sb.ToString();
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
