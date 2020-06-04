using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class VertexLayoutMember : Variable
    {
        public VertexLayoutMember(string type, string name, string semantic) : base(type, name)
        {
            Semantic = semantic;
        }

        public string Semantic { get; set; }
    }

    class VertexLayout
    {
        public VertexLayout(List<VertexLayoutMember> members)
        {
            Members = new List<VertexLayoutMember>(members);
        }
        public List<VertexLayoutMember> Members { get; set; }
    }
}
