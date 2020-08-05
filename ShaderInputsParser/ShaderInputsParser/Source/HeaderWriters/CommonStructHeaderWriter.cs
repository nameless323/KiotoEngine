using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.HeaderWriters
{
    using TemplateGroup = Antlr4.StringTemplate.TemplateGroup;
    using StringTemplate = Antlr4.StringTemplate.Template;
    class CommonStructHeaderWriter
    {
        List<Types.Structure> RemoveDuplicates(List<Types.Structure> origin)
        {
            List<Types.Structure> res = new List<Types.Structure>();
            HashSet<string> names = new HashSet<string>(StringComparer.Ordinal);
            foreach (var s in origin)
            {
                if (!names.Contains(s.Typename))
                {
                    res.Add(s);
                    names.Add(s.Typename);
                }
            }
            return res;
        }
        public void WriteHeaders(ShaderOutputGlobalContext ctx)
        {
            TemplateGroup structCppGroup = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/cppTemplate.stg");
            TemplateGroup commonGroup = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/commonStructuresTemplate.stg");
            var structWithoutDuplicates = RemoveDuplicates(ctx.Structures);
            string cppStructures = CppHeaderWriter.WriteStructures(structCppGroup, structWithoutDuplicates);
            StringTemplate commonTemplate = commonGroup.GetInstanceOf("cppHeader");
            commonTemplate.Add("structs", cppStructures);

            string cppFilename = Program.CppOutputDir + "/CommonStructures.h";

            System.IO.File.WriteAllText(cppFilename, commonTemplate.Render());

            TemplateGroup structHlslGroup = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/hlslTemplate.stg");
            string hlslStructures = HlslHeadersWriter.WriteStructures(structHlslGroup, structWithoutDuplicates);
            StringTemplate commonHlslTemplate = commonGroup.GetInstanceOf("hlslHeader");

            commonHlslTemplate.Add("structs", hlslStructures);

            string hlslFilename = Program.HlslOutputDir + "/CommonStructures.hlsl";
            System.IO.File.WriteAllText(hlslFilename, commonHlslTemplate.Render());
        }
    }
}
