using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
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
            Structure structure = new Structure(name, visitor.Variables);
            Structures.Add(structure);

            return name;
        }
        public List<Structure> Structures = new List<Structure>();
    }
}
