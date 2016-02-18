// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Threading;

    using Diagnostics;
    using Interop;

    /// <summary>A single node in a scene graph.</summary>
    public sealed partial class SceneNode
    {
        private static SceneNode rootNode;

        private SceneNode(IntPtr ptr)
        {
            NativeHandle = ptr;
        }

        /// <summary>Returns the root node.</summary>
        public static SceneNode RootNode => rootNode;

        public bool IsEnabled
        {
            get { return isEnabled(NativeHandle); }
            set { setEnabled(NativeHandle, value); }
        }

        public string Tag { get; set; }

        /// <summary>Adds a child group node.</summary>
        /// <returns>The node that the group was added to.</returns>
        public SceneNode AddChild()
        {
            var node = new SceneNode(addChild(NativeHandle));
            TraceChildNode(node);
            return node;
        }

        /// <summary>Adds a child node.</summary>
        /// <param name="node">The node to be reparented.</param>
        /// <returns>The node that was reparented.</returns>
        public SceneNode AddChild(SceneNode node)
        {
            node.RemoveTrace();
            addChildNode(NativeHandle, node.NativeHandle);
            TraceChildNode(node);
            return node;
        }

        /// <summary>Adds a child node with an object of specified type.</summary>
        /// <param name="entity">The object to add to the tree.</param>
        /// <typeparam name="T">The type of the object to add.</typeparam>
        /// <returns>The node that the object was added to.</returns>
        public SceneNode AddChild<T>(T entity) where T : NativeObject<T>
        {
            if (entity is Animation)
            {
                return AddChildWith(entity, addChildAnimation);
            }

            if (entity is Label)
            {
                return AddChildWith(entity, addChildLabel);
            }

            if (entity is SpriteBatch)
            {
                return AddChildWith(entity, addChildSpritebatch);
            }

            throw new NotSupportedException($"SceneNode doesn't support entities of type {entity.GetType()}");
        }

        /// <summary>Recursively moves all sprites by (x,y).</summary>
        public void Move(float x, float y) => move(NativeHandle, new Vector2(x, y));

        /// <summary>Removes this node, and its children, from the scene graph.</summary>
        public void Remove()
        {
            Assert.True(NativeHandle != RootNode.NativeHandle, @"Don't remove the root node.");

            if (NativeHandle == IntPtr.Zero || NativeHandle == RootNode.NativeHandle)
            {
                return;
            }

            RemoveTrace();
            remove(NativeHandle);
            Invalidate();
        }

        internal static void SetRootNode(IntPtr ptr)
            => Interlocked.CompareExchange(ref rootNode, new SceneNode(ptr), null);

        private SceneNode AddChildWith<T>(T entity, MemberFuncPP func) where T : NativeObject<T>
        {
            var node = new SceneNode(func(NativeHandle, entity.NativeHandle));
            TraceChildNode(node);
            return node;
        }
    }
}
