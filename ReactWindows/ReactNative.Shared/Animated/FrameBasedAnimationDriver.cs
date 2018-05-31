// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Json;
using System;

namespace ReactNative.Animated
{
    class FrameBasedAnimationDriver : AnimationDriver
    {
        private const double FrameTimeMilliseconds = 1000.0 / 60.0;

        private readonly double[] _frames;
        private readonly double _toValue;

        private long _startFrameTimeTicks = -1;
        private double _fromValue;
        private int _iterations;
        private int _currentLoop;

        public FrameBasedAnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback, JObject config)
            : base(id, animatedValue, endCallback)
        {
            _frames = config.GetValue("frames", StringComparison.Ordinal).ToObject<double[]>();
            _toValue = config.Value<double>("toValue");
            _iterations = config.ContainsKey("iterations") ? config.Value<int>("iterations") : 1;
            _currentLoop = 1;
            HasFinished = _iterations == 0;
        }

        public override void RunAnimationStep(TimeSpan renderingTime)
        {
            if (_startFrameTimeTicks < 0)
            {
                _startFrameTimeTicks = renderingTime.Ticks;
                _fromValue = AnimatedValue.RawValue;
            }

            var timeFromStartTicks = (renderingTime.Ticks - _startFrameTimeTicks) / 10000.0;
            // frames are calcualted at 60FPS, to get index by a given time offset from the start of the
            // animation, we take the time diff in milliseconds and divide it by 60 frames per 1000ms.
            var frameIndex = (int)(timeFromStartTicks / FrameTimeMilliseconds);
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
                nextValue = _toValue;
                if (_iterations == -1 || _currentLoop < _iterations)
                {
                    // looping animation, return to start
                    _startFrameTimeTicks = renderingTime.Ticks;
                    _currentLoop++;
                }
                else
                {
                    // animation has completed, no more frames left
                    HasFinished = true;
                }
            }
            else
            {
                nextValue = _fromValue + _frames[frameIndex] * (_toValue - _fromValue);
            }

            AnimatedValue.RawValue = nextValue;
        }
    }
}
