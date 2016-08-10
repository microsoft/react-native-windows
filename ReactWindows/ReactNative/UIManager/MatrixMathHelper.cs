using System;
using Windows.UI.Xaml.Media.Media3D;

namespace ReactNative.UIManager
{
    static class MatrixMathHelper
    {
        public static void MultiplyInto(ref Matrix3D m, Matrix3D o)
        {
            double a00 = m.M11, a01 = m.M12, a02 = m.M13, a03 = m.M14,
              a10 = m.M21, a11 = m.M22, a12 = m.M23, a13 = m.M24,
              a20 = m.M31, a21 = m.M32, a22 = m.M33, a23 = m.M34,
              a30 = m.OffsetX, a31 = m.OffsetY, a32 = m.OffsetZ, a33 = m.M44;

            double b0 = o.M11, b1 = o.M12, b2 = o.M13, b3 = o.M14;
            m.M11 = b0* a00 + b1* a10 + b2* a20 + b3* a30;
            m.M12 = b0* a01 + b1* a11 + b2* a21 + b3* a31;
            m.M13 = b0* a02 + b1* a12 + b2* a22 + b3* a32;
            m.M14 = b0* a03 + b1* a13 + b2* a23 + b3* a33;

            b0 = o.M21; b1 = o.M22; b2 = o.M23; b3 = o.M24;
            m.M21 = b0* a00 + b1* a10 + b2* a20 + b3* a30;
            m.M22 = b0* a01 + b1* a11 + b2* a21 + b3* a31;
            m.M23 = b0* a02 + b1* a12 + b2* a22 + b3* a32;
            m.M24 = b0* a03 + b1* a13 + b2* a23 + b3* a33;

            b0 = o.M31; b1 = o.M32; b2 = o.M33; b3 = o.M34;
            m.M31 = b0* a00 + b1* a10 + b2* a20 + b3* a30;
            m.M32= b0* a01 + b1* a11 + b2* a21 + b3* a31;
            m.M33 = b0* a02 + b1* a12 + b2* a22 + b3* a32;
            m.M34 = b0* a03 + b1* a13 + b2* a23 + b3* a33;

            b0 = o.OffsetX; b1 = o.OffsetY; b2 = o.OffsetZ; b3 = o.M44;
            m.OffsetX = b0* a00 + b1* a10 + b2* a20 + b3* a30;
            m.OffsetY = b0* a01 + b1* a11 + b2* a21 + b3* a31;
            m.OffsetZ = b0* a02 + b1* a12 + b2* a22 + b3* a32;
            m.M44 = b0* a03 + b1* a13 + b2* a23 + b3* a33;
        }

        public static void ApplyPerspective(ref Matrix3D m, double perspective)
        {
            m.M34 = -1 / perspective;
        }

        public static void ApplyScaleX(ref Matrix3D m, double factor)
        {
            m.M11 = factor;
        }

        public static void ApplyScaleY(ref Matrix3D m, double factor)
        {
            m.M22 = factor;
        }

        public static void ApplyScaleZ(ref Matrix3D m, double factor)
        {
            m.M33 = factor;
        }

        public static void ApplyTranslate2D(ref Matrix3D m, double x, double y)
        {
            m.OffsetX = x;
            m.OffsetY = y;
        }

        public static void ApplyTranslate3D(ref Matrix3D m, double x, double y, double z)
        {
            m.OffsetX = x;
            m.OffsetY = y;
            m.OffsetZ = z;
        }

        public static void ApplySkewX(ref Matrix3D m, double radians)
        {
            m.M21 = Math.Sin(radians);
            m.M22 = Math.Cos(radians);
        }

        public static void ApplySkewY(ref Matrix3D m, double radians)
        {
            m.M11 = Math.Cos(radians);
            m.M22 = Math.Sin(radians);
        }

        public static void ApplyRotateX(ref Matrix3D m, double radians)
        {
            m.M22 = Math.Cos(radians);
            m.M23 = Math.Sin(radians);
            m.M32 = -Math.Sin(radians);
            m.M33 = Math.Cos(radians);
        }

        public static void ApplyRotateY(ref Matrix3D m, double radians)
        {
            m.M11 = Math.Cos(radians);
            m.M13 = -Math.Sin(radians);
            m.M31 = Math.Sin(radians);
            m.M33 = Math.Cos(radians);
        }

        public static void ApplyRotateZ(ref Matrix3D m, double radians)
        {
            m.M11 = Math.Cos(radians);
            m.M12 = Math.Sin(radians);
            m.M21 = -Math.Sin(radians);
            m.M22 = Math.Cos(radians);
        }

        public static double DegreesToRadians(double value)
        {
            return value * Math.PI / 180;
        }
    }
}
