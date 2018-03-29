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
        /// Checks the syntax of the FAL URI. It doesn't check if
        /// this URI corresponds to an entry in the FAL.
        /// </summary>
        public static bool IsValidURI(string uri)
        {
            return uri.StartsWith(Scheme);
        }

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
            var token = ExtractAccessToken(uri);
            return await StorageApplicationPermissions.FutureAccessList.GetFileAsync(token);
        }

        /// <summary>
        /// Resolves a URI returned by the Add(folder) method to the folder object.
        /// The folder object remains in the cache and can be retrieved again later.
        /// </summary>
        public static async Task<StorageFolder> GetFolderAsync(string uri)
        {
            var token = ExtractAccessToken(uri);
            return await StorageApplicationPermissions.FutureAccessList.GetFolderAsync(token);
        }

        /// <summary>
        /// Checks if FAL contains a file or folder with the given URI.
        /// Returns false If the URI is invalid.
        /// </summary>
        public static bool ContainsItem(string uri)
        {
            if (TryExtractAccessToken(uri, out var token))
            {
                return StorageApplicationPermissions.FutureAccessList.ContainsItem(token);
            }
            else
            {
                return false;
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

        private static string ExtractAccessToken(string uri)
        {
            if (!TryExtractAccessToken(uri, out var token))
            {
                throw new ArgumentException("Invalid future access list URI: " + uri);
            }
            else
            {
                return token;
            }
        }

        private static string GetAccessToken(IStorageItem item)
        {
            var futureAccessList = StorageApplicationPermissions.FutureAccessList;
            var existingEntries = futureAccessList.Entries;
            var capacity = futureAccessList.MaximumItemsAllowed;
            var now = DateTime.UtcNow;

            var expiry = existingEntries.Count >= capacity / 2 ?
                now.AddDays(-1) :
                now.AddDays(-7);

            foreach (var entry in existingEntries)
            {
                if (long.TryParse(entry.Metadata, out long ticks))
                {
                    var time = new DateTime(ticks);

                    if (time < expiry)
                    {
                        futureAccessList.Remove(entry.Token);
                    }
                }
            }

            return futureAccessList.Add(item, now.Ticks.ToString());
        }
    }
}
