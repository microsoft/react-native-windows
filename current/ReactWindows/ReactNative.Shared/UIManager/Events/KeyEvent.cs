// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using System;
#if WINDOWS_UWP
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
#else
using System.Windows.Input;
#endif


namespace ReactNative.UIManager.Events
{
#if WINDOWS_UWP
    using Key = VirtualKey;
#endif

    class KeyEvent : Event
    {
        public const string KeyDownEventString = "topKeyDown";
        public const string KeyUpEventString = "topKeyUp";
        public const string KeyPressEventString = "topKeyPress";

        private readonly string _key;
        private readonly int _keyCode;
        private readonly bool _shiftKey;
        private readonly bool _altKey;
        private readonly bool _ctrlKey;

        public KeyEvent(string eventName, int viewTag, Key key)
            : base(viewTag)
        {
            EventName = eventName;

            _key = key.ToString();
            _keyCode = key.GetKeyCode();

#if WINDOWS_UWP
            _shiftKey = Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Shift).HasFlag(CoreVirtualKeyStates.Down);
            _altKey = Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Menu).HasFlag(CoreVirtualKeyStates.Down);
            _ctrlKey = Window.Current.CoreWindow.GetKeyState(Windows.System.VirtualKey.Control).HasFlag(CoreVirtualKeyStates.Down);
#else
            _shiftKey = (Keyboard.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift;
            _altKey = (Keyboard.Modifiers & ModifierKeys.Alt) == ModifierKeys.Alt;
            _ctrlKey = (Keyboard.Modifiers & ModifierKeys.Control) == ModifierKeys.Control;
#endif
        }

        public override string EventName
        {
            get;
        }

        public override bool CanCoalesce
        {
            get
            {
                return false;
            }
        }

        public override void Dispatch(RCTEventEmitter eventEmitter)
        {
            var eventData = new JObject
                {
                    { "target", ViewTag },
                    { "key", _key },
                    { "keyCode", _keyCode },
                    { "shiftKey", _shiftKey },
                    { "altKey", _altKey },
                    { "ctrlKey", _ctrlKey },
              };

            eventEmitter.receiveEvent(ViewTag, EventName, eventData);
        }
    }
}
