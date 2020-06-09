using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ShaderInputsParserApp.Source.HeaderWriters
{
    using TemplateGroup = Antlr4.StringTemplate.TemplateGroup;
    using StringTemplate = Antlr4.StringTemplate.Template;
    class FactoryWriter
    {
        public void WriteFactory(string[] files)
        {
            TemplateGroup cppGroup = new Antlr4.StringTemplate.TemplateGroupFile(Environment.CurrentDirectory + "/Templates/factoryTemplateCpp.stg");

            StringBuilder mapping = new StringBuilder();
            foreach (var filepath in files)
            {
                string filenameWithoutExt = Path.GetFileNameWithoutExtension(filepath);
                StringTemplate mapTemplate = cppGroup.GetInstanceOf("shaderMap");
                mapTemplate.Add("key", filenameWithoutExt);
                mapping.Append(mapTemplate.Render() + '\n');
            }
            StringTemplate factoryCppTemplate = cppGroup.GetInstanceOf("factory");
            factoryCppTemplate.Add("mapping", mapping.ToString());

            string filename = Program.CppOutputDir + "/KiotoShaders";

            System.IO.File.WriteAllText(filename + ".cpp", factoryCppTemplate.Render());

            TemplateGroup hGroup = new Antlr4.StringTemplate.TemplateGroupFile(Environment.CurrentDirectory + "/Templates/factoryTemplateH.stg");
            StringTemplate factoryHTemplate = hGroup.GetInstanceOf("factory");
            System.IO.File.WriteAllText(filename + ".h", factoryHTemplate.Render());
        }
    }
}
