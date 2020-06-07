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
            string semanticIndex = semantic[semantic.Length - 1].ToString();
            if (int.TryParse(semanticIndex, out int index))
                semantic = semantic.Remove(semantic.Length - 1, 1);

            VertexLayoutMember m = new VertexLayoutMember(type, name, semantic, index);
            Members.Add(m);
            return base.VisitVertexLayoutMember(context);
        }

        public List<VertexLayoutMember> Members = new List<VertexLayoutMember>();
    }
}
