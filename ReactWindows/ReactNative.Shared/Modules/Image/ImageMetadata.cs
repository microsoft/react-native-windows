namespace ReactNative.Modules.Image
{
    struct ImageMetadata
    {
        public ImageMetadata(string uri, int width, int height)
        {
            Uri = uri;
            Width = width;
            Height = height;
        }

        public string Uri { get; }

        public int Width { get; }

        public int Height { get; }
    }
}
