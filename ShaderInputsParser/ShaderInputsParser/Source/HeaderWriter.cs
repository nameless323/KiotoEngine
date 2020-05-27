using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class HeaderWriter
    {
        public HeaderWriter()
        {
        }

        public void WriteHeaders(ShaderOutputContext ctx)
        {
            Antlr4.StringTemplate.TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Environment.CurrentDirectory + "/Templates/hlslTemplate.stg");
            StringBuilder result = new StringBuilder();

            List<string> includes = ctx.Includes;
            foreach (var include in includes)
            {
                Antlr4.StringTemplate.Template inclTemplate = group.GetInstanceOf("include");
                inclTemplate.Add("path", include);
                result.Append(inclTemplate.Render() + '\n');
            }
            result.Append('\n');

            List<Structure> ss = ctx.Structures;
            foreach (var s in ss)
            {
                StringBuilder members = new StringBuilder();
                foreach (var member in s.Members)
                {
                    Antlr4.StringTemplate.Template varTemplate = group.GetInstanceOf("var");
                    varTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                    varTemplate.Add("name", member.Name);
                    members.Append(varTemplate.Render() + '\n');
                }
                Antlr4.StringTemplate.Template structTemplate = group.GetInstanceOf("struct");
                structTemplate.Add("name", s.Name);
                structTemplate.Add("members", members);
                result.Append(structTemplate.Render() + '\n' + '\n');
            }

            List<ConstantBuffer> constantBuffers = ctx.ConstantBuffers;

            foreach (var cbuffer in constantBuffers)
            {
                StringBuilder members = new StringBuilder();
                foreach (var member in cbuffer.Members)
                {
                    Antlr4.StringTemplate.Template varTemplate = group.GetInstanceOf("var");
                    varTemplate.Add("type", Variable.ConvertHlslType(member.Type));
                    varTemplate.Add("name", member.Name);
                    members.Append(varTemplate.Render() + '\n');
                }
                Antlr4.StringTemplate.Template cbufferTemplate = group.GetInstanceOf("cbuffer");
                cbufferTemplate.Add("name", cbuffer.Name);
                cbufferTemplate.Add("members", members);
                cbufferTemplate.Add("reg", cbuffer.Bindpoint.Reg);
                cbufferTemplate.Add("space", cbuffer.Bindpoint.Space);
                result.Append(cbufferTemplate.Render() + '\n' + '\n');
            }

            string filenameOut = "C:/Repos/KiotoEngine/Assets/autogen/hlsl/shaderparser.hlsl";
            System.IO.File.WriteAllText(filenameOut, result.ToString());
        }
    }
}
