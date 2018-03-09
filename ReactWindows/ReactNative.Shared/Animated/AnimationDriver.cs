using ReactNative.Bridge;
using System;

namespace ReactNative.Animated
{
    abstract class AnimationDriver
    {
        public AnimationDriver(int id, ValueAnimatedNode animatedValue, ICallback endCallback)
        {
            Id = id;
            AnimatedValue = animatedValue;
            EndCallback = endCallback;
        }

        public int Id
        {
            get;
        }

        public ValueAnimatedNode AnimatedValue
        {
            get;
        }

        public ICallback EndCallback
        {
            get;
        }

        public bool HasFinished
        {
            get;
            protected set;
        }

        public abstract void RunAnimationStep(TimeSpan renderingTime);
    }
}
