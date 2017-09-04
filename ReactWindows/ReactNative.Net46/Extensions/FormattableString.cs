
namespace System
{
    class FormattableString : IFormattable
    {
        protected FormattableString()
        {
            
        }

        public string ToString(string format, IFormatProvider formatProvider)
        {
            return this.ToString();
        }

        public static string Invariant(FormattableString fString)
        {
            return fString.ToString();
        }

        public static string Invariant(String fString)
        {
            return fString;
        }
    }
}
