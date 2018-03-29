#if RNCODEGEN
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.AccessCache;

namespace ReactNative.CodeGen
{
    static class MetadataWriter
    {
        private const string MetadataTokenKey = "RNCODEGEN_KEY";

        private static readonly object _gate = new object();

        private static StorageFolder _folder;
        private static Task<StorageFolder> _getFolderTask;

        public static async void Write(JObject metadata)
        {
            await Task.Run(async () =>
            {
                var folder = await GetFolderAsync();
                var name = metadata.Value<string>("name");
                var file = await folder.CreateFileAsync($"{name}.json", CreationCollisionOption.ReplaceExisting);
                File.WriteAllText(file.Path, metadata.ToString());
            });
        }

        private static Task<StorageFolder> GetFolderAsync()
        {
            lock (_gate)
            {
                if (_getFolderTask == null)
                {
                    _getFolderTask = GetFolderCoreAsync();
                }
            }

            return _getFolderTask;
        }

        private static async Task<StorageFolder> GetFolderCoreAsync()
        {
            if (_folder == null)
            {
                var picker = new Windows.Storage.Pickers.FolderPicker();
                picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.Desktop;
                picker.FileTypeFilter.Add(".json");
                _folder = await DispatcherHelpers.CallOnDispatcher(async () => await picker.PickSingleFolderAsync()).Unwrap();
                if (ApplicationData.Current.LocalSettings.Values.TryGetValue(MetadataTokenKey, out var token))
                {
                    StorageApplicationPermissions.FutureAccessList.AddOrReplace((string)token, _folder);
                }
                else
                {
                    var newToken = StorageApplicationPermissions.FutureAccessList.Add(_folder);
                    ApplicationData.Current.LocalSettings.Values.Add(MetadataTokenKey, newToken);
                }
            }

            return _folder;
        }
    }
}
#endif
