using System;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Simple structural type for containing dimensions.
    /// </summary>
    public struct Dimensions : IEquatable<Dimensions>
    {
        private const int prime = 17;

        /// <summary>
        /// The X-coordinate.
        /// </summary>
        public double X;

        /// <summary>
        /// The Y-coordinate.
        /// </summary>
        public double Y;

        /// <summary>
        /// The width.
        /// </summary>
        public double Width;

        /// <summary>
        /// The height.
        /// </summary>
        public double Height;

        /// <summary>
        /// Checks if two <see cref="Dimensions"/> instances are equal.
        /// </summary>
        /// <param name="other">The other object.</param>
        /// <returns>
        /// <code>true</code> if the instances are equal, otherwise <code>false</code>.
        /// </returns>
        public bool Equals(Dimensions other)
        {
            return X == other.X
                && Y == other.Y
                && Width == other.Width
                && Height == other.Height;
        }

        /// <summary>
        /// Checks if two instances are equal.
        /// </summary>
        /// <param name="obj">The other object.</param>
        /// <returns>
        /// <code>true</code> if the instances are equal, otherwise <code>false</code>.
        /// </returns>
        public override bool Equals(object obj)
        {
            if (!(obj is Dimensions))
            {
                return false;
            }

            return Equals((Dimensions)obj);
        }

        /// <summary>
        /// Gets a hash code for the instance.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            var hash = X.GetHashCode();
            unchecked
            {
                hash = hash * prime + Y.GetHashCode();
                hash = hash * prime + Width.GetHashCode();
                return hash * prime + Height.GetHashCode();
            }
        }

        /// <summary>
        /// Checks if two <see cref="Dimensions"/> instances are equal.
        /// </summary>
        /// <param name="x">The first instance.</param>
        /// <param name="y">The second instance.</param>
        /// <returns>
        /// <code>true</code> if the instances are equal, otherwise <code>false</code>.
        /// </returns>
        public static bool operator ==(Dimensions x, Dimensions y)
        {
            return x.Equals(y);
        }

        /// <summary>
        /// Checks if two <see cref="Dimensions"/> instances are not equal.
        /// </summary>
        /// <param name="x">The first instance.</param>
        /// <param name="y">The second instance.</param>
        /// <returns>
        /// <code>true</code> if the instances are equal, otherwise <code>false</code>.
        /// </returns>
        public static bool operator !=(Dimensions x, Dimensions y)
        {
            return !x.Equals(y);
        }
    }
}
