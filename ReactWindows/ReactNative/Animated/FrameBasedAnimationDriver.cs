using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using System;

namespace ReactNative.Animated
{
    class FrameBasedAnimationDriver : AnimationDriver
    {
        private readonly double[] _frames;
        private readonly double _toValue;

        private long _startFrameTimeTicks = -1;
        private double _fromValue;

        public FrameBasedAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _frames = config.GetValue("frames").ToObject<double[]>();
            _toValue = config.Value<double>("toValue");
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            if (_startFrameTimeTicks < 0)
            {
                _startFrameTimeTicks = renderingTime.Ticks;
                _fromValue = AnimatedValue.Value;
            }

            var timeFromStartTicks = (renderingTime.Ticks - _startFrameTimeTicks);
            // frames are calcualted at 60FPS, to get index by a given time offset from the start of the
            // animation, we take the time diff in milliseconds and divide it by 60 frames per 1000ms.
            var frameIndex = (int)(timeFromStartTicks / 10000 * 60 / 1000);
            if (frameIndex < 0)
            {
                throw new InvalidOperationException("Calculated frame index should never be lower than 0.");
            }
            else if (HasFinished)
            {
                // nothing to do here
                return;
            }

            var nextValue = default(double);
            if (frameIndex >= _frames.Length - 1)
            {
                HasFinished = true;
                nextValue = _toValue;
            }
            else
            {
                nextValue = _fromValue + _frames[frameIndex] * (_toValue - _fromValue);
            }

            AnimatedValue.Value = nextValue;
        }
    }
}
