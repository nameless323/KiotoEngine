using Antlr4.Runtime;
using antlrGenerated;
using ShaderInputsParserApp.Source;
using ShaderInputsParserApp.Source.Types;
using System;
using System.IO;
using System.Text;

namespace ShaderInputsParserApp
{
    class Program
    {
        static ShaderInputsParser InitializeAntlr(string content)
        {
            AntlrInputStream inputStream = new AntlrInputStream(content);
            ShaderInputsLexer lexer = new ShaderInputsLexer(inputStream);
            CommonTokenStream tokenStream = new CommonTokenStream(lexer);
            return new ShaderInputsParser(tokenStream);
        }

        static void Main(string[] args)
        {
            string filename = Environment.CurrentDirectory + "/Grammar/GrammarInput.txt";
            string content = File.ReadAllText(filename);

            ShaderInputsParser parser = InitializeAntlr(content);
            ShaderInputsParser.InputFileContext ctx = parser.inputFile();

            ShaderInputsVisitor visitor = new ShaderInputsVisitor();
            visitor.Visit(ctx);
            ShaderOutputContext outputCtx = visitor.OutputContext;

            Validator validator = new Validator();
            try
            {
                validator.Validate(outputCtx.ConstantBuffers);
            }
            catch (DuplicateNameException ex)
            {
                Console.WriteLine(ex.Message);
                throw;
            }

            BindpointManager bindpointManager = new BindpointManager();
            bindpointManager.AssignBindpoints(outputCtx);

            // Bindpoint manager stage
            //    calculate resource counts for inputlayouts
            //    calculate resource counts for inputGroups (possible to dump files)
            //    build actual bindings

            HeaderWriter writer = new HeaderWriter();
            writer.WriteHLSLHeaders(outputCtx);
            writer.WriteCPPHeaders(outputCtx);
        }
    }
}
