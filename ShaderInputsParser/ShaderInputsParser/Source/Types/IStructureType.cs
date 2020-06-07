using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    interface IStructureType
    {
        public string Name { get; set; }
        public List<Variable> Members { get; set; }
    }
}
