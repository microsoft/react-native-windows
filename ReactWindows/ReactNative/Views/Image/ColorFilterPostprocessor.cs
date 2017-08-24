using ImagePipeline.Request;
using Windows.Graphics.Imaging;

namespace ReactNative.Views.Image
{
    /// <summary>
    /// A postprocessor that applies tint and background color.
    /// </summary>
    public class ColorFilterPostprocessor : BasePostprocessor
    {
        private uint? _tintColor;
        private uint? _backgroundColor;

        /// <summary>
        /// Instantiates the <see cref="ColorFilterPostprocessor"/>.
        /// </summary>
        public ColorFilterPostprocessor(
            uint? tintColor,
            uint? backgroundColor)
        {
            _tintColor = tintColor;
            _backgroundColor = backgroundColor;
        }

        /// <summary>
        /// Process the bitmap pixel data.
        /// </summary>
        /// <param name="data">The bitmap pixel data.</param>
        /// <param name="width">
        /// The width of the new software bitmap, in pixels.
        /// </param>
        /// <param name="height">
        /// The height of the new software bitmap, in pixels.
        /// </param>
        /// <param name="format">
        /// The pixel format of the new software bitmap.
        /// </param>
        /// <param name="alpha">
        /// The alpha mode of the new software bitmap.
        /// </param>
        public override void Process(
            byte[] data,
            int width,
            int height,
            BitmapPixelFormat format,
            BitmapAlphaMode alpha)
        {
            for (var i = 3; i < data.Length; i += 4)
            {
                if (_tintColor != null && data[i] != 0)
                {
                    var color = _tintColor.Value;
                    data[i - 1] = (byte)((color & 0xFF0000) >> 16);
                    data[i - 2] = (byte)((color & 0xFF00) >> 8);
                    data[i - 3] = (byte)(color & 0xFF);
                }

                if (_backgroundColor != null && data[i] == 0)
                {
                    var color = _backgroundColor.Value;
                    data[i] = (byte)((color & 0xFF000000) >> 24);
                    data[i - 1] = (byte)((color & 0xFF0000) >> 16);
                    data[i - 2] = (byte)((color & 0xFF00) >> 8);
                    data[i - 3] = (byte)(color & 0xFF);
                }
            }
        }
    }
}
