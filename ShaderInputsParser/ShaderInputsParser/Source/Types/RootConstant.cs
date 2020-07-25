using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.Types
{
    class RootConstant : IBindable
    {
        public RootConstant(string type, string name)
        {
            Type = type;
            Name = name;
        }
        public string Name { get; private set; }
        public string Type { get; private set; }
        public List<Annotation> Annotations { get; set; }

        public BindpointDesc Bindpoint { get; set; }
        public BindpointType ResourceBindpointType { get { return BindpointType.Buffer; } set { } }
    }
}
