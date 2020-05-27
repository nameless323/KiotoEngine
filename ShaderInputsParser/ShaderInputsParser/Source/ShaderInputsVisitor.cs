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
        public List<Texture> Textures { get; set; } = new List<Texture>();
        public List<Sampler> Samplers { get; set; } = new List<Sampler>();

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
            Texture t = new Texture(name);
            OutputContext.Textures.Add(t);
            return base.VisitTex2d(context);
        }
        public override string VisitInclude(ShaderInputsParser.IncludeContext context)
        {
            string path = context.FILEPATH().GetText();
            OutputContext.Includes.Add(path);
            return base.VisitInclude(context);
        }
        public override string VisitSampler(ShaderInputsParser.SamplerContext context)
        {
            string name = context.NAME().GetText();
            Sampler s = new Sampler(name);
            OutputContext.Samplers.Add(s);
            return base.VisitSampler(context);
        }
        public ShaderOutputContext OutputContext { get; private set; } = new ShaderOutputContext();
    }
}
