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

        public void AssignBindpoints(List<Structure> structs)
        {
            int i = 0;
            foreach (var s in structs)
            {
                s.Bindpoint = i++;
            }
        }
    }
}
