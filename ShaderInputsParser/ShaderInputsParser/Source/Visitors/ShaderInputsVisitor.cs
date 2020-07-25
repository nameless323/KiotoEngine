using Antlr4.Runtime.Misc;
using antlrGenerated;
using ShaderInputsParserApp.Source.Types;
using ShaderInputsParserApp.Source.Visitors;
using System;
using System.Collections.Generic;
using System.IO;
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
        public List<ConstantBuffer> ConstantBuffers { get; set; } = new List<ConstantBuffer>();
        public List<RootConstant> RootConstants { get; set; } = new List<RootConstant>();
        public List<Texture> Textures { get; set; } = new List<Texture>();
        public List<Sampler> Samplers { get; set; } = new List<Sampler>();
        public VertexLayout VertLayout { get; set; } = null;
        public ShadersBinding ShaderBinding { get; set; } = null;

        public void Merge(ShaderOutputContext other)
        {
            Structures.AddRange(other.Structures);
            ConstantBuffers.AddRange(other.ConstantBuffers);
            Textures.AddRange(other.Textures);
            Samplers.AddRange(other.Samplers);
            RootConstants.AddRange(other.RootConstants);
            if (VertLayout != null && other.VertLayout != null)
                throw new DuplicateBindpointException("Vertex layout is defined twice");
            if (VertLayout == null)
                VertLayout = other.VertLayout;
            if (ShaderBinding != null && other.ShaderBinding != null)
                throw new DuplicateBindpointException("Shader bindings are defined twice");
            if (ShaderBinding == null)
                ShaderBinding = other.ShaderBinding;
        }
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
        public override string VisitCbufferTempl(ShaderInputsParser.CbufferTemplContext context)
        {
            // In our grammar the cb name goes last whilst the typename first (if exist), if not, there is only one name, typename is a general type.
            int cbufferNamePos = context.NAME().Length - 1;
            string name = context.NAME()[cbufferNamePos].GetText();
            string typename = "";
            if (context.TYPE() != null)
                typename = context.TYPE().GetText();
            else if (context.NAME()[0] != null)
                typename = context.NAME()[0].GetText();
            ArrayDimVisitor arrayVisiotr = new ArrayDimVisitor();
            arrayVisiotr.Visit(context);

            AnnotationsVisitor annotVisitor = new AnnotationsVisitor();
            annotVisitor.Visit(context);

            ConstantBuffer buffer = new ConstantBuffer(name, typename);
            buffer.Annotations = new List<Annotation>(annotVisitor.Annotations);
            buffer.Size = arrayVisiotr.Size;
            OutputContext.ConstantBuffers.Add(buffer);

            return name;
        }
        public override string VisitRootConstant(ShaderInputsParser.RootConstantContext context)
        {
            AnnotationsVisitor annotVisitor = new AnnotationsVisitor();
            annotVisitor.Visit(context);

            RootConstant constant = new RootConstant(context.TYPE().GetText(), context.NAME().GetText());
            constant.Annotations = new List<Annotation>(annotVisitor.Annotations);
            OutputContext.RootConstants.Add(constant);
            return "";
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
            string filename = context.FILEPATH().GetText();
            string filepath = Program.InputDir + "/" + filename;
            if (!File.Exists(filepath))
                throw new Exception("Include file " + filepath + " doesn't exist");

            string includeContent = File.ReadAllText(filepath);

            ShaderInputsParser parser = Program.InitializeAntlr(includeContent);
            ShaderInputsParser.InputFileContext ictx = parser.inputFile();

            ShaderInputsVisitor visitor = new ShaderInputsVisitor();
            visitor.Visit(ictx);

            OutputContext.Merge(visitor.OutputContext);

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
