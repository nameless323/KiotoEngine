using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ShaderOutputContext
    {
        public List<Structure> Structures { get; set; } = new List<Structure>();
        public List<string> Includes { get; set; } = new List<string>();
        public List<ConstantBuffer> ConstantBuffers { get; set; } = new List<ConstantBuffer>();

    }

    class ShaderInputsVisitor : ShaderInputsParserBaseVisitor<string>
    {
        public override string VisitStruct(ShaderInputsParser.StructContext context)
        {
            string name = context.NAME().GetText();

            MembersVisitor visitor = new MembersVisitor();
            visitor.Visit(context);
            Structure structure = new Structure(name, visitor.Members);
            OutputContext.Structures.Add(structure);

            return name;
        }
        public override string VisitCbuffer(ShaderInputsParser.CbufferContext context)
        {
            string name = context.NAME().GetText();

            MembersVisitor visitor = new MembersVisitor();
            visitor.Visit(context);

            AnnotationsVisitor annotVisitor = new AnnotationsVisitor();
            annotVisitor.Visit(context);

            ConstantBuffer buffer = new ConstantBuffer(name, visitor.Members);
            buffer.Annotations = new List<Annotation>(annotVisitor.Annotations);
            OutputContext.ConstantBuffers.Add(buffer);

            return name;
        }
        public override string VisitTex2d(ShaderInputsParser.Tex2dContext context)
        {
            string name = context.NAME().GetText();
            return base.VisitTex2d(context);
        }
        public ShaderOutputContext OutputContext { get; private set; } = new ShaderOutputContext();
    }
}
