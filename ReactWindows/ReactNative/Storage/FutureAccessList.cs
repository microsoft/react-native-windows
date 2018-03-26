using System;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.AccessCache;

namespace ReactNative.Storage
{
    /// <summary>
    /// The reason this future access list thing was introduced was a need
    /// to transfer a file object from RN to JS and then to another RN module.
    /// The naive approach to transfer the file path didn't work because the
    /// file path doesn't carry read/write permissions and a UWP app can directly
    /// access only files inside its app folder.
    ///
    /// Eventually, RNW will have a more general approach to store such things
    /// and it will support other schemes like blob. At that time this class will
    /// likely to be rewritten.
    /// </summary>
    public static class FutureAccessList
    {
        /// <summary>
        /// FAL URIs start with this prefix.
        /// </summary>
        public static string Scheme => "urn:future-access-list:";

        /// <summary>
        /// Stores a file or folder object in the future access list cache.
        /// The capacity of the cache is limited (up to about 1000 items) and
        /// if necessary old items can be removed to free space for new ones.
        /// </summary>
        public static string Add(IStorageItem item)
        {
            var token = GetAccessToken(item);
            return Scheme + Uri.EscapeUriString(token);
        }

        /// <summary>
        /// Removes the file or folder associated with this URI.
        /// </summary>
        public static void Remove(string uri)
        {
            if (TryExtractAccessToken(uri, out var token))
            {
                StorageApplicationPermissions.FutureAccessList.Remove(token);
            }
        }

        /// <summary>
        /// Resolves a URI returned by the Add(file) method to the file object.
        /// The file object remains in the cache and can be retrieved again later.
        /// </summary>
        public static async Task<StorageFile> GetFileAsync(string uri)
        {
            if (TryExtractAccessToken(uri, out var token))
            {
                return await StorageApplicationPermissions.FutureAccessList.GetFileAsync(token);
            }
            else
            {
                throw new ArgumentException("Invalid future access list URI: " + uri);
            }
        }

        /// <summary>
        /// Resolves a URI returned by the Add(folder) method to the folder object.
        /// The folder object remains in the cache and can be retrieved again later.
        /// </summary>
        public static async Task<StorageFolder> GetFolderAsync(string uri)
        {
            if (TryExtractAccessToken(uri, out var token))
            {
                return await StorageApplicationPermissions.FutureAccessList.GetFolderAsync(token);
            }
            else
            {
                throw new ArgumentException("Invalid future access list URI: " + uri);
            }
        }

        private static bool TryExtractAccessToken(string uri, out string token)
        {
            if (!uri.StartsWith(Scheme))
            {
                token = null;
                return false;
            }
            else
            {
                token = Uri.UnescapeDataString(uri.Substring(Scheme.Length));
                return true;
            }
        }

        private static string GetAccessToken(IStorageItem item)
        {
            var futureAccessList = StorageApplicationPermissions.FutureAccessList;
            var existingEntries = futureAccessList.Entries;
            var capacity = futureAccessList.MaximumItemsAllowed;
            var now = DateTime.Now;

            // Use more aggresive timeout when number of items exceed 100 or half of the capacity
            // Note: 100 is an arbitary limit in case that OS reduces the capacity (it is 1000 in RS1). 
            var expiry = existingEntries.Count >= capacity / 2 || existingEntries.Count >= 100 ?
                now.AddDays(-1) :
                now.AddDays(-7);

            foreach (var entry in existingEntries)
            {
                if (DateTime.TryParse(entry.Metadata, out DateTime time) && time < expiry)
                {
                    futureAccessList.Remove(entry.Token);
                }
            }

            return futureAccessList.Add(item, now.ToString(System.Globalization.CultureInfo.InvariantCulture));
        }
    }
}
