// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Threading
{
    using System.Threading.Tasks;

    public static class TaskExtensions
    {
        /// <summary>
        /// Consumes a task and doesn't do anything with it. Useful for fire-and-forget calls to
        /// asynchronous methods within asynchronous methods.
        /// </summary>
        /// <param name="task">The task to forget.</param>
        public static void Forget(this Task task)
        {
        }
    }
}
