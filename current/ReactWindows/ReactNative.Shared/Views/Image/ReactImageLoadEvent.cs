// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.UIManager.Events;
using System;
using static System.FormattableString;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// Event class for all image loading related events.
    /// </summary>
    public class ReactImageLoadEvent : Event
    {
        /// <summary>
        /// The event identifier for image load start.
        /// </summary>
        public const int OnLoadStart = 1;

        /// <summary>
        /// The event identifier for image load.
        /// </summary>
        public const int OnLoad = 2;

        /// <summary>
        /// The event identifier for image load end.
        /// </summary>
        public const int OnLoadEnd = 3;

        /// <summary>
        /// The event identifier for image load error.
        /// </summary>
        public const int OnError = 4;

        private readonly int _eventType;
        private readonly string _imageUri;
        private readonly int _width;
        private readonly int _height;
        private readonly string _error;

        /// <summary>
        /// Instantiates a <see cref="ReactImageLoadEvent"/>.
        /// </summary>
        /// <param name="viewId">The view identifier.</param>
        /// <param name="eventType">The event identifier.</param>
        public ReactImageLoadEvent(int viewId, int eventType)
            : this(viewId, eventType, null, 0, 0)
        {
        }

        /// <summary>
        /// Instantiates a <see cref="ReactImageLoadEvent"/>.
        /// </summary>
        /// <param name="viewId">The view identifier.</param>
        /// <param name="eventType">The event identifier.</param>
        /// <param name="imageUri">The image URI.</param>
        /// <param name="width">The image width.</param>
        /// <param name="height">The image height.</param>
        public ReactImageLoadEvent(int viewId, int eventType, string imageUri, int width, int height) 
            : base(viewId)
        {
            _eventType = eventType;
            _imageUri = imageUri;
            _width = width;
            _height = height;
        }

        /// <summary>
        /// Instantiates a <see cref="ReactImageLoadEvent"/>.
        /// </summary>
        /// <param name="viewId">The view identifier.</param>
        /// <param name="error">The error string.</param>
        public ReactImageLoadEvent(int viewId, string error)
            : base(viewId)
        {
            _eventType = OnError;
            _error = error;
        }

        /// <summary>
        /// The name of the event.
        /// </summary>
        public override string EventName
        {
            get
            {
                switch (_eventType)
                {
                    case OnLoadStart:
                        return "topLoadStart";
                    case OnLoad:
                        return "topLoad";
                    case OnLoadEnd:
                        return "topLoadEnd";
                    case OnError:
                        return "topError";
                    default:
                        throw new InvalidOperationException(
                            Invariant($"Invalid image event '{_eventType}'."));
                }
            }
        }

        /// <summary>
        /// The coalescing key for the event.
        /// </summary>
        public override short CoalescingKey
        {
            get
            {
                return (short)_eventType;
            }
        }

        /// <summary>
        /// The sorting key for the event.
        /// </summary>
        public override int SortingKey
        {
            get
            {
                return _eventType;
            }
        }

        /// <summary>
        /// Dispatches the event.
        /// </summary>
        /// <param name="eventEmitter">The event emitter.</param>
        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = default(JObject);

            if (_imageUri != null || _eventType == OnLoad)
            {
                eventData = new JObject();

                if (_imageUri != null)
                {
                    eventData.Add("uri", _imageUri);
                }

                if (_eventType == OnLoad)
                {
                    var sourceData = new JObject
                    {
                        { "width", _width },
                        { "height", _height },
                    };

                    if (_imageUri != null)
                    {
                        sourceData.Add("url", _imageUri);
                    }

                    eventData.Add("source", sourceData);
                }
            }

            if (_eventType == OnError)
            {
                eventData = new JObject()
                {
                    { "error", _error }
                };
            }

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
