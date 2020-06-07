using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class Sampler : IBindable
    {
        public Sampler(string name)
        {
            Name = name;
        }

        public string Name { get; private set; }
        public BindpointDesc Bindpoint { get; set; }
        public List<Annotation> Annotations { get; set; }
        public BindpointType ResourceBindpointType { get { return BindpointType.Sampler; } set { } }
    }
}
