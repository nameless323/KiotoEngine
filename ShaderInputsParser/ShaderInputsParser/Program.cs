using Antlr4.Runtime;
using antlrGenerated;
using ShaderInputsParserApp.Source;
using ShaderInputsParserApp.Source.HeaderWriters;
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

        static void CreateOutputDirectories()
        {
            Directory.CreateDirectory(OutputDir);
            Directory.CreateDirectory(OutputDir + "/hlsl");
            Directory.CreateDirectory(OutputDir + "/cpp");
        }

        static void Main(string[] args)
        {
            ParseCommandLine(args);
            try
            {
                if (InputDir == null)
                    throw new InvalidCommandLineException("Input directory isn't set in the command line");
                if (!Directory.Exists(InputDir))
                    throw new InvalidCommandLineException("Input directory doesn't exist");

                if (OutputDir == null)
                    throw new InvalidCommandLineException("Output directory isn't set in the command line");
            }
            catch (InvalidCommandLineException ex)
            {
                Console.WriteLine(ex.Message);
            }

            CreateOutputDirectories();

            string[] files = Directory.GetFiles(InputDir, "*.sinp", SearchOption.AllDirectories);

            foreach (var filepath in files)
            {
                string content = File.ReadAllText(filepath);

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

                string filename = Path.GetFileNameWithoutExtension(filepath);
                HlslHeadersWriter hlslWriter = new HlslHeadersWriter();
                hlslWriter.WriteHeaders(outputCtx, filename);
                CppHeaderWriter cppWriter = new CppHeaderWriter();
                cppWriter.WriteHeaders(outputCtx, filename);
            }
        }
    }
}
