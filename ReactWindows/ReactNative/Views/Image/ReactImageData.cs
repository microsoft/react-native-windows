using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml;

namespace ReactNative.Views.Image
{
    class ReactImageData : IDisposable
    {
        private List<KeyValuePair<string, double>> _sources;

        public List<KeyValuePair<string, double>> Sources
        {
            get
            {
                return _sources;
            }
        }

        public string SourceUri { get; set; }

        public EffectManager EffectManager { get; private set; }

        public uint? TintColor { get; set; }

        public uint? BackgroundColor { get; set; }

        public double? BlurRadius { get; set; }

        public ReactImageData(FrameworkElement element)
        {
            EffectManager = new EffectManager(element);
        }

        public void SetSources(JArray sources)
        {
            if (_sources != default(List<KeyValuePair<string, double>>))
            {
                _sources.Clear();
            }
            else
            {
                _sources = new List<KeyValuePair<string, double>>(sources.Count);
            }

            foreach (var source in sources)
            {
                var sourceData = (JObject)source;
                _sources.Add(
                    new KeyValuePair<string, double>(
                        sourceData.Value<string>("uri"),
                        sourceData.Value<double>("width") * sourceData.Value<double>("height")));
            }

            _sources.Sort((p1, p2) => p1.Value.CompareTo(p2.Value));
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                EffectManager.Dispose();
            }
        }

        /// <summary>
        /// Disposes the <see cref="EffectManager"/>.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }
    }
}
