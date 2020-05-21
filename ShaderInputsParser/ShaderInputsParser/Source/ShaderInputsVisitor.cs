using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ShaderInputsVisitor : ShaderInputsParserBaseVisitor<string>
    {
        public override string VisitStruct(ShaderInputsParser.StructContext context)
        {
            string name = context.NAME().GetText();

            StructureVisitor visitor = new StructureVisitor();
            visitor.Visit(context);

            return name;
        }

    }
}
