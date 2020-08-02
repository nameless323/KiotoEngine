using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.Visitors
{
    class MembersVisitor : ShaderInputsParserBaseVisitor<object>
    {
        public override object VisitVariable(ShaderInputsParser.VariableContext context)
        {
            string type;
            string name;
            if (context.TYPE() != null)
            {
                type = context.TYPE().GetText();
                name = context.NAME()[0].GetText();
            }
            else
            {
                type = context.NAME()[0].GetText();
                name = context.NAME()[1].GetText();
            }

            ArrayDimVisitor arrayVisiotr = new ArrayDimVisitor();
            arrayVisiotr.Visit(context);

            Variable var = new Variable(type, name, arrayVisiotr.Size);
            Members.Add(var);
            return base.VisitVariable(context);
        }

        public List<Variable> Members = new List<Variable>();
    }
}
