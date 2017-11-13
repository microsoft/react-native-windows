using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System.Collections.Generic;
using System;
using Windows.Storage;
using Windows.Storage.Search;
using System.Linq;
using Windows.Storage.FileProperties;

namespace ReactNative.Modules.CameraRoll
{
    /// <summary>
    /// Native module CameraRoll.
    /// CameraRoll provides access to the local camera roll / gallery.
    /// </summary>
    public class CameraRollModule : ReactContextNativeModuleBase
    {
        // Supported media file types
        private static readonly List<string> SupportedVideoFileTypes = new List<string> { ".mp4", ".wmv", ".avi" };
        private static readonly List<string> SupportedImageFileTypes = new List<string> { ".jpeg", ".jpg", ".png", ".gif", ".bmp" };
        
        /// <summary>
        /// Instantiates the <see cref="CameraRollModule"/>. 
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        public CameraRollModule(ReactContext reactContext) : base(reactContext)
        {
        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get { return "CameraRoll"; }    
        }

        /// <summary>
        /// Saves the photo or video to the camera roll / gallery.
        /// </summary>
        [ReactMethod]
        public async void saveToCameraRoll(JObject options, IPromise promise)
        {
            // TODO: tag, type?
            promise.Reject(new NotImplementedException("CameraRoll.saveToCameraRoll() method not yet implemented."));
        }

        /// <summary>
        /// Returns a Promise with photo/video identifier objects from the local camera roll.
        /// </summary>
        [ReactMethod]
        public async void getPhotos(JObject options, IPromise promise)
        {                
            try
            {                 
                if (options["first"] == null)
                {
                    promise.Reject(new ArgumentNullException("The option [first] must be defined."));
                    return;
                }

                if ((int) options["first"] < 1)
                {
                    promise.Reject(new ArgumentException("The option [first] is not valid."));
                    return;
                }

                var limit = (uint)(options["first"]);

                // TODO:
                // after
                // mimeTypes, groupName 

                if (options["groupTypes"] != null) {
                    promise.Reject(new ArgumentException("The option [groupTypes] is not supported on UWP"));
                    return;
                }

                QueryOptions queryOption;

                switch ((string)(options["assetType"]))
                {
                    case "All":
                        queryOption = new QueryOptions(CommonFileQuery.OrderByDate, SupportedVideoFileTypes
                          .Concat(SupportedImageFileTypes)
                          .ToList()
                         );
                        break;
                    case "Videos":
                        queryOption = new QueryOptions(CommonFileQuery.OrderByDate, SupportedVideoFileTypes);
                        break;
                    case "Photos":
                    default:
                        queryOption = new QueryOptions(CommonFileQuery.OrderByDate, SupportedImageFileTypes);
                        break;
                }

                StorageFolder picturesFolder = KnownFolders.CameraRoll;
                queryOption.FolderDepth = FolderDepth.Deep;

                IReadOnlyList<StorageFile> sortedItems = await picturesFolder
                  .CreateFileQueryWithOptions(queryOption)
                  .GetFilesAsync(0, limit);
                
                var result = new JObject();
                var edges = new JArray();
                var pageinfo = new JObject();
                var endcursor = "";

                foreach (StorageFile file in sortedItems)
                {
                    var node = new JObject();
                    var nodeObj = new JObject();
                    var image = new JObject();
                    var location = new JObject();
                    var uri = new Uri(file.Path);

                    ImageProperties imageProperties = await file.Properties.GetImagePropertiesAsync();

                    // Put ImageInfo
                    image.Add("uri", uri);
                    image.Add("width", imageProperties.Width);
                    image.Add("height", imageProperties.Height);

                    // Put BasicNodeInfo
                    node.Add("image", image);
                    node.Add("type", file.ContentType);
                    node.Add("timestamp", imageProperties.DateTaken.ToUnixTimeSeconds());
                    node.Add("group_name", file.DisplayName);

                    // Put LocationInfo
                    if (imageProperties.Latitude != null && imageProperties.Longitude != null)
                    {
                        location.Add("latitude", imageProperties.Latitude);
                        location.Add("longitude", imageProperties.Longitude);
                        node.Add("location", location);
                    }                    
                    
                    nodeObj.Add("node", node);
                    edges.Add(nodeObj);

                    endcursor = imageProperties.DateTaken.ToString();
                }

                // Put PageInfo
                pageinfo.Add("has_next_page", limit < sortedItems.Count);
                pageinfo.Add("end_cursor", endcursor);

                // Put Edges
                result.Add("edges", edges);
                result.Add("page_info", pageinfo);

                promise.Resolve(result);
            }
            catch (Exception ex)
            {
                promise.Reject(new InvalidOperationException("CameraRoll.getPhotos()", ex));
            }
        }
    }
}