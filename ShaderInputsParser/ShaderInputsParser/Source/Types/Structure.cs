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

        public List<Annotation> Annotations
        {
            get
            {
                return m_annotations;
            }
            set
            {
                m_annotations = value;
                foreach (var a in m_annotations)
                {
                    if (a.Name == "common")
                    {
                        IsCommon = true;
                        return;
                    }
                }
            }
        }

        List<Annotation> m_annotations = new List<Annotation>();
        public bool IsCommon { get; set; } = false;
    }
}
