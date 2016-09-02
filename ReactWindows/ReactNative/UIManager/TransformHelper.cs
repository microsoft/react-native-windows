using Newtonsoft.Json.Linq;
using System;
using System.Linq;
using Windows.UI.Xaml.Media.Media3D;
using static System.FormattableString;

namespace ReactNative.UIManager
{
    static class TransformHelper
    {
        public static Matrix3D ProcessTransform(JArray transforms)
        {
            var result = Matrix3D.Identity;
            var firstToken = transforms.First;
            if (firstToken.Type != JTokenType.Object)
            {
                ApplyMatrix(ref result, transforms);
                return result;
            }

            foreach (var transform in transforms)
            {
                var helperMatrix = Matrix3D.Identity;
                var transformMap = (JObject)transform;
                var transformType = transformMap.Properties().SingleOrDefault().Name;
                switch (transformType)
                {
                    case "matrix":
                        ApplyMatrix(ref helperMatrix, (JArray)transformMap.GetValue(transformType));
                        break;
                    case "perspective":
                        MatrixMathHelper.ApplyPerspective(ref helperMatrix, transformMap.Value<double>(transformType));
                        break;
                    case "rotateX":
                        MatrixMathHelper.ApplyRotateX(
                            ref helperMatrix,
                            ConvertToRadians(transformMap, transformType));
                        break;
                    case "rotateY":
                        MatrixMathHelper.ApplyRotateY(
                            ref helperMatrix,
                            ConvertToRadians(transformMap, transformType));
                        break;
                    case "rotate":
                    case "rotateZ":
                        MatrixMathHelper.ApplyRotateZ(
                            ref helperMatrix,
                            ConvertToRadians(transformMap, transformType));
                        break;
                    case "scale":
                        var scale = transformMap.Value<double>(transformType);
                        MatrixMathHelper.ApplyScaleX(ref helperMatrix, scale);
                        MatrixMathHelper.ApplyScaleY(ref helperMatrix, scale);
                        break;
                    case "scaleX":
                        MatrixMathHelper.ApplyScaleX(ref helperMatrix, transformMap.Value<double>(transformType));
                        break;
                    case "scaleY":
                        MatrixMathHelper.ApplyScaleY(ref helperMatrix, transformMap.Value<double>(transformType));
                        break;
                    case "translate":
                        var value = (JArray)transformMap.GetValue(transformType);
                        var x = value.Value<double>(0);
                        var y = value.Value<double>(1);
                        var z = value.Count > 2 ? value.Value<double>(2) : 0.0;
                        MatrixMathHelper.ApplyTranslate3D(ref helperMatrix, x, y, z);
                        break;
                    case "translateX":
                        MatrixMathHelper.ApplyTranslate2D(ref helperMatrix, transformMap.Value<double>(transformType), 0.0);
                        break;
                    case "translateY":
                        MatrixMathHelper.ApplyTranslate2D(ref helperMatrix, 0.0, transformMap.Value<double>(transformType));
                        break;
                    case "skewX":
                        MatrixMathHelper.ApplySkewX(
                            ref helperMatrix,
                            ConvertToRadians(transformMap, transformType));
                        break;
                    case "skewY":
                        MatrixMathHelper.ApplySkewY(
                            ref helperMatrix,
                            ConvertToRadians(transformMap, transformType));
                        break;
                    default:
                        throw new InvalidOperationException(
                            Invariant($"Unsupported transform type: '{transformType}'"));
                }

                MatrixMathHelper.MultiplyInto(ref result, helperMatrix);
            }

            return result;
        }

        private static double ConvertToRadians(JObject transformMap, string key)
        {
            var value = default(double);
            var inRadians = true;
            var mapValue = transformMap.GetValue(key);
            if (mapValue.Type == JTokenType.String)
            {
                var stringValue = mapValue.Value<string>();
                if (stringValue.EndsWith("rad"))
                {
                    stringValue = stringValue.Substring(0, stringValue.Length - 3);
                }
                else if (stringValue.EndsWith("deg"))
                {
                    inRadians = false;
                    stringValue = stringValue.Substring(0, stringValue.Length - 3);
                }

                value = double.Parse(stringValue);
            }
            else
            {
                value = mapValue.Value<double>();
            }

            return inRadians ? value : MatrixMathHelper.DegreesToRadians(value);
        }

        private static void ApplyMatrix(ref Matrix3D matrix, JArray values)
        {
            matrix.M11 = values.Value<double>(0);
            matrix.M12 = values.Value<double>(1);
            matrix.M13 = values.Value<double>(2);
            matrix.M14 = values.Value<double>(3);
            matrix.M21 = values.Value<double>(4);
            matrix.M22 = values.Value<double>(5);
            matrix.M23 = values.Value<double>(6);
            matrix.M24 = values.Value<double>(7);
            matrix.M31 = values.Value<double>(8);
            matrix.M32 = values.Value<double>(9);
            matrix.M33 = values.Value<double>(10);
            matrix.M34 = values.Value<double>(11);
            matrix.OffsetX = values.Value<double>(12);
            matrix.OffsetY = values.Value<double>(13);
            matrix.OffsetZ = values.Value<double>(14);
            matrix.M44 = values.Value<double>(15);
        }
    }
}
