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
        public static string HlslOutputDir { get; private set; }
        public static string CppOutputDir { get; private set; }
        public static string TemplatesDir { get; private set; }
        public static ShaderInputsParser InitializeAntlr(string content)
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
                else if (args[i] == "hlslOutDir:")
                {
                    ++i;
                    HlslOutputDir = args[i];
                }
                else if (args[i] == "cppOutDir:")
                {
                    ++i;
                    CppOutputDir = args[i];
                }
                else if (args[i] == "templatesDir:")
                {
                    ++i;
                    TemplatesDir = args[i];
                }
            }
        }

        static void CleanDirectory(string path)
        {
            DirectoryInfo dir = new DirectoryInfo(path);

            foreach (FileInfo fi in dir.GetFiles())
            {
                fi.Delete();
            }

            foreach (DirectoryInfo di in dir.GetDirectories())
            {
                CleanDirectory(di.FullName);
                di.Delete();
            }
        }

        static void CreateOutputDirectories(bool deleteExisting)
        {
            if (deleteExisting)
            {
                CleanDirectory(HlslOutputDir);
                CleanDirectory(CppOutputDir);
            }
            Directory.CreateDirectory(HlslOutputDir);
            Directory.CreateDirectory(CppOutputDir);
            Directory.CreateDirectory(CppOutputDir + "/sInp/");
        }

        static void Main(string[] args)
        {
            ParseCommandLine(args);
            try
            {
                if (InputDir == null)
                    throw new InvalidCommandLineException("Input (inDir) directory isn't set in the command line");
                if (!Directory.Exists(InputDir))
                    throw new InvalidCommandLineException("Input directory doesn't exist (" + InputDir + ")");

                if (TemplatesDir == null)
                    throw new InvalidCommandLineException("Template (templatesDir) directory isn't set in the command line");
                if (!Directory.Exists(TemplatesDir))
                    throw new InvalidCommandLineException("Template directory doesn't exist (" + TemplatesDir + ")");

                if (HlslOutputDir == null)
                    throw new InvalidCommandLineException("Hlsl directory isn't set in the command line");

                if (CppOutputDir == null)
                    throw new InvalidCommandLineException("Cpp output directory isn't set in the command line");

            }
            catch (InvalidCommandLineException ex)
            {
                Console.WriteLine(ex.Message);
            }

            CreateOutputDirectories(true);

            string[] files = Directory.GetFiles(InputDir, "*.sinp", SearchOption.AllDirectories);

            foreach (var filepath in files)
            {
                try
                {
                    string content = File.ReadAllText(filepath);

                    ShaderInputsParser parser = InitializeAntlr(content);
                    ShaderInputsParser.InputFileContext ctx = parser.inputFile();

                    ShaderInputsVisitor visitor = new ShaderInputsVisitor();
                    visitor.Visit(ctx);
                    ShaderOutputContext outputCtx = visitor.OutputContext;

                    Validator validator = new Validator();

                    validator.Validate(outputCtx.ConstantBuffers);

                    BindpointManager bindpointManager = new BindpointManager();
                    bindpointManager.AssignBindpoints(outputCtx);

                    string filename = Path.GetFileNameWithoutExtension(filepath);
                    HlslHeadersWriter hlslWriter = new HlslHeadersWriter();
                    hlslWriter.WriteHeaders(outputCtx, filename);
                    CppHeaderWriter cppWriter = new CppHeaderWriter();
                    cppWriter.WriteHeaders(outputCtx, filename);
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Exception occurred while parsing file " + filepath);
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                    throw;
                }
            }
            FactoryWriter factoryWriter = new FactoryWriter();
            factoryWriter.WriteFactory(files);
        }
    }
}
