// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
#if DEBUG
    using System;
    using System.Collections.Generic;
#endif
    using System.Diagnostics;

    /// <summary>A single node in a scene graph.</summary>
    public sealed partial class SceneNode
    {
        private const string DEBUG = @"DEBUG";

#if DEBUG
        List<SceneNode> children = new List<SceneNode>();
        SceneNode parent;
#endif

        /// <summary>
        /// Invalidates the node and all its children. For debugging purposes only.
        /// </summary>
        [Conditional(DEBUG)]
        private void Invalidate()
        {
#if DEBUG
            NativeHandle = IntPtr.Zero;
            foreach (var node in children)
            {
                node.Invalidate();
            }
#endif
        }

        [Conditional(DEBUG)]
        private void RemoveTrace()
        {
#if DEBUG
            parent.children.Remove(this);
#endif
        }

        /// <summary>
        /// Adds node to the list of children. For debugging purposes only.
        /// </summary>
        [Conditional(DEBUG)]
        private void TraceChildNode(SceneNode node)
        {
#if DEBUG
            children.Add(node);
            node.parent = this;
#endif
        }
    }
}
