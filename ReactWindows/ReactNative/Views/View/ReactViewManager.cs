using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.Storage;
using Windows.Storage.AccessCache;
using Windows.ApplicationModel.DataTransfer;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager;
using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;

namespace ReactNative.Views.View
{
    /// <summary>
    /// The UWP-specific RCTView implementation.
    /// </summary>
    public class ReactViewManager: BaseViewManager
    {
        /// <summary>
        /// The exported custom bubbling event types.
        /// </summary>
        public override IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants
        {
            get
            {
                return new Dictionary<string, object>()
                {
                    {
                        "topDragEnter",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onDragEnter" },
                                    { "captured" , "onDragEnterCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topDragOver",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onDragOver" },
                                    { "captured" , "onDragOverCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topDrop",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onDrop" },
                                    { "captured" , "onDropCapture" }
                                }
                            }
                        }
                    },
                    {
                        "topDragLeave",
                        new Dictionary<string, object>()
                        {
                            {
                                "phasedRegistrationNames",
                                new Dictionary<string, string>()
                                {
                                    { "bubbled" , "onDragLeave" },
                                    { "captured" , "onDragLeaveCapture" }
                                }
                            }
                        }
                    },
                };
            }
        }

        /// <summary>
        /// Enables the Canvas as a drop target.
        /// </summary>
        [ReactProp("allowDrop")]
        public void SetAllowDrop(BorderedCanvas view, bool allowDrop)
        {
            view.AllowDrop = allowDrop;

            if (allowDrop)
            {
                view.DragEnter += OnDragEnter;
                view.DragOver += OnDragOver;
                view.Drop += OnDrop;
                view.DragLeave += OnDragLeave;
            }
            else
            {
                view.DragEnter -= OnDragEnter;
                view.DragOver -= OnDragOver;
                view.Drop -= OnDrop;
                view.DragLeave -= OnDragLeave;
            }
        }

        private static async Task<JObject> GetDataTransferInfo(DataPackageView data, bool drop = false)
        {
            var files = new JArray();
            var items = new JArray();
            var types = new JArray();

            if (data.Contains(StandardDataFormats.StorageItems))
            {
                foreach (var item in await data.GetStorageItemsAsync())
                {
                    try
                    {
                        var file = item as StorageFile;

                        if (file != null)
                        {
                            var props = await file.GetBasicPropertiesAsync();
                            var type = file.ContentType;
                            var path = drop ? "blob:" + GetAccessToken(file) : "";

                            files.Add(new JObject
                            {
                                { "name", file.Name },
                                { "size", props.Size },
                                { "type", type },
                                { "uri", path }
                            });

                            items.Add(new JObject
                            {
                                { "kind", "file" },
                                { "type", type }
                            });

                            types.Add(type);
                        }
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine("GetDataTransferInfo: " + ex);
                    }
                }
            }

            return new JObject
            {
                { "files", files },
                { "items", items },
                { "types", types }
            };
        }

        private static string GetAccessToken(StorageFile file)
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

            return futureAccessList.Add(file, now.ToString(System.Globalization.CultureInfo.InvariantCulture));
        }

        private static async void OnDragEnter(object sender, DragEventArgs args)
        {
            var view = sender as BorderedCanvas;
            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragEnter", data));
        }

        private static async void OnDragOver(object sender, DragEventArgs args)
        {
            var view = sender as BorderedCanvas;

            // In web when JS gets a "drag over" event, it modifies the event
            // object to tell if the DOM element supports dropping items:
            //
            //      e.dataTransfer.effectAllowed = 'copy';
            //      e.dataTransfer.dropEffect = 'copy';
            //
            // However in RN this approach doesn't work, so we use a simpler
            // solution: JS sets allowDrop=true which implies that the RN element
            // always allows dropping items.
            args.AcceptedOperation = DataPackageOperation.Copy;

            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragOver", data));
        }

        private static async void OnDrop(object sender, DragEventArgs args)
        {
            var view = sender as BorderedCanvas;
            var data = await GetDataTransferInfo(args.DataView, true);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDrop", data));
        }

        private static async void OnDragLeave(object sender, DragEventArgs args)
        {
            var view = sender as BorderedCanvas;
            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragLeave", data));
        }

        class DragDropEvent : Event
        {
            private readonly string _name;
            private readonly JObject _data;

            public override string EventName => _name;
            public override bool CanCoalesce => false;

            public DragDropEvent(int viewTag, string name, JObject data)
                : base(viewTag)
            {
                _name = name;
                _data = data;
            }

            public override void Dispatch(RCTEventEmitter eventEmitter)
            {
                eventEmitter.receiveEvent(ViewTag, EventName, new JObject
                {
                    { "target", ViewTag },
                    { "dataTransfer", _data }
                });
            }
        }
    }
}
