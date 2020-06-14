using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.HeaderWriters
{
    using TemplateGroup = Antlr4.StringTemplate.TemplateGroup;
    using StringTemplate = Antlr4.StringTemplate.Template;
    class CppHeaderWriter : IHeaderWriter
    {
        private Dictionary<string, string> m_typesToKiotoDefaults = new Dictionary<string, string>()
        {
            { "float", "0" },
            { "float2", "Vector2()" },
            { "float3", "Vector3()" },
            { "float4", "Vector4()" },
            { "float3x3", "Matrix3()" },
            { "float4x4", "Matrix4()" }
        };
        private Dictionary<string, string> m_shaderStagesToKioto = new Dictionary<string, string>()
        {
            { "VS", "ShaderProgramType::Vertex" },
            { "PS", "ShaderProgramType::Fragment" },
            { "CS", "ShaderProgramType::Compute" }
        };
        private Dictionary<string, string> m_typeToKiotoFormat = new Dictionary<string, string>()
        {
            { "float", "eDataFormat::R8" },
            { "float2", "eDataFormat::R8_G8" },
            { "float3", "eDataFormat::R8_G8_B8" },
            { "float4", "eDataFormat::R8_G8_B8_A8" },
        };
        private Dictionary<string, string> m_semanticToKiotoSemantic = new Dictionary<string, string>()
        {
            { "POSITION", "eVertexSemantic::Position" },
            { "NORMAL", "eVertexSemantic::Normal" },
            { "TEXCOORD", "eVertexSemantic::Texcoord" },
            { "COLOR", "eVertexSemantic::Color" }
        };

        string WriteConstantBuffers(ShaderOutputContext ctx, TemplateGroup group)
        {
            StringBuilder res = new StringBuilder();
            List<ConstantBuffer> cbs = ctx.ConstantBuffers;
            foreach (var cb in cbs)
            {
                StringBuilder membersResult = new StringBuilder();
                foreach (var m in cb.Members)
                {
                    StringTemplate cBufferMemberTemplate = group.GetInstanceOf("cbmember");
                    cBufferMemberTemplate.Add("cbname", cb.Name);
                    cBufferMemberTemplate.Add("memberName", m.Name);
                    cBufferMemberTemplate.Add("initVal", m_typesToKiotoDefaults[m.Type]);
                    membersResult.Append(cBufferMemberTemplate.Render() + "\n");
                }
                StringTemplate cBufferTemplate = group.GetInstanceOf("cbuffer");
                cBufferTemplate.Add("cbname", cb.Name);
                cBufferTemplate.Add("reg", cb.Bindpoint.Reg.ToString());
                cBufferTemplate.Add("space", cb.Bindpoint.Space.ToString());
                cBufferTemplate.Add("addParams", membersResult.ToString());
                res.Append(cBufferTemplate.Render() + '\n');
            }
            return res.ToString();
        }

        string WriteTextureSets(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<Texture> textures = ctx.Textures;

            StringBuilder res = new StringBuilder();
            foreach (var t in textures)
            {
                StringTemplate textureTemplate = group.GetInstanceOf("texture");
                textureTemplate.Add("name", t.Name);
                textureTemplate.Add("reg", t.Bindpoint.Reg);
                res.Append(textureTemplate.Render() + '\n');
            }
            StringTemplate textureSetTemplate = group.GetInstanceOf("texSet");
            textureSetTemplate.Add("addParams", res.ToString());

            return textureSetTemplate.Render();
        }

        string WriteBindings(ShaderOutputContext ctx, TemplateGroup group)
        {
            StringBuilder res = new StringBuilder();
            List<ShaderBinding> shaderBinds = ctx.ShaderBinding.Bindings;
            for (int i = 0; i < shaderBinds.Count; ++i)
            {
                res.Append("uint8(" + m_shaderStagesToKioto[shaderBinds[i].ShaderType] + ")");
                if (i != (shaderBinds.Count - 1))
                    res.Append(" | ");
            }
            return res.ToString();
        }

        string WriteVertexLayouts(ShaderOutputContext ctx, TemplateGroup group)
        {
            List<VertexLayoutMember> vertexLayoutMembers = ctx.VertLayout.Members;
            StringBuilder res = new StringBuilder();
            foreach (var vlm in vertexLayoutMembers)
            {
                StringTemplate vertexLayoutMemberTemplate = group.GetInstanceOf("vlayoutmember");
                vertexLayoutMemberTemplate.Add("semantic", m_semanticToKiotoSemantic[vlm.Semantic]);
                vertexLayoutMemberTemplate.Add("index", vlm.SemanticIndex);
                vertexLayoutMemberTemplate.Add("format", m_typeToKiotoFormat[vlm.Type]);
                res.Append(vertexLayoutMemberTemplate.Render() + '\n');
            }
            return res.ToString();
        }

        public void WriteHeaders(ShaderOutputContext ctx, string filename)
        {
            TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/cppTemplate.stg");

            string constantBuffers = WriteConstantBuffers(ctx, group);
            string textureSets = WriteTextureSets(ctx, group);
            string bindings = WriteBindings(ctx, group);
            string vertexLayouts = WriteVertexLayouts(ctx, group);
            string shaderCode = Program.ShadersDirManager.GetShaderCode(filename);

            StringTemplate headerTemplate = group.GetInstanceOf("header");
            headerTemplate.Add("name", filename);
            headerTemplate.Add("cbuffers", constantBuffers);
            headerTemplate.Add("texSets", textureSets);
            headerTemplate.Add("shaderProgs", bindings);
            headerTemplate.Add("vertexLayout", vertexLayouts);
            headerTemplate.Add("text", shaderCode);

            string outDirHlsl = Program.CppOutputDir + "/sInp/";
            string filenameOut = outDirHlsl + filename + ".h";
            System.IO.File.WriteAllText(filenameOut, headerTemplate.Render());
        }
    }
}
