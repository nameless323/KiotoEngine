using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ConstantBuffer
    {
        public ConstantBuffer(string name, List<Variable> members)
        {
            Name = name;
            Members = new List<Variable>(members);
        }

        public string Name { get; }

        public List<Variable> Members { get; }

        public BindpointDesc Bindpoint;
    }
}
