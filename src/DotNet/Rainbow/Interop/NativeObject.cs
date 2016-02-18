// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Interop
{
    using System;
    using System.Runtime.InteropServices;

    using Diagnostics;

    public abstract class NativeObject<TManaged> : IDisposable
    {
        ~NativeObject()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(false);
        }

        /// <summary>
        /// Handle for the unmanaged object on which all operations are done.
        /// </summary>
        public IntPtr NativeHandle { get; protected set; } = IntPtr.Zero;

        /// <summary>
        /// Delegate for deleting the unmanaged instances of <see cref="TManaged"/>.
        /// </summary>
        private static ActionP Delete { get; set; }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        internal static T InitializeComponents<T>(IntPtr structure)
            where T : class, ITable
        {
            Assert.Null(Delete, $"{typeof(TManaged)} was already initialized");

            var table = Marshal.PtrToStructure<T>(structure);

            Assert.NotNull(table);
            Assert.Equal(table.SizeOfStruct, Marshal.SizeOf(table));

            var dtor = Delete;
            GetDelegateForFunctionPointer(table.DtorFunc, out dtor);
            Delete = dtor;

            return table;
        }

        /// <summary>
        /// Converts an unmanaged function pointer to a delegate.
        /// </summary>
        /// <param name="ptr">
        /// The unmanaged function pointer to be converted.
        /// </param>
        /// <param name="instance">
        /// The delegate to store the converted unmanaged function pointer.
        /// </param>
        /// <typeparam name="TDelegate">
        /// The delegate type to convert the unmanaged function pointer to.
        /// </typeparam>
        protected static void GetDelegateForFunctionPointer<TDelegate>(
            IntPtr ptr,
            out TDelegate instance)
        {
            Assert.NotNull(ptr);
            instance = Marshal.GetDelegateForFunctionPointer<TDelegate>(ptr);
        }

        protected void SetValue<T>(ref T field, T value, MemberActionP setValue)
            where T : NativeObject<T>
        {
            Assert.NotNull(value);

            if (field == value)
            {
                return;
            }

            field = value;
            setValue(NativeHandle, value.NativeHandle);
        }

        /// <summary>
        /// Called when managed state (managed objects) should be disposed.
        /// </summary>
        protected virtual void OnDisposing()
        {
        }

        private void Dispose(bool disposing)
        {
            if (NativeHandle != IntPtr.Zero)
            {
                if (disposing)
                {
                    OnDisposing();
                }

                Delete(NativeHandle);
                NativeHandle = IntPtr.Zero;
            }
        }
    }
}
