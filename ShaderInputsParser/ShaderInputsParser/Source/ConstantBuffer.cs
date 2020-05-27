using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ConstantBuffer : IStructureType, IBindable
    {
        public ConstantBuffer(string name, List<Variable> members)
        {
            Name = name;
            Members = new List<Variable>(members);
        }

        public string Name { get; set; }

        public List<Variable> Members { get; set; }
        public List<Annotation> Annotations { get; set; }

        public BindpointDesc Bindpoint { get; set; }
    }
}
