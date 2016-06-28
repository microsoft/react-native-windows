using System;
using System.Collections.Generic;
using System.Threading;
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

        public ImageReference(BitmapImage image, int count, string key, Dictionary<string, ImageReference> dictionary)
        {
            _image = image;
            _referenceCount = count;
            _key = key;
            _imageDictionary = dictionary;
        }

        public string Key
        {
            get
            {
                return _key;
            }
        }

        public BitmapImage Image
        {
            get
            {
                return _image;
            }
        }

        public byte[] PixelData
        {
            get
            {
                return _pixelData;
            }
        }

        public void SetPixelData(byte[] data)
        {
            _pixelData = data;
        }

        public void Reserve()
        {
            Interlocked.Increment(ref _referenceCount);
        }

        public void Dispose()
        {
            Interlocked.Decrement(ref _referenceCount);

            if (_referenceCount == 0)
            {
                lock (_imageDictionary)
                {
                    _imageDictionary.Remove(_key);
                }

                _image = null;
                _pixelData = null;
            }
        }
    }
}
