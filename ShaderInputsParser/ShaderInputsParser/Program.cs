using Antlr4.Runtime;
using antlrGenerated;
using ShaderInputsParserApp.Source;
using System;
using System.IO;

namespace ShaderInputsParserApp
{
    class Program
    {
        static void Main(string[] args)
        {
            string filename = Environment.CurrentDirectory + "/Grammar/GrammarInput.txt";
            string content = File.ReadAllText(filename);

            AntlrInputStream inputStream = new AntlrInputStream(content);
            ShaderInputsLexer lexer = new ShaderInputsLexer(inputStream);
            CommonTokenStream tokenStream = new CommonTokenStream(lexer);
            ShaderInputsParser parser = new ShaderInputsParser(tokenStream);

            ShaderInputsParser.InputFileContext ctx = parser.inputFile();

            ShaderInputsVisitor visitor = new ShaderInputsVisitor();
            visitor.Visit(ctx);
            Console.WriteLine("Hello World!");
        }
    }
}
