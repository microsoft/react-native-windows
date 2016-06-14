using System.Runtime.InteropServices.WindowsRuntime;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

namespace ReactNative.Views.Image
{
    class ImageHelpers
    {
        internal static async void SetPixels(
            Border view,
            ImageReference image,
            Color? tintColor,
            Color? backgroundColor)
        {
            var pixels = new byte[image.PixelData.Length];
            image.PixelData.CopyTo(pixels, 0);

            for (var i = 3; i < pixels.Length; i += 4) // 0 = B, 1 = G, 2 = R, 3 = A
            {
                if (tintColor != null)
                {
                    if (pixels[i] != 0)
                    {
                        pixels[i - 3] = (byte)tintColor?.B;
                        pixels[i - 2] = (byte)tintColor?.G;
                        pixels[i - 1] = (byte)tintColor?.R;
                    }
                }

                if (backgroundColor != null)
                {
                    var frontAlpha = (double)pixels[i] / 255;
                    var backAlpha = (double)backgroundColor?.A / 255;

                    pixels[i - 3] = ColorValue(pixels[i - 3], frontAlpha, (double)backgroundColor?.B, backAlpha);
                    pixels[i - 2] = ColorValue(pixels[i - 2], frontAlpha, (double)backgroundColor?.G, backAlpha);
                    pixels[i - 1] = ColorValue(pixels[i - 1], frontAlpha, (double)backgroundColor?.R, backAlpha);
                    pixels[i] = AlphaValue(pixels[i], (double)backgroundColor?.A);
                }
            }

            var writableBitmap = new WriteableBitmap(image.Image.PixelWidth, image.Image.PixelHeight);

            // Open a stream to copy the image contents to the WriteableBitmap's pixel buffer 
            using (System.IO.Stream writeStream = writableBitmap.PixelBuffer.AsStream())
            {
                await writeStream.WriteAsync(pixels, 0, pixels.Length);
            }

            ((ImageBrush)view.Background).ImageSource = writableBitmap;

            image.Dispose();
        }

        private static byte ColorValue(double frontColor, double frontAlpha, double backColor, double backAlpha)
        {
            return (byte)((frontColor * frontAlpha + backColor * backAlpha * (1 - frontAlpha)) /
                                (frontAlpha + backAlpha * (1 - frontAlpha)));
        }

        private static byte AlphaValue(double frontAlpha, double backAlpha)
        {
            return (byte)(frontAlpha + backAlpha * (1 - (frontAlpha / 255)));
        }
    }
}
