using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class BindpointDesc
    {
        public BindpointDesc(uint reg, uint space)
        {
            Reg = reg;
            Space = space;
        }
        public uint Reg { get; set; }
        public uint Space { get; set; }
    }
}
