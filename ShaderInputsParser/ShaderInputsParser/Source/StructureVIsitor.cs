using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class StructureVisitor : ShaderInputsParserBaseVisitor<object>
    {
        public override object VisitVariable(ShaderInputsParser.VariableContext context)
        {
            string name = context.NAME().GetText();
            return base.VisitVariable(context);
        }
    }
}
