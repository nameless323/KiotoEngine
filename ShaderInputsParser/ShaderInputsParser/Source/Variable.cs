using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.Types
{
    class Variable
    {
        public Variable(string type, string name)
        {
            Type = type;
            Name = name;
        }

        public string Type { get; }
        public string Name { get; }

        public static string ConvertType(string type)
        {
            if (type == "float4")
                return "vector4";
            if (type == "float4x4")
                return "matrix4";
            if (type == "float")
                return "float32";
            return "";
        }
    }
}
