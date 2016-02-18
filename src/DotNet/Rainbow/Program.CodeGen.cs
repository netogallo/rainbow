// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
#if CODEGEN
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Text.RegularExpressions;

    using Diagnostics;
    using Graphics;
    using Interop;

    /// <content>
    /// Contains code pertaining to C++ interop.
    /// </content>
    /// <remarks>
    /// To build the bindings generator, define <c>CODEGEN</c> and compile with
    /// <c>emitEntryPoint</c> set to <c>true</c>.
    /// </remarks>
    public sealed partial class Program
    {
        private const string InteropExtension = @".Interop.cs";
        private const string RainbowPath = @"Rainbow";

        private static char[] ArgsSeparators { get; } = { ',' };

        private static string InteropPath { get; } = Path.Combine(RainbowPath, @"Interop");

        private static Regex MethodRegex { get; } =
            new Regex(@"\/\*([\w*]+)\*\/ ([\w*]+) \(\*(\w+)\)\(([^)]*)", RegexOptions.Compiled);

        private static Encoding TextEncoding { get; } = new UTF8Encoding(false);

        private static Dictionary<string, TypeInfo> TypeDictionary { get; } =
            new Dictionary<string, TypeInfo>
            {
                ["Colorb"] = new TypeInfo { Type = typeof(Color), Name = "Color", Initial = "Cb" },
                ["Vec2f"] = new TypeInfo { Type = typeof(Vector2), Name = "Vector2", Initial = "V" },
                ["bool"] = new TypeInfo { Type = typeof(bool), Name = "bool", Initial = "B" },
                ["char*"] = new TypeInfo { Type = typeof(byte[]), Name = "byte[]", Initial = "C" },
                ["const char*"] = new TypeInfo { Type = typeof(byte[]), Name = "byte[]", Initial = "C" },
                ["const int32_t*"] = new TypeInfo { Type = typeof(int[]), Name = "int[]", Initial = "Ip" },
                ["const uint32_t*"] = new TypeInfo { Type = typeof(uint[]), Name = "uint[]", Initial = "Up" },
                ["float"] = new TypeInfo { Type = typeof(float), Name = "float", Initial = "F" },
                ["int32_t"] = new TypeInfo { Type = typeof(int), Name = "int", Initial = "I" },
                ["uint32_t"] = new TypeInfo { Type = typeof(uint), Name = "uint", Initial = "U" },
                ["void"] = new TypeInfo { Type = typeof(void), Name = "void" },
                ["void*"] = new TypeInfo { Type = typeof(IntPtr), Name = "IntPtr", Initial = "P" },
            };

        /// <summary>
        /// Generates C# bindings by reading C++ headers and performing naive regex matching.
        /// Requires strict formatting (use ClangFormat), and special names.
        /// </summary>
        public static void Main()
        {
            var delegateInfoDict = new SortedDictionary<string, DelegateInfo>();
            var functionList = new List<string>();
            var headers = new List<string>();
            var stringBuilder = new StringBuilder(8192);
            var tables = new List<string>();

            var files = Directory.EnumerateFiles(InteropPath, @"U*.h", SearchOption.AllDirectories);
            foreach (var filePath in files)
            {
                if (filePath.EndsWith(@"UMaster.h", StringComparison.Ordinal))
                {
                    continue;
                }

                Console.Write($"Processing {filePath}…");

                var tableName = Path.GetFileNameWithoutExtension(filePath);
                var className = tableName.Substring(1);
                var module = Path.GetDirectoryName(filePath).Replace(InteropPath, string.Empty);
                if (module.StartsWith(Path.DirectorySeparatorChar.ToString(), StringComparison.Ordinal))
                {
                    module = module.Substring(1);
                }

                var outputPath = Path.Combine(RainbowPath, module, className + InteropExtension);

                using (var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
                using (var streamReader = new StreamReader(fileStream))
                using (var interopStream = new FileStream(outputPath, FileMode.Create, FileAccess.Write))
                using (var streamWriter = new StreamWriter(interopStream, TextEncoding))
                {
                    WriteHeader(streamWriter, $"Rainbow.{module}");
                    streamWriter.WriteLine(@"    using System;");
                    streamWriter.WriteLine();
                    streamWriter.WriteLine(@"    using Interop;");
                    streamWriter.WriteLine();
                    streamWriter.WriteLine(@"    /// <content>");
                    streamWriter.WriteLine($"    /// Contains unmanaged delegates for <see cref=\"{className}\"/>.");
                    streamWriter.WriteLine(@"    /// </content>");
                    streamWriter.WriteLine($"    public sealed partial class {className} : NativeObject<{className}>");
                    streamWriter.WriteLine(@"    {");

                    int delegateCount = 0;
                    string line;
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        var match = MethodRegex.Match(line);
                        if (!match.Success || match.Groups.Count != 5)
                        {
                            continue;
                        }

                        string callingConvention = match.Groups[1].Value;
                        TypeInfo returnType = GetManagedType(match.Groups[2].Value);
                        string funcName = GetFunctionName(stringBuilder, match.Groups[3].Value);
                        TypeInfo[] args = GetManagedTypes(
                            match.Groups[4].Value.Split(ArgsSeparators, StringSplitOptions.RemoveEmptyEntries));

                        functionList.Add(funcName);

                        DelegateInfo delegateInfo;
                        var key = GetKey(stringBuilder, callingConvention, args, returnType);
                        if (!delegateInfoDict.TryGetValue(key, out delegateInfo))
                        {
                            delegateInfo = GetDelegateInfo(stringBuilder, callingConvention, args, returnType);
                            delegateInfoDict[key] = delegateInfo;
                        }

                        if (IsDestructor(funcName))
                        {
                            // Don't emit the destructor as it's handled in the base class.
                            // See |Rainbow.Interop.NativeObject<TManaged>|.
                            continue;
                        }

                        streamWriter.WriteLine($"        private static {delegateInfo} {funcName};");
                        ++delegateCount;
                    }

                    if (delegateCount > 0)
                    {
                        streamWriter.WriteLine();
                    }

                    streamWriter.WriteLine(@"        internal static void InitializeComponents(IntPtr ptr)");
                    streamWriter.WriteLine(@"        {");
                    streamWriter.WriteLine($"            var ftable = InitializeComponents<{tableName}>(ptr);");

                    foreach (string func in functionList)
                    {
                        if (IsDestructor(func))
                        {
                            // Don't emit the destructor as it's handled in the base class.
                            // See |Rainbow.Interop.NativeObject<TManaged>|.
                            continue;
                        }

                        streamWriter.WriteLine($"            GetDelegateForFunctionPointer(ftable.{func.ToPascalCase()}Func, out {func});");
                    }

                    CloseBraces(streamWriter, 3);
                }

                using (var fileStream = new FileStream(Path.Combine(InteropPath, tableName + @".cs"), FileMode.Create, FileAccess.Write))
                using (var streamWriter = new StreamWriter(fileStream, TextEncoding))
                {
                    WriteHeader(streamWriter, @"Rainbow.Interop", usingSystemModules: true, sequentialLayout: true);
                    streamWriter.WriteLine($"    internal sealed class {tableName} : ITable");
                    streamWriter.WriteLine(@"    {");
                    streamWriter.WriteLine(@"        private readonly uint sizeOfStruct;");

                    foreach (var func in functionList)
                    {
                        streamWriter.WriteLine($"        private readonly IntPtr {func}Func;");
                    }

                    streamWriter.WriteLine();
                    streamWriter.WriteLine($"        public uint SizeOfStruct => sizeOfStruct;");

                    foreach (var func in functionList)
                    {
                        streamWriter.WriteLine();
                        streamWriter.WriteLine($"        public IntPtr {func.ToPascalCase()}Func => {func}Func;");
                    }

                    CloseBraces(streamWriter, 2);
                }

                functionList.Clear();
                headers.Add(filePath);
                tables.Add(className);

                Console.WriteLine(@" done");
            }

            foreach (var kv in delegateInfoDict)
            {
                stringBuilder.AppendLine(kv.Value.Declaration);
            }

            stringBuilder.Remove(stringBuilder.Length - 1, 1);

            var unmanagedFunctionPointers = Path.Combine(InteropPath, "UnmanagedFunctionPointers.cs");
            using (var outStream = new FileStream(unmanagedFunctionPointers, FileMode.Create, FileAccess.Write))
            using (var fileWriter = new StreamWriter(outStream, new UTF8Encoding(false)))
            {
                Console.Write($"Generating {unmanagedFunctionPointers}…");

                WriteHeader(fileWriter, @"Rainbow.Interop", usingSystemModules: true);
                fileWriter.WriteLine(@"    using Graphics;");
                fileWriter.WriteLine();
                fileWriter.Write(stringBuilder.ToString());
                fileWriter.WriteLine(@"}");

                Console.WriteLine(@" done");
            }

            stringBuilder.Clear();

            var masterCs = Path.Combine(InteropPath, "UMaster.cs");
            using (var outStream = new FileStream(masterCs, FileMode.Create, FileAccess.Write))
            using (var fileWriter = new StreamWriter(outStream, new UTF8Encoding(false)))
            {
                Console.Write($"Generating {masterCs}…");

                WriteHeader(fileWriter, @"Rainbow.Interop", usingSystemModules: true, sequentialLayout: true);
                fileWriter.WriteLine(@"    internal sealed class UMaster");
                fileWriter.WriteLine(@"    {");
                fileWriter.WriteLine(@"        private readonly uint sizeOfStruct;");
                fileWriter.WriteLine(@"        private readonly IntPtr rootNode;");

                foreach (var t in tables)
                {
                    fileWriter.WriteLine($"        private readonly IntPtr {t.ToCamelCase()};");
                }

                fileWriter.WriteLine();
                fileWriter.WriteLine(@"        public uint SizeOfStruct => sizeOfStruct;");
                fileWriter.WriteLine();
                fileWriter.WriteLine(@"        public IntPtr RootNode => rootNode;");

                foreach (var t in tables)
                {
                    fileWriter.WriteLine();
                    fileWriter.WriteLine($"        public IntPtr {t} => {t.ToCamelCase()};");
                }

                CloseBraces(fileWriter, 2);
                Console.WriteLine(@" done");
            }

            var masterH = Path.Combine(InteropPath, "UMaster.h");
            using (var outStream = new FileStream(masterH, FileMode.Create, FileAccess.Write))
            using (var fileWriter = new StreamWriter(outStream, new UTF8Encoding(false)))
            {
                Console.Write($"Generating {masterH}…");

                WriteHeader(fileWriter);
                fileWriter.WriteLine(@"#ifndef DOTNET_RAINBOW_INTEROP_UMASTER_H_");
                fileWriter.WriteLine(@"#define DOTNET_RAINBOW_INTEROP_UMASTER_H_");
                fileWriter.WriteLine();

                headers.Sort();
                foreach (var header in headers)
                {
                    fileWriter.WriteLine($"#include \"DotNet/{header}\"");
                }

                fileWriter.WriteLine();
                fileWriter.WriteLine(@"namespace rainbow");
                fileWriter.WriteLine(@"{");
                fileWriter.WriteLine(@"    struct UMaster");
                fileWriter.WriteLine(@"    {");
                fileWriter.WriteLine(@"        uint32_t size_of_struct = sizeof(UMaster);");
                fileWriter.WriteLine(@"        SceneNode* root_node;");

                foreach (var t in tables)
                {
                    fileWriter.WriteLine($"        U{t}* {t.ToSnakeCase(stringBuilder)};");
                    stringBuilder.Clear();
                }

                fileWriter.WriteLine(@"    };");
                fileWriter.WriteLine();
                fileWriter.WriteLine(@"    inline void initialize_components(SceneNode& root_node,");
                fileWriter.WriteLine(@"                                      void (*initialize_components)(void*))");
                fileWriter.WriteLine(@"    {");

                foreach (var t in tables)
                {
                    fileWriter.WriteLine($"        U{t} {t.ToSnakeCase(stringBuilder)};");
                    stringBuilder.Clear();
                }

                fileWriter.WriteLine();
                fileWriter.WriteLine(@"        UMaster master;");
                fileWriter.WriteLine(@"        master.root_node = &root_node;");

                foreach (var t in tables)
                {
                    var field = t.ToSnakeCase(stringBuilder);
                    fileWriter.WriteLine($"        master.{field} = &{field};");
                    stringBuilder.Clear();
                }

                fileWriter.WriteLine();
                fileWriter.WriteLine(@"        initialize_components(&master);");
                fileWriter.WriteLine(@"    }");
                fileWriter.WriteLine(@"}");
                fileWriter.WriteLine();
                fileWriter.WriteLine(@"#endif");

                Console.WriteLine(@" done");
            }

            var runtimeCs = Path.Combine(RainbowPath, "Program.Runtime.cs");
            using (var outStream = new FileStream(runtimeCs, FileMode.Create, FileAccess.Write))
            using (var fileWriter = new StreamWriter(outStream, new UTF8Encoding(false)))
            {
                Console.Write($"Generating {runtimeCs}…");

                WriteHeader(fileWriter, @"Rainbow", usingSystemModules: true);
                fileWriter.WriteLine(@"    using Audio;");
                fileWriter.WriteLine(@"    using Diagnostics;");
                fileWriter.WriteLine(@"    using Graphics;");
                fileWriter.WriteLine(@"    using Interop;");
                fileWriter.WriteLine();
                fileWriter.WriteLine(@"    /// <content>");
                fileWriter.WriteLine(@"    /// Contains code pertaining to C++ interop.");
                fileWriter.WriteLine(@"    /// </content>");
                fileWriter.WriteLine(@"    public sealed partial class Program");
                fileWriter.WriteLine(@"    {");
                fileWriter.WriteLine(@"        /// <summary>");
                fileWriter.WriteLine(@"        /// Initializes all Rainbow components on startup.");
                fileWriter.WriteLine(@"        /// </summary>");
                fileWriter.WriteLine(@"        /// <param name=""masterTablePtr"">Pointer to the master table of functions.</param>");
                fileWriter.WriteLine(@"        internal static void InitializeComponents(IntPtr masterTablePtr)");
                fileWriter.WriteLine(@"        {");
                fileWriter.WriteLine(@"            var masterTable = Marshal.PtrToStructure<UMaster>(masterTablePtr);");
                fileWriter.WriteLine(@"            Assert.Equal(masterTable.SizeOfStruct, Marshal.SizeOf(masterTable));");
                fileWriter.WriteLine();

                foreach (var t in tables)
                {
                    fileWriter.WriteLine($"            {t}.InitializeComponents(masterTable.{t});");
                }

                fileWriter.WriteLine();
                fileWriter.WriteLine(@"            SceneNode.SetRootNode(masterTable.RootNode);");
                CloseBraces(fileWriter, 3);

                Console.WriteLine(@" done");
            }
        }

        private static void CloseBraces(StreamWriter stream, int count)
        {
            for (int i = 0; i < count; ++i)
            {
                for (int j = 1; j < count - i; ++j)
                {
                    stream.Write(@"    ");
                }

                stream.WriteLine(@"}");
            }
        }

        private static string CopyStringAndClear(StringBuilder stringBuilder)
        {
            string s = stringBuilder.ToString();
            stringBuilder.Clear();
            return s;
        }

        private static DelegateInfo GetDelegateInfo(
            StringBuilder stringBuilder,
            string callingConvention,
            TypeInfo[] args,
            TypeInfo returnType)
        {
            string delegateName = GetDelegateName(stringBuilder, callingConvention, args, returnType);
            stringBuilder.Append($"    public unsafe delegate {returnType} {delegateName}(");
            char identifier = 'a';
            foreach (TypeInfo type in args)
            {
                stringBuilder.Append($"{type.Name} {identifier}, ");
                ++identifier;
            }

            if (identifier != 'a')
            {
                stringBuilder.Remove(stringBuilder.Length - 2, 2);
            }

            stringBuilder.AppendLine(@");");

            return new DelegateInfo
                {
                    Declaration = CopyStringAndClear(stringBuilder),
                    Name = delegateName,
                };
        }

        private static string GetDelegateName(
            StringBuilder stringBuilder,
            string callingConvention,
            TypeInfo[] args,
            TypeInfo returnType)
        {
            bool isThisCall = callingConvention.Equals(@"ThisCall");
            if (isThisCall)
            {
                stringBuilder.Append(@"Member");
            }

            bool isAction = returnType.Type == typeof(void);
            stringBuilder.Append(isAction ? @"Action" : @"Func");
            for (int i = isThisCall ? 1 : 0; i < args.Length; ++i)
            {
                stringBuilder.Append(args[i].Initial);
            }

            if (!isAction)
            {
                stringBuilder.Append(returnType.Initial);
            }

            return CopyStringAndClear(stringBuilder);
        }

        private static string GetFunctionName(StringBuilder stringBuilder, string funcName)
        {
            Assert.True(
                funcName.EndsWith(@"_fn", StringComparison.Ordinal),
                $"Expected well-formed function pointer name, got {funcName}.");

            bool shouldCapitalize = false;
            foreach (char c in funcName)
            {
                if (c == '_')
                {
                    shouldCapitalize = true;
                }
                else if (shouldCapitalize)
                {
                    shouldCapitalize = false;
                    stringBuilder.Append(char.ToUpperInvariant(c));
                }
                else
                {
                    stringBuilder.Append(c);
                }
            }

            stringBuilder.Remove(stringBuilder.Length - 2, 2);
            return CopyStringAndClear(stringBuilder);
        }

        private static string GetKey(
            StringBuilder stringBuilder,
            string callingConvention,
            TypeInfo[] args,
            TypeInfo returnType)
        {
            stringBuilder.Append(callingConvention);
            stringBuilder.Append(returnType.Type == typeof(void) ? @":Action:" : @":Func:");
            foreach (TypeInfo info in args)
            {
                stringBuilder.Append($"{info},");
            }

            stringBuilder.Append(returnType);
            return CopyStringAndClear(stringBuilder);
        }

        [Pure]
        private static TypeInfo GetManagedType(string type)
        {
            type = type.Trim();
            if (TypeDictionary.ContainsKey(type))
            {
                return TypeDictionary[type];
            }

            Assert.True(type.EndsWith(@"*", StringComparison.Ordinal), $"Expected a pointer type, got {type}.");
            return TypeDictionary[@"void*"];
        }

        [Pure]
        private static TypeInfo[] GetManagedTypes(string[] types)
        {
            var info = new TypeInfo[types.Length];
            for (int i = 0; i < types.Length; ++i)
            {
                info[i] = GetManagedType(types[i]);
            }

            return info;
        }

        [Pure]
        private static bool IsDestructor(string func)
            => func.Equals(@"Dtor", StringComparison.OrdinalIgnoreCase);

        private static void WriteHeader(
            StreamWriter stream,
            string nameSpace = null,
            bool usingSystemModules = false,
            bool sequentialLayout = false)
        {
            int year = DateTime.Now.Year - 2000;
            stream.WriteLine(@"// This file was generated by Rainbow for .NET Core. Do not modify.");
            stream.WriteLine(@"//");
            stream.WriteLine($"// Copyright (c) 2010-{year} Bifrost Entertainment AS and Tommy Nguyen");
            stream.WriteLine(@"// Distributed under the MIT License.");
            stream.WriteLine(@"// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)");
            stream.WriteLine();

            if (!string.IsNullOrEmpty(nameSpace))
            {
                stream.WriteLine($"namespace {nameSpace}");
                stream.WriteLine(@"{");
                if (usingSystemModules)
                {
                    stream.WriteLine(@"    using System;");
                    stream.WriteLine(@"    using System.Runtime.InteropServices;");
                    stream.WriteLine();
                    if (sequentialLayout)
                    {
                        stream.WriteLine(@"    [StructLayout(LayoutKind.Sequential)]");
                    }
                }
            }
        }

        private struct DelegateInfo
        {
            public string Declaration { get; set; }

            public string Name { get; set; }

            public override string ToString()
            {
                return Name;
            }
        }

        private struct TypeInfo
        {
            public Type Type { get; set; }

            public string Name { get; set; }

            public string Initial { get; set; }

            public override string ToString()
            {
                return Name;
            }
        }
    }
#endif
}
