﻿using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    public class DuplicateDefinedException : System.Exception
    {
        public DuplicateDefinedException() : base() { }
        public DuplicateDefinedException(string message) : base(message) { }
        public DuplicateDefinedException(string message, System.Exception inner) : base(message, inner) { }
    }
    class ShaderOutputContext
    {
        public List<Structure> Structures { get; set; } = new List<Structure>();
        public List<string> Includes { get; set; } = new List<string>();
        public List<ConstantBuffer> ConstantBuffers { get; set; } = new List<ConstantBuffer>();
        public List<Texture> Textures { get; set; } = new List<Texture>();
        public List<Sampler> Samplers { get; set; } = new List<Sampler>();
        public VertexLayout VertLayout { get; set; } = null;
        public ShadersBinding ShaderBinding { get; set; } = null;
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
        public override string VisitVertexLayout(ShaderInputsParser.VertexLayoutContext context)
        {
            if (OutputContext.VertLayout != null)
                throw new DuplicateDefinedException("VertexLayout defined more than one time in a shader input file");
            VertexLayoutVisitor layoutVisitor = new VertexLayoutVisitor();
            layoutVisitor.Visit(context);
            OutputContext.VertLayout = new VertexLayout(layoutVisitor.Members);
            return base.VisitVertexLayout(context);
        }

        public override string VisitShadersBinding(ShaderInputsParser.ShadersBindingContext context)
        {
            if (OutputContext.ShaderBinding != null)
                throw new DuplicateDefinedException("ShadersBinding defined more than one time in a shader input file");

            ShadersBindingVisitor sbVisitor = new ShadersBindingVisitor();
            sbVisitor.Visit(context);
            OutputContext.ShaderBinding = sbVisitor.ShaderBindings;
            return base.VisitShadersBinding(context);
        }
        public ShaderOutputContext OutputContext { get; private set; } = new ShaderOutputContext();
    }
}
