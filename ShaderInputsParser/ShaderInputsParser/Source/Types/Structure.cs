using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace ShaderInputsParserApp.Source.Types
{
    class Structure : IStructureType
    {
        public Structure(string name, List<Variable> members)
        {
            Typename = name;
            Members = new List<Variable>(members);
        }

        public string Typename { get; set; }

        public List<Variable> Members { get; set; }

        public int Bindpoint { get; set; } = 0;
    }
}
