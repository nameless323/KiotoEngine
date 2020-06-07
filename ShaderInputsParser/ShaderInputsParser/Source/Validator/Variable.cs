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

        public static string ConvertHlslType(string type)
        {
            return type;
        }
    }
}
