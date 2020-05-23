using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace ShaderInputsParserApp.Source.Types
{
    class Structure
    {
        public Structure(string name, List<Variable> vars)
        {
            Name = name;
            Members = new List<Variable>(vars);
        }

        public string Name { get; }

        public List<Variable> Members { get; }

        public int Bindpoint { get; set; } = 0;
    }
}
