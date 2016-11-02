using ReactNative.Modules.DevSupport;
using System.Collections.Generic;
#if WINDOWS_UWP
using Windows.Storage;
#else
using System;
using System.Configuration;
using static System.FormattableString;
#endif

namespace ReactNative.DevSupport
{
    class DevInternalSettings : IDeveloperSettings
    {
        private const string FpsDebugKey = "fps_debug";
        private const string DebugServerHostKey = "debug_http_host";
        private const string JsDevModeDebugKey = "js_dev_mode_debug";
        private const string AnimationsDebugKey = "animations_debug";
        private const string JsMinifyDebugKey = "js_minify_debug";
        private const string ReloadOnJSChangeKey = "reload_on_js_change";
        private const string HotModuleReplacementKey = "hot_module_replacement";

        private static readonly HashSet<string> s_triggerReload = new HashSet<string>
        {
            FpsDebugKey,
            ReloadOnJSChangeKey,
            JsDevModeDebugKey,
            JsMinifyDebugKey,
        };

        private readonly IDevSupportManager _debugManager;

        public DevInternalSettings(IDevSupportManager debugManager)
        {
            _debugManager = debugManager;
        }

        public bool IsJavaScriptDevModeEnabled
        {
            get
            {
#if DEBUG
                return GetSetting(JsDevModeDebugKey, true);
#else
                return GetSetting(JsDevModeDebugKey, false);
#endif
            }
            set
            {
                SetSetting(JsDevModeDebugKey, value);
            }
        }

        public string DebugServerHost
        {
            get
            {
                return GetSetting(DebugServerHostKey, default(string));
            }
            set
            {
                SetSetting(DebugServerHostKey, value);
            }
        }

        public bool IsAnimationFpsDebugEnabled
        {
            get
            {
                return GetSetting(ReloadOnJSChangeKey, false);
            }
            set
            {
                SetSetting(ReloadOnJSChangeKey, value);
            }
        }

        public bool IsElementInspectorEnabled { get; set; }

        public bool IsFpsDebugEnabled
        {
            get
            {
                return GetSetting(FpsDebugKey, false);
            }
            set
            {
                SetSetting(FpsDebugKey, value);
            }
        }

        public bool IsHotModuleReplacementEnabled
        {
            get
            {
                return GetSetting(HotModuleReplacementKey, false);
            }
            set
            {
                SetSetting(HotModuleReplacementKey, value);
            }
        }

        public bool IsJavaScriptMinifyEnabled
        {
            get
            {
                return GetSetting(JsMinifyDebugKey, false);
            }
            set
            {
                SetSetting(JsMinifyDebugKey, value);
            }
        }

        public bool IsReloadOnJavaScriptChangeEnabled
        {
            get
            {
                return GetSetting(ReloadOnJSChangeKey, false);
            }
            set
            {
                SetSetting(ReloadOnJSChangeKey, value);
            }
        }

        private T GetSetting<T>(string key, T defaultValue)
        {
#if WINDOWS_UWP
            var values = ApplicationData.Current.LocalSettings.Values;
            if (values.ContainsKey(key))
            {
                var data = values[key];
                if (data is T)
                {
                    return (T)data;
                }
            }

            return defaultValue;
#else
            if (typeof(T) == typeof(bool))
            {
                var result = default(bool);
                var parsed = bool.TryParse(ConfigurationManager.AppSettings[key], out result);
                return (T)(object)(parsed && result);
            }
            else
            {
                throw new NotSupportedException(Invariant($"Configuration values of type '{typeof(T)}' are not supported."));
            }
#endif
        }

        private void SetSetting<T>(string key, T value)
        {
#if WINDOWS_UWP
            var values = ApplicationData.Current.LocalSettings.Values;
            values[key] = value;
#else
            var values = ConfigurationManager.AppSettings;
            if (values[key] == null)
            {
                values.Add(key, value.ToString());
            }
            else
            {
                values[key] = value.ToString();
            }
#endif

            if (s_triggerReload.Contains(key))
            {
                _debugManager.ReloadSettings();
            }
        }
    }
}
