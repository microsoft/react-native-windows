using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    class ImageReference : IDisposable
    {
        private BitmapImage _image;
        private byte[] _pixelData;
        private int _referenceCount;
        private string _key;
        private Dictionary<string, ImageReference> _imageDictionary;

        internal ImageReference(BitmapImage image, int count, string key, Dictionary<string, ImageReference> dictionary)
        {
            _image = image;
            _referenceCount = count;
            _key = key;
            _imageDictionary = dictionary;
        }

        internal BitmapImage Image
        {
            get
            {
                return _image;
            }
        }

        internal byte[] PixelData
        {
            get
            {
                return _pixelData;
            }
        }

        internal void SetPixelData(byte[] data)
        {
            _pixelData = data;
        }

        internal void Reserve()
        {
            _referenceCount++;
        }

        public void Dispose()
        {
            _referenceCount--;

            if (_referenceCount == 0)
            {
                _imageDictionary.Remove(_key);
                _image = null;
                _pixelData = null;
            }
        }
    }
}
