using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
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
            string type = context.TYPE().GetText();
            Variable var = new Variable(type, name);
            Variables.Add(var);
            return base.VisitVariable(context);
        }

        public List<Variable> Variables = new List<Variable>();
    }
}
