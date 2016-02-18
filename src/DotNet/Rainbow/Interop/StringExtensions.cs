// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Interop
{
    using System.Diagnostics.Contracts;
    using System.Text;

    public static class StringExtensions
    {
        private static Encoding UTF8 { get; } = new UTF8Encoding(false);

        /// <summary>
        /// UTF-8 encodes all the characters in the specified string into a null-terminated sequence
        /// of bytes.
        /// </summary>
        /// <param name="s">The <see cref="string"/> containing the set of characters to encode.</param>
        /// <returns>A byte array containing the results of encoding the specified set of characters.</returns>
        [Pure]
        public static byte[] GetUTF8Bytes(this string s)
        {
            int length = UTF8.GetByteCount(s);
            byte[] bytes = new byte[length + 1];
            UTF8.GetBytes(s, 0, s.Length, bytes, 0);
            bytes[length] = byte.MinValue;
            return bytes;
        }

        [Pure]
        public static string ToCamelCase(this string s)
        {
            if (string.IsNullOrEmpty(s))
            {
                return s;
            }

            if (s.Length == 1)
            {
                return s.ToLowerInvariant();
            }

            return char.ToLowerInvariant(s[0]) + s.Substring(1);
        }

        [Pure]
        public static string ToPascalCase(this string s)
        {
            if (string.IsNullOrEmpty(s))
            {
                return s;
            }

            if (s.Length == 1)
            {
                return s.ToUpperInvariant();
            }

            return char.ToUpperInvariant(s[0]) + s.Substring(1);
        }

        public static string ToSnakeCase(this string s, StringBuilder sb = null)
        {
            if (string.IsNullOrEmpty(s))
            {
                return s;
            }

            if (s.Length == 1)
            {
                return s.ToLowerInvariant();
            }

            sb = sb ?? new StringBuilder(s.Length * 2);
            sb.Append(char.ToLowerInvariant(s[0]));
            for (int i = 1; i < s.Length; ++i)
            {
                char c = s[i];
                if (char.IsUpper(c))
                {
                    sb.Append('_');
                    sb.Append(char.ToLowerInvariant(c));
                }
                else
                {
                    sb.Append(c);
                }
            }

            return sb.ToString();
        }
    }
}
