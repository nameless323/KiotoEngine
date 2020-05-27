using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class Texture : IBindable
    {
        public Texture(string name)
        {
            Name = name;
        }

        public string Name { get; private set; }
        public BindpointDesc Bindpoint { get; set; }
        public List<Annotation> Annotations { get; set; }
    }
}
