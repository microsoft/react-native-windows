// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Facebook.Yoga;

namespace ReactNative.UIManager
{
    class EdgeSpacing
    {
        /// <summary>
        /// Spacing type that represents the left direction. E.g. <code>marginLeft</code>.
        /// </summary>
        public const int Left = 0;

        /// <summary>
        /// Spacing type that represents the top direction. E.g. <code>marginTop</code>.
        /// </summary>
        public const int Top = 1;

        /// <summary>
        /// Spacing type that represents the right direction. E.g. <code>marginRight</code>.
        /// </summary>
        public const int Right = 2;

        /// <summary>
        /// Spacing type that represents the bottom direction. E.g. <code>marginBottom</code>.
        /// </summary>
        public const int Bottom = 3;

        /// <summary>
        /// Spacing type that represents start direction e.g. left in left-to-right, right in right-to-left.
        /// </summary>
        public const int Start = 4;

        /// <summary>
        /// Spacing type that represents end direction e.g. right in left-to-right, left in right-to-left.
        /// </summary>
        public const int End = 5;

        /// <summary>
        /// Spacing type that represents horizontal direction (left and right). E.g.
        /// <code>marginHorizontal</code>.
        /// </summary>
        public const int Horizontal = 6;

        /// <summary>
        /// Spacing type that represents vertical direction (top and bottom). E.g. <code>marginVertical</code>.
        /// </summary>
        public const int Vertical = 7;

        /// <summary>
        /// Spacing type that represents all directions (left, top, right, bottom). E.g. <code>margin</code>.
        /// </summary>
        public const int All = 8;

        private static readonly int[] s_flagsMap = {
            1, /*Left*/
            2, /*Top*/
            4, /*Right*/
            8, /*Bottom*/
            16, /*Start*/
            32, /*End*/
            64, /*Horizontal*/
            128, /*Vertical*/
            256, /*All*/
        };

        private readonly YogaValue[] _spacing = NewFullSpacingArray();
        private readonly YogaValue _defaultValue;
        private int _valueFlags = 0;
        private bool _hasAliasesSet;

        /// <summary>
        /// Instantiates a <see cref="EdgeSpacing"/>
        /// </summary>
        public EdgeSpacing()
            : this(YogaValue.Point(0))
        {
        }

        /// <summary>
        /// Instantiates a <see cref="EdgeSpacing"/>.
        /// </summary>
        /// <param name="defaultValue">The default spacing value.</param>
        public EdgeSpacing(YogaValue defaultValue)
        {
            _defaultValue = defaultValue;
        }

        /// <summary>
        /// Set a spacing value.
        /// </summary>
        /// <param name="spacingType">
        /// One of <see cref="Left" />, <see cref="Top" />, <see cref="Right" />, <see cref="Bottom" />,
        /// <see cref="Vertical" />, <see cref="Horizontal" />, <see cref="All" />.
        /// </param>
        /// <param name="value">the value for this direction.</param>
        /// <returns>
        /// <code>true</code> if the spacing has changed, or <code>false</code>
        /// if the same value was already set.
        /// </returns>
        public bool Set(int spacingType, YogaValue value)
        {
            if (!EpsilonEqualityComparer.Instance.Equals(_spacing[spacingType], value))
            {
                _spacing[spacingType] = value;

                if (YogaConstants.IsUndefined(value))
                {
                    _valueFlags &= ~s_flagsMap[spacingType];
                }
                else
                {
                    _valueFlags |= s_flagsMap[spacingType];
                }

                _hasAliasesSet =
                    (_valueFlags & s_flagsMap[All]) != 0 ||
                    (_valueFlags & s_flagsMap[Vertical]) != 0 ||
                    (_valueFlags & s_flagsMap[Horizontal]) != 0;

                return true;
            }

            return false;
        }

        /// <summary>
        /// Get the spacing for a direction. This takes into account any default values that have been Set.
        /// </summary>
        /// <param name="spacingType">
        /// One of <see cref="Left"/>, <see cref="Top" />, <see cref="Right" />, <see cref="Bottom" />.
        /// </param>
        /// <returns>The spacing value.</returns>
        public YogaValue Get(int spacingType)
        {
            var defaultValue = (spacingType == Start || spacingType == End
                ? YogaConstants.Undefined
                : _defaultValue);

            if (_valueFlags == 0)
            {
                return defaultValue;
            }

            if ((_valueFlags & s_flagsMap[spacingType]) != 0)
            {
                return _spacing[spacingType];
            }

            if (_hasAliasesSet)
            {
                var secondType = spacingType == Top || spacingType == Bottom ? Vertical : Horizontal;
                if ((_valueFlags & s_flagsMap[secondType]) != 0)
                {
                    return _spacing[secondType];
                }
                else if ((_valueFlags & s_flagsMap[All]) != 0)
                {
                    return _spacing[All];
                }
            }

            return defaultValue;
        }

        /// <summary>
        /// Get the raw value (that was Set using <see cref="Set" />), without taking into account
        /// any default values.
        /// </summary>
        /// <param name="spacingType">
        /// One of <see cref="Left" />, <see cref="Top" />, <see cref="Right" />,
        /// <see cref="Bottom" />, <see cref="Vertical" />, <see cref="Horizontal" />, 
        /// <see cref="All" />.
        /// </param>
        /// <returns>The raw spacing value.</returns>
        public YogaValue GetRaw(int spacingType)
        {
            return _spacing[spacingType];
        }

        /// <summary>
        /// Resets the spacing instance to its default state. This method is meant to be used when
        /// recycling <see cref="EdgeSpacing" /> instances.
        /// </summary>
        public void Reset()
        {
            for (var i = 0; i < _spacing.Length; ++i)
            {
                _spacing[i] = YogaConstants.Undefined;
            }

            _hasAliasesSet = false;
            _valueFlags = 0;
        }

        /// <summary>
        /// Try to get start value and fallback to given type if not defined. This is used privately
        /// by the layout engine as a more efficient way to fetch direction-aware values by
        /// avoid extra method invocations.
        /// </summary>
        /// <param name="spacingType">The preferred spacing type.</param>
        /// <param name="fallbackType">The fallback spacing type.</param>
        /// <returns>The spacing value.</returns>
        internal YogaValue GetWithFallback(int spacingType, int fallbackType)
        {
            return
                (_valueFlags & s_flagsMap[spacingType]) != 0
                    ? _spacing[spacingType]
                    : Get(fallbackType);
        }

        private static YogaValue[] NewFullSpacingArray()
        {
            var undefined = YogaValue.Undefined();
            return new[] {
                undefined,
                undefined,
                undefined,
                undefined,
                undefined,
                undefined,
                undefined,
                undefined,
                undefined,
            };
        }
    }
}
