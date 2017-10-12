using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System.Collections.Generic;
using Windows.UI.ViewManagement;

namespace ReactNative.Modules.Accessibilityinfo
{
    class AccessibilityInfoModule : ReactContextNativeModuleBase
    {
        private readonly AccessibilitySettings _accessibility = new AccessibilitySettings();
        private readonly UISettings _settings = new UISettings();

        private string GetRgbaString(UIElementType type)
        {
            var color = _settings.UIElementColor(type);
            return "rgba(" + color.R + "," + color.G + "," + color.B + "," + color.A + ")";
        }

        private IDictionary<string, string> GetHighContrastColors()
        {
            return new Dictionary<string, string>
            {
                {  "windowText", GetRgbaString(UIElementType.WindowText) },
                {  "hotlight", GetRgbaString(UIElementType.Hotlight) },
                {  "grayText", GetRgbaString(UIElementType.GrayText) },
                {  "highlightText", GetRgbaString(UIElementType.HighlightText) },
                {  "highlight", GetRgbaString(UIElementType.Highlight) },
                {  "buttonText", GetRgbaString(UIElementType.ButtonText) },
                {  "buttonFace", GetRgbaString(UIElementType.ButtonFace) },
                {  "window", GetRgbaString(UIElementType.Window) },
            };
        }

        public AccessibilityInfoModule(ReactContext reactContext)
            : base(reactContext)
        {

        }

        public override string Name => "AccessibilityInfo";

        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    // TODO: It would be better to have a sync GethIsHighContrast,
                    // but this is not supported by the framework at the moment.
                    { "initialHighContrast", _accessibility.HighContrast },
                };
            }
        }

        public override void Initialize()
        {
            _accessibility.HighContrastChanged += OnHighContrastChanged;
        }

        public override void OnReactInstanceDispose()
        {
            _accessibility.HighContrastChanged -= OnHighContrastChanged;
        }

        private void OnHighContrastChanged(AccessibilitySettings sender, object args)
        {
            Context.GetJavaScriptModule<RCTDeviceEventEmitter>()
                .emit("highContrastDidChange", sender.HighContrast);
        }

        [ReactMethod]
        public void fetchIsHighContrast(IPromise promise)
        {
            promise.Resolve(_accessibility.HighContrast);
        }
    }
}