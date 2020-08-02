using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.Types
{
    class Variable
    {
        private static Dictionary<string, string> s_toCppTypes = new Dictionary<string, string>()
        {
            { "float", "float32" },
            { "float2", "Vector2" },
            { "float3", "Vector3" },
            { "float4", "Vector4" },
            { "float3x3", "Matrix3" },
            { "float4x4", "Matrix4" },
            { "int", "int32" },
            { "uint", "uint32" }
        };

        public Variable(string type, string name, int dim)
        {
            Type = type;
            Name = name;
            Dimension = dim;
        }

        public string Type { get; }
        public string Name { get; }
        public int Dimension { get; } = 0;

        public static string ConvertHlslType(string type)
        {
            return type;
        }

        public static string ConvertCppType(string type)
        {
            if (s_toCppTypes.ContainsKey(type))
                return s_toCppTypes[type];
            return type;
        }
    }
}
