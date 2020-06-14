using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ShadersDirectoryManager
    {
        public ShadersDirectoryManager(string shadersDir)
        {
            string[] files = Directory.GetFiles(shadersDir, "*.hlsl", SearchOption.AllDirectories);
            m_files = new string[files.Length];
            files.CopyTo(m_files, 0);
        }

        public string GetShaderCode(string filename)
        {
            foreach (var file in m_files)
            {
                if (Path.GetFileNameWithoutExtension(file).Equals(filename))
                    return File.ReadAllText(file);
            }
            throw new Exception("Shader with the name \"" + filename + "\" is not found.");
        }

        string[] m_files = null;
    }
}
