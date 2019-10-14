using Microsoft.ReactNative.Managed;

namespace SampleLibraryCS
{
    [ReactModule]
    class CsStrings
    {

        [ReactMethod]
        public int Length(string value)
        {
            return value.Length;
        }

        [ReactMethod]
        public string Concat(string x, string y)
        {
            return x + y;
        }

        [ReactMethod]
        public void Substr(string value, int length, ReactCallback<string> resolve, ReactCallback<string> reject)
        {
            if (length <= value.Length)
            {
                resolve(value.Substring(0, length));
            }
            else
            {
                reject("length must be less or equal to string length");
            }
        }
    }
}
