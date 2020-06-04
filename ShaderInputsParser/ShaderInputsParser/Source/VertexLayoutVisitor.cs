using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class VertexLayoutVisitor : ShaderInputsParserBaseVisitor<object>
    {
        public override object VisitVertexLayoutMember(ShaderInputsParser.VertexLayoutMemberContext context)
        {
            string name = context.NAME().GetText();
            string type = context.TYPE().GetText();
            string semantic = context.V_SEMANTIC().GetText();
            VertexLayoutMember m = new VertexLayoutMember(type, name, semantic);
            Members.Add(m);
            return base.VisitVertexLayoutMember(context);
        }

        public List<VertexLayoutMember> Members = new List<VertexLayoutMember>();
    }
}
