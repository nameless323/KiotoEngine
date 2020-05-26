using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class BindpointManager
    {
        public BindpointManager()
        {
        }

        public void AssignBindpoints(List<ConstantBuffer> structs)
        {
            uint i = 0;
            foreach (var s in structs)
            {
                s.Bindpoint = new BindpointDesc(i++, 0);
            }
        }
    }
}
