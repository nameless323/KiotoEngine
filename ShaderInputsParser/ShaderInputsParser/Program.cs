using Antlr4.Runtime;
using antlrGenerated;
using ShaderInputsParserApp.Source;
using ShaderInputsParserApp.Source.Types;
using System;
using System.IO;
using System.Text;

namespace ShaderInputsParserApp
{
    public class InvalidCommandLineException : System.Exception
    {
        public InvalidCommandLineException() : base() { }
        public InvalidCommandLineException(string message) : base(message) { }
        public InvalidCommandLineException(string message, System.Exception inner) : base(message, inner) { }
    }

    class Program
    {
        public static string InputDir { get; private set; }
        public static string OutputDir { get; private set; }
        static ShaderInputsParser InitializeAntlr(string content)
        {
            AntlrInputStream inputStream = new AntlrInputStream(content);
            ShaderInputsLexer lexer = new ShaderInputsLexer(inputStream);
            CommonTokenStream tokenStream = new CommonTokenStream(lexer);
            return new ShaderInputsParser(tokenStream);
        }

        static void ParseCommandLine(string[] args)
        {
            for (int i = 0; i < args.Length; ++i)
            {
                if (args[i] == "inDir:")
                {
                    ++i;
                    InputDir = args[i];
                }
                else if (args[i] == "outDir:")
                {
                    ++i;
                    OutputDir = args[i];
                }
            }
        }

        static void Main(string[] args)
        {
            ParseCommandLine(args);
            if (InputDir == null)
                throw new InvalidCommandLineException("Input directory isn't set in the command line");
            if (!System.IO.Directory.Exists(InputDir))
                throw new InvalidCommandLineException("Input directory doesn't exist");

            if (OutputDir == null)
                throw new InvalidCommandLineException("Output directory isn't set in the command line");

            System.IO.Directory.CreateDirectory(OutputDir);
            System.IO.Directory.CreateDirectory(OutputDir + "/hlsl");
            System.IO.Directory.CreateDirectory(OutputDir + "/cpp");

            string[] files = Directory.GetFiles(InputDir, "*.sinp", SearchOption.AllDirectories);

            foreach (var filename in files)
            {
                string currentInputFilename = Path.GetFileNameWithoutExtension(filename);
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

                HeaderWriter writer = new HeaderWriter(currentInputFilename);
                writer.WriteHLSLHeaders(outputCtx);
                writer.WriteCPPHeaders(outputCtx);
            }
        }
    }
}
