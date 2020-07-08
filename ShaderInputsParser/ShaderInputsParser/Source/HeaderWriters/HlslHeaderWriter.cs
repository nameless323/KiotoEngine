using ShaderInputsParserApp.Source.HeaderWriters;
using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    using TemplateGroup = Antlr4.StringTemplate.TemplateGroup;
    using StringTemplate = Antlr4.StringTemplate.Template;
    class HlslHeadersWriter : IHeaderWriter
    {
        string WriteStructures(ShaderOutputContext ctx, TemplateGroup group, IEnumerable<IStructureType> structures)
        {
            if (structures.Count() == 0)
                return "";

            StringBuilder result = new StringBuilder();

            foreach (var structure in structures)
            {
                if (structure.Members == null) // [a_vorontcov] For example templated cb.
                    continue;

                StringBuilder members = new StringBuilder();
                foreach (var member in structure.Members)
                {
                    StringTemplate memberTemplate = group.GetInstanceOf("var");
                    memberTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                    memberTemplate.Add("name", member.Name);
                    members.Append(memberTemplate.Render() + '\n');
                }
                StringTemplate structTemplate = group.GetInstanceOf("struct");
                structTemplate.Add("name", structure.Typename);
                structTemplate.Add("members", members);
                result.Append(structTemplate.Render() + '\n' + '\n');
            }

            return result.ToString();
        }

        string WriteVertexLayout(ShaderOutputContext ctx, TemplateGroup group)
        {
            VertexLayout vLayout = ctx.VertLayout;

            StringBuilder result = new StringBuilder();

            result.Append("///////////////// VERTEX LAYOUT /////////////////// ");
            result.Append('\n');

            StringBuilder members = new StringBuilder();
            foreach (var member in vLayout.Members)
            {
                StringTemplate memberTemplate = group.GetInstanceOf("vlayoutvar");
                memberTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                memberTemplate.Add("name", member.Name);
                string index = "";
                if (member.Semantic == "TEXCOORD" || member.Semantic == "COLOR")
                    index = member.SemanticIndex.ToString();
                memberTemplate.Add("semantic", member.Semantic + index);
                members.Append(memberTemplate.Render() + '\n');
            }
            StringTemplate structTemplate = group.GetInstanceOf("struct");
            structTemplate.Add("name", "vIn");
            structTemplate.Add("members", members);
            result.Append(structTemplate.Render() + '\n' + '\n');

            result.Append('\n');
            result.Append('\n');

            return result.ToString();
        }

        string WriteConstantBuffers(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<ConstantBuffer> constantBuffers = ctx.ConstantBuffers;
            if (constantBuffers.Count == 0)
                return "";

            StringBuilder result = new StringBuilder();
            result.Append("///////////////// CONSTANT BUFFERS /////////////////// ");
            result.Append('\n');

            foreach (var constantBuffer in constantBuffers)
            {
                StringTemplate cbufferTemplate = group.GetInstanceOf("cbufferTempl");
                cbufferTemplate.Add("name", constantBuffer.Name);
                cbufferTemplate.Add("typename", constantBuffer.Typename);
                cbufferTemplate.Add("reg", constantBuffer.Bindpoint.Reg);
                cbufferTemplate.Add("space", constantBuffer.Bindpoint.Space);
                if (constantBuffer.Size != 0)
                    cbufferTemplate.Add("isArray", "true");
                if (constantBuffer.Size > 0)
                    cbufferTemplate.Add("size", constantBuffer.Size);
                result.Append(cbufferTemplate.Render() + '\n' + '\n');
            }

            result.Append('\n');
            result.Append('\n');

            return result.ToString();
        }

        string WriteTextures(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<Texture> textures = ctx.Textures;
            if (textures.Count == 0)
                return "";

            StringBuilder result = new StringBuilder();
            result.Append("///////////////// TEXTURES /////////////////// ");
            result.Append('\n');

            foreach (var texture in textures)
            {
                StringTemplate textureTemplate = group.GetInstanceOf("tex2d");
                textureTemplate.Add("name", texture.Name);
                textureTemplate.Add("reg", texture.Bindpoint.Reg);
                textureTemplate.Add("space", texture.Bindpoint.Space);
                result.Append(textureTemplate.Render() + '\n');
            }

            result.Append('\n');
            result.Append('\n');

            return result.ToString();
        }

        string WriteSamplers(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<Sampler> samplers = ctx.Samplers;
            if (samplers.Count == 0)
                return "";

            StringBuilder result = new StringBuilder();
            result.Append("///////////////// SAMPLERS /////////////////// ");
            result.Append('\n');

            foreach (var sampler in samplers)
            {
                StringTemplate samplerTemplate = group.GetInstanceOf("sampler");
                samplerTemplate.Add("name", sampler.Name);
                samplerTemplate.Add("reg", sampler.Bindpoint.Reg);
                samplerTemplate.Add("space", sampler.Bindpoint.Space);
                result.Append(samplerTemplate.Render() + '\n');
            }

            result.Append('\n');
            result.Append('\n');

            return result.ToString();
        }

        public void WriteHeaders(ShaderOutputContext ctx, string filename)
        {
            TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/hlslTemplate.stg");
            StringBuilder result = new StringBuilder();
            result.Append("//////////////////////////////////////////////// \n");
            result.Append("////////// AUTOGENERATED FILE, DO NOT EDIT !//// \n");
            result.Append("//////////////////////////////////////////////// \n");


            result.Append("///////////////// STRUCTURES /////////////////// ");
            result.Append('\n');

            result.Append(WriteStructures(ctx, group, ctx.Structures));
            result.Append(WriteStructures(ctx, group, ctx.ConstantBuffers));

            result.Append('\n');
            result.Append('\n');

            result.Append(WriteConstantBuffers(ctx, group));
            result.Append(WriteTextures(ctx, group));
            result.Append(WriteSamplers(ctx, group));
            result.Append(WriteVertexLayout(ctx, group));

            string outDirHlsl = Program.HlslOutputDir;
            string filenameOut = outDirHlsl + "/" + filename + ".hlsl";
            File.WriteAllText(filenameOut, result.ToString());
        }
    }
}
