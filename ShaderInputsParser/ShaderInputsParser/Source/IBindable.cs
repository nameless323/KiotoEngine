using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    interface IBindable
    {
        public BindpointDesc Bindpoint { get; set; }
    }
}
