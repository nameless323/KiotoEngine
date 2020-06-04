using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    struct ShaderBinding
    {
        public string ShaderType;
        public string EntryPointName;
    }
    class ShadersBinding
    {
        public List<ShaderBinding> Bindings { get; set; } = new List<ShaderBinding>();
    }
}
