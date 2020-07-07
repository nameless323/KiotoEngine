using Antlr4.Runtime.Misc;
using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.Visitors
{
    class ArrayDimVisitor : ShaderInputsParserBaseVisitor<string>
    {
        public override string VisitArrayDimSpecifier(ShaderInputsParser.ArrayDimSpecifierContext context)
        {
            StringBuilder builder = new StringBuilder();
            Size = -1; // Unbound
            foreach (var n in context.NUMBER())
            {
                builder.Append(n);
            }
            if (builder.ToString().Length > 0)
                Size = int.Parse(builder.ToString());
            return base.VisitArrayDimSpecifier(context);
        }
        public int Size { get; set; } = 0;
    }
}
