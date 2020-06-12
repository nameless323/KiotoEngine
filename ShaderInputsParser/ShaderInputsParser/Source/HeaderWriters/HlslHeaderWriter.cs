using ShaderInputsParserApp.Source.HeaderWriters;
using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    using TemplateGroup = Antlr4.StringTemplate.TemplateGroup;
    using StringTemplate = Antlr4.StringTemplate.Template;
    class HlslHeadersWriter : IHeaderWriter
    {
        string WriteIncludes(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<string> includes = ctx.Includes;
            if (includes.Count == 0)
                return "";

            StringBuilder result = new StringBuilder();
            foreach (var include in includes)
            {
                StringTemplate includeTemplate = group.GetInstanceOf("include");
                includeTemplate.Add("path", include);
                result.Append(includeTemplate.Render() + '\n');
            }
            result.Append('\n');
            result.Append('\n');
            return result.ToString();
        }

        string WriteStructures(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<Structure> structures = ctx.Structures;
            if (structures.Count == 0)
                return "";

            StringBuilder result = new StringBuilder();

            result.Append("///////////////// STRUCTURES /////////////////// ");
            result.Append('\n');

            foreach (var structure in structures)
            {
                StringBuilder members = new StringBuilder();
                foreach (var member in structure.Members)
                {
                    StringTemplate memberTemplate = group.GetInstanceOf("var");
                    memberTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                    memberTemplate.Add("name", member.Name);
                    members.Append(memberTemplate.Render() + '\n');
                }
                StringTemplate structTemplate = group.GetInstanceOf("struct");
                structTemplate.Add("name", structure.Name);
                structTemplate.Add("members", members);
                result.Append(structTemplate.Render() + '\n' + '\n');
            }

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
                StringBuilder membersString = new StringBuilder();
                foreach (var member in constantBuffer.Members)
                {
                    StringTemplate varTemplate = group.GetInstanceOf("var");
                    varTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                    varTemplate.Add("name", member.Name);
                    membersString.Append(varTemplate.Render() + '\n');
                }
                StringTemplate cbufferTemplate = group.GetInstanceOf("cbuffer");
                cbufferTemplate.Add("name", constantBuffer.Name);
                cbufferTemplate.Add("members", membersString);
                cbufferTemplate.Add("reg", constantBuffer.Bindpoint.Reg);
                cbufferTemplate.Add("space", constantBuffer.Bindpoint.Space);
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
            return result.ToString();
        }

        public void WriteHeaders(ShaderOutputContext ctx, string filename)
        {
            TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/hlslTemplate.stg");
            StringBuilder result = new StringBuilder();
            result.Append("//////////////////////////////////////////////// \n");
            result.Append("////////// AUTOGENERATED FILE, DO NOT EDIT !//// \n");
            result.Append("//////////////////////////////////////////////// \n");

            result.Append(WriteIncludes(ctx, group));
            result.Append(WriteStructures(ctx, group));
            result.Append(WriteConstantBuffers(ctx, group));
            result.Append(WriteTextures(ctx, group));
            result.Append(WriteSamplers(ctx, group));

            string outDirHlsl = Program.HlslOutputDir;
            string filenameOut = outDirHlsl + filename + ".hlsl";
            File.WriteAllText(filenameOut, result.ToString());
        }
    }
}
