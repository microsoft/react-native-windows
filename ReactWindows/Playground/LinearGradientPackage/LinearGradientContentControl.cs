using ReactNative.UIManager;
using Windows.UI;
using Windows.UI.Xaml.Media;

namespace BV.LinearGradient
{
    public class LinearGradientContentControl : BorderedContentControl
    {
        private uint[] _colors;
        private double[] _locations;
        private double[] _start;
        private double[] _end;
            
        /// <summary>
        /// Instantiates the <see cref="LinearGradientContentControl"/>.
        /// </summary>
        /// <param name="content">The content.</param>
        public LinearGradientContentControl(object content) : base(content)
        {
        }

        /// <summary>
        /// Set the gradient direction line start point.
        /// </summary>
        /// <param name="start">The start point.</param>
        public void SetStart(double[] start)
        {
            _start = start;
            CreateLinearGradient();
        }

        /// <summary>
        /// Set the gradient direction line end point.
        /// </summary>
        /// <param name="end">The end point.</param>
        public void SetEnd(double[] end)
        {
            _end = end;
            CreateLinearGradient();
        }

        /// <summary>
        /// Set the gradient colors.
        /// </summary>
        /// <param name="colors">The gradient colors.</param>
        public void SetColors(uint[] colors)
        {
            _colors = colors;
            CreateLinearGradient();
        }

        /// <summary>
        /// Set the offset locations for the gradient stops.
        /// </summary>
        /// <param name="locations">The masked color.</param>
        public void SetLocations(double[] locations)
        {
            _locations = locations;
            CreateLinearGradient();
        }

        private void CreateLinearGradient()
        {
            if (_colors?.Length > 0)
            {
                var delta = 1.0;
                var useDelta = true;
                var stopCollection = new GradientStopCollection();

                if (_locations?.Length == _colors.Length)
                {
                    useDelta = false;
                }
                else if (_colors.Length > 1)
                {
                    delta = 1.0 / (_colors.Length - 1);
                }

                for (var i = 0; i < _colors.Length; i++)
                {
                    var stop = new GradientStop();
                    stop.Color = Parse(_colors[i]);

                    if (useDelta)
                    {
                        stop.Offset = i * delta;
                    }
                    else
                    {
                        stop.Offset = _locations[i];
                    }      
                      
                    stopCollection.Add(stop);
                }

                var angle = 90.0;

                if (_start?.Length == 2 && _end?.Length == 2)
                {
                    var x = _end[0] - _start[0];
                    var y = _end[1] - _start[1];

                    if (x != 0)
                    {
                        angle = 90.0 - System.Math.Atan(y / x);
                    }
                    else
                    {
                        angle = 0;
                    }
                }

                var brush = new LinearGradientBrush(stopCollection, angle);

                var customBorder = EnsureBorder();
                customBorder.Background = brush;
            }
        }

        private static Color Parse(uint value)
        {
            var color = value;
            var b = (byte)color;
            color >>= 8;
            var g = (byte)color;
            color >>= 8;
            var r = (byte)color;
            color >>= 8;
            var a = (byte)color;
            return Color.FromArgb(a, r, g, b);
        }
    }
}
