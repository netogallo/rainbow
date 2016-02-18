// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;

    /// <content>
    /// Provides interop with C++ counterparts.
    /// </content>
    public static partial class InputManager
    {
        private static BitArray KeyboardState { get; } = new BitArray((int)VirtualKey.KeyCount);

        internal static void OnControllerAxisMotion(ControllerAxisMotion motion)
        {
            ControllerAxisMotion?.Invoke(null, motion);
        }

        internal static void OnControllerButtonDown(ControllerButtonEvent e)
        {
            ControllerButtonDown?.Invoke(null, e);
        }

        internal static void OnControllerButtonUp(ControllerButtonEvent e)
        {
            ControllerButtonUp?.Invoke(null, e);
        }

        internal static void OnControllerConnected(uint id)
        {
            ControllerConnected?.Invoke(null, id);
        }

        internal static void OnControllerDisconnected(uint id)
        {
            ControllerDisconnected?.Invoke(null, id);
        }

        internal static void OnKeyDown(KeyStroke keyStroke)
        {
            Console.WriteLine($"Key down: {keyStroke.Key.ToString()}");
            KeyboardState.Set(keyStroke.Key, true);
            KeyDown?.Invoke(null, keyStroke);
        }

        internal static void OnKeyUp(KeyStroke keyStroke)
        {
            Console.WriteLine($"Key up: {keyStroke.Key.ToString()}");
            KeyboardState.Set(keyStroke.Key, false);
            KeyUp?.Invoke(null, keyStroke);
        }

        internal static unsafe void OnPointerBegan(Pointer[] pointers, uint count)
        {
            Console.WriteLine($"Pointer began: {pointers[0].X},{pointers[0].Y}");
            PointerBegan?.Invoke(null, pointers.AsReadOnly(count));
        }

        internal static unsafe void OnPointerCanceled(Pointer[] pointers, uint count)
        {
            PointerCanceled?.Invoke(null, pointers.AsReadOnly(count));
        }

        internal static unsafe void OnPointerEnded(Pointer[] pointers, uint count)
        {
            Console.WriteLine($"Pointer ended: {pointers[0].X},{pointers[0].Y}");
            PointerEnded?.Invoke(null, pointers.AsReadOnly(count));
        }

        internal static unsafe void OnPointerMoved(Pointer[] pointers, uint count)
        {
            ////Console.WriteLine($"Pointer moved: {pointers[0].X},{pointers[0].Y}");
            PointerMoved?.Invoke(null, pointers.AsReadOnly(count));
        }

        [Pure]
        private static IReadOnlyList<T> AsReadOnly<T>(this T[] array, uint count)
        {
            return new ArraySegment<T>(array, 0, (int)count);
        }

        private static void Set(this BitArray bitArray, VirtualKey key, bool value)
        {
            bitArray[(int)key] = value;
        }
    }
}
