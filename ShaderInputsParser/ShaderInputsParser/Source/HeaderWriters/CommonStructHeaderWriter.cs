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
            TemplateGroup structGroup = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/cppTemplate.stg");
            TemplateGroup cppGroup = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/commonStructuresTemplate.stg");
            string structures = CppHeaderWriter.WriteStructures(structGroup, RemoveDuplicates(ctx.Structures));
            StringTemplate commonTemplate = cppGroup.GetInstanceOf("header");
            commonTemplate.Add("structs", structures);

            string filename = Program.CppOutputDir + "/CommonStructures.h";

            System.IO.File.WriteAllText(filename, commonTemplate.Render());
        }
    }
}
