using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ConstantBuffer : IStructureType, IBindable
    {
        public ConstantBuffer(string name, List<Variable> members)
        {
            Name = name;
            Members = new List<Variable>(members);
            IsTemplated = false;
        }

        public ConstantBuffer(string name, string typename)
        {
            Name = name;
            Typename = typename;
            IsTemplated = true;
        }

        public string Name { get; set; }
        public string Typename { get; set; }

        public bool IsTemplated { get; }

        public List<Variable> Members { get; set; }
        public List<Annotation> Annotations { get; set; }

        public BindpointDesc Bindpoint { get; set; }
        public BindpointType ResourceBindpointType { get { return BindpointType.Buffer; } set { } }
    }
}
