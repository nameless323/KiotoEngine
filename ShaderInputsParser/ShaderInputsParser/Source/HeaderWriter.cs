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

        public void WriteHeaders(List<Structure> structs)
        {
            Antlr4.StringTemplate.TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Environment.CurrentDirectory + "/Templates/hlslTemplate.stg");
            StringBuilder result = new StringBuilder();
            foreach (var structure in structs)
            {
                StringBuilder members = new StringBuilder();
                members.Append('\n');
                foreach (var member in structure.Members)
                {
                    Antlr4.StringTemplate.Template varTemplate = group.GetInstanceOf("var");
                    varTemplate.Add("type", Variable.ConvertType(member.Type));
                    varTemplate.Add("name", member.Name);
                    members.Append(varTemplate.Render() + '\n');
                }
                Antlr4.StringTemplate.Template structTemplate = group.GetInstanceOf("struct");
                structTemplate.Add("structKeyword", "cbuffer");
                structTemplate.Add("name", structure.Name);
                structTemplate.Add("members", members);
                structTemplate.Add("bindpoint", structure.Bindpoint);
                structTemplate.Add("space", "0");
                result.Append(structTemplate.Render() + '\n' + '\n');
            }

            string filenameOut = "C:/Repos/KiotoEngine/Assets/autogen/hlsl/shaderparser.hlsl";
            System.IO.File.WriteAllText(filenameOut, result.ToString());
        }
    }
}
