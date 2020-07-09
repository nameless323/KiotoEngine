using Antlr4.Runtime;
using antlrGenerated;
using Microsoft.VisualBasic.CompilerServices;
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
            if (!Directory.Exists(path))
                return;

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

        static void UpdateOutputVersionFile(DateTime lastUpdateTime, string srcVerPath)
        {
            File.WriteAllText(srcVerPath, lastUpdateTime.ToString());
        }

        static bool CompareOutputsVersion(out DateTime lastUpdateTime, out string srcVerPath)
        {
            string[] files = Directory.GetFiles(InputDir, "*.sinp", SearchOption.AllDirectories);
            var srcModificationTime = Directory.GetLastWriteTime(InputDir);
            foreach (var filepath in files)
            {
                var currModifTime = File.GetLastWriteTime(filepath);
                if (DateTime.Compare(currModifTime, srcModificationTime) > 0)
                    srcModificationTime = currModifTime;
            }
            // [a_vorontcov] Trim ms, we don't need it.
            srcModificationTime = new DateTime(srcModificationTime.Year, srcModificationTime.Month, srcModificationTime.Day, srcModificationTime.Hour, srcModificationTime.Minute, srcModificationTime.Second);

            lastUpdateTime = srcModificationTime;

            srcVerPath = CppOutputDir + "/srcver";
            if (File.Exists(srcVerPath))
            {
                string lastModifTimeStr = File.ReadAllText(srcVerPath);
                var lastModifTime = DateTime.Parse(lastModifTimeStr);
                if (DateTime.Compare(srcModificationTime, lastModifTime) > 0)
                {
                    Console.WriteLine("Previous generated time " + lastModifTimeStr + " Current shader inputs modification time " + srcModificationTime.ToString() + "\nRegenerating output files...");
                    return false;
                }
                else
                {
                    Console.WriteLine("Previous generated time " + lastModifTimeStr + " Current shader inputs modification time " + srcModificationTime.ToString() + "\nNo regenaration needed. Shader inputs weren't changed");
                    return true;
                }
            }
            else
            {
                Console.WriteLine("Shader inputs version file is not found. Creating the file and regenerating output files...");
                return false;
            }
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

            CreateOutputDirectories(false);

            DateTime lastInputUpdateTime;
            string updateTimeFilepath;
            if (CompareOutputsVersion(out lastInputUpdateTime, out updateTimeFilepath))
                return;

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

                    //Validator validator = new Validator();
                    //validator.Validate(outputCtx.ConstantBuffers);

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

            UpdateOutputVersionFile(lastInputUpdateTime, updateTimeFilepath);
        }
    }
}
