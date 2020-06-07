using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    enum BindpointType
    {
        Buffer,
        Texture,
        Sampler,
        UAV
    };

    interface IBindable
    {
        public BindpointDesc Bindpoint { get; set; }
        public List<Annotation> Annotations { get; set; }
        public BindpointType ResourceBindpointType { get; set; }
    }
}
