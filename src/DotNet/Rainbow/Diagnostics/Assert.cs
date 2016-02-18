// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Diagnostics
{
    using System;
    using System.Diagnostics;
    using System.Diagnostics.Contracts;

    public static class Assert
    {
        private const string DEBUG = "DEBUG";
        private const string ValueCannotBeNull = "Value cannot be null.";
        private const string ValuesCannotBeEqual = "Values cannot be equal.";
        private const string ValuesMustBeEqual = "Values must be equal";

        [Conditional(DEBUG)]
        public static void Equal(int lhs, int rhs)
            => Debug.Assert(int.Equals(lhs, rhs), $"{ValuesMustBeEqual}: {lhs} vs {rhs}");

        [Conditional(DEBUG)]
        public static void Equal(double lhs, double rhs)
            => Debug.Assert(double.Equals(lhs, rhs), $"{ValuesMustBeEqual}: {lhs} vs {rhs}");

        [Conditional(DEBUG)]
        public static void Equal(string lhs, string rhs)
            => Debug.Assert(string.Equals(lhs, rhs), $"{ValuesMustBeEqual}: \"{lhs}\" vs \"{rhs}\"");

        [Conditional(DEBUG)]
        public static void Equal<T>(T lhs, T rhs) where T : class
            => Debug.Assert(IsEqual(lhs, rhs), $"{ValuesMustBeEqual}.");

        [Conditional(DEBUG)]
        public static void False(bool condition, string message)
            => Debug.Assert(!condition, message);

        [Conditional(DEBUG)]
        public static void NotEqual(int lhs, int rhs, string message = ValuesCannotBeEqual)
            => Debug.Assert(!int.Equals(lhs, rhs), message);

        [Conditional(DEBUG)]
        public static void NotEqual(double lhs, double rhs, string message = ValuesCannotBeEqual)
            => Debug.Assert(!double.Equals(lhs, rhs), message);

        [Conditional(DEBUG)]
        public static void NotEqual(string lhs, string rhs, string message = ValuesCannotBeEqual)
            => Debug.Assert(!string.Equals(lhs, rhs), message);

        [Conditional(DEBUG)]
        public static void NotEqual<T>(T lhs, T rhs, string message = ValuesCannotBeEqual) where T : class
            => Debug.Assert(!IsEqual(lhs, rhs), message);

        [Conditional(DEBUG)]
        public static void NotNull(IntPtr ptr, string message = ValueCannotBeNull)
            => Debug.Assert(ptr != IntPtr.Zero, message);

        [Conditional(DEBUG)]
        public static void NotNull<T>(T value, string message = ValueCannotBeNull) where T : class
            => Debug.Assert(value != null, message);

        [Conditional(DEBUG)]
        public static void Null<T>(T value, string message = "Value must be null.") where T : class
            => Debug.Assert(value == null, message);

        [Conditional(DEBUG)]
        public static void True(bool condition, string message = "Value must be true.")
            => Debug.Assert(condition, message);

        [Pure]
        private static bool IsEqual<T>(T lhs, T rhs) where T : class
            => lhs == null ? rhs == null : lhs.Equals(rhs);
    }
}
