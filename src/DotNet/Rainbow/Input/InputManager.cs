// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;

    /// <summary>
    /// Interface for input events and states.
    /// </summary>
    public static partial class InputManager
    {
        public static event EventHandler<ControllerAxisMotion> ControllerAxisMotion;

        public static event EventHandler<ControllerButtonEvent> ControllerButtonDown;

        public static event EventHandler<ControllerButtonEvent> ControllerButtonUp;

        public static event EventHandler<uint> ControllerConnected;

        public static event EventHandler<uint> ControllerDisconnected;

        public static event EventHandler<KeyStroke> KeyDown;

        public static event EventHandler<KeyStroke> KeyUp;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerBegan;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerCanceled;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerEnded;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerMoved;

        private static Dictionary<InputAction, InputActionTrigger> ActionDict { get; } =
            new Dictionary<InputAction, InputActionTrigger>();

        [Pure]
        public static bool IsDown(InputAction action)
        {
            InputActionTrigger trigger;
            return ActionDict.TryGetValue(action, out trigger) && IsDown(trigger.VirtualKey);
        }

        [Pure]
        public static bool IsDown(uint controller, ControllerButton button)
        {
            return false;
        }

        [Pure]
        public static bool IsDown(VirtualKey key) => KeyboardState[(int)key];

        public static void Map(InputAction action, VirtualKey key)
        {
            InputActionTrigger trigger;
            if (!ActionDict.TryGetValue(action, out trigger))
            {
                trigger = new InputActionTrigger();
            }
            trigger.VirtualKey = key;
            ActionDict[action] = trigger;
        }
    }
}
