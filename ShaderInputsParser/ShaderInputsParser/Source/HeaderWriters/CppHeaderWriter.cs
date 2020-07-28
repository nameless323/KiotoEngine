using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Linq;
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
                StringTemplate cBufferTemplate = group.GetInstanceOf("cbuffer");
                cBufferTemplate.Add("cbname", cb.Name);
                cBufferTemplate.Add("reg", cb.Bindpoint.Reg.ToString());
                cBufferTemplate.Add("space", cb.Bindpoint.Space.ToString());
                cBufferTemplate.Add("typename", cb.Typename);
                if (cb.Size > 1)
                    cBufferTemplate.Add("size", cb.Size);
                    res.Append(cBufferTemplate.Render() + '\n');
            }
            return res.ToString();
        }
        string WriteRootConstants(ShaderOutputContext ctx, TemplateGroup group)
        {
            StringBuilder res = new StringBuilder();
            List<UniformConstant> consts = ctx.UniformConstants;
            foreach (var c in consts)
            {
                StringTemplate cBufferTemplate = group.GetInstanceOf("uniformConstant");
                cBufferTemplate.Add("name", c.Name);
                cBufferTemplate.Add("reg", c.Bindpoint.Reg.ToString());
                cBufferTemplate.Add("space", c.Bindpoint.Space.ToString());
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

        string WriteProgramNames(ShaderOutputContext ctx, TemplateGroup group)
        {
            StringBuilder res = new StringBuilder();
            List<ShaderBinding> shaderBinds = ctx.ShaderBinding.Bindings;

            for (int i = 0; i < shaderBinds.Count; ++i)
            {
                StringTemplate nameTemplate = group.GetInstanceOf("programNamePair");
                nameTemplate.Add("shaderProg", m_shaderStagesToKioto[shaderBinds[i].ShaderType]);
                nameTemplate.Add("name", shaderBinds[i].EntryPointName);
                res.Append(nameTemplate.Render() + '\n');
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

        string WriteStructures(ShaderOutputContext ctx, TemplateGroup group, IEnumerable<IStructureType> structures)
        {
            if (structures.Count() == 0)
                return "";

            StringBuilder result = new StringBuilder();

            foreach (var structure in structures)
            {
                if (structure.Members == null) // [a_vorontcov] For example templated cb.
                    continue;

                StringBuilder members = new StringBuilder();
                foreach (var member in structure.Members)
                {
                    StringTemplate memberTemplate = group.GetInstanceOf("structMember");
                    memberTemplate.Add("type", Variable.ConvertCppType(member.Type));
                    memberTemplate.Add("name", member.Name);
                    members.Append(memberTemplate.Render() + '\n');
                }
                StringTemplate structTemplate = group.GetInstanceOf("struct");
                structTemplate.Add("name", structure.Typename);
                structTemplate.Add("members", members);
                result.Append(structTemplate.Render() + '\n');
            }

            return result.ToString();
        }

        public void WriteHeaders(ShaderOutputContext ctx, string filename)
        {
            TemplateGroup group = new Antlr4.StringTemplate.TemplateGroupFile(Program.TemplatesDir + "/cppTemplate.stg");

            string constantBuffers = WriteConstantBuffers(ctx, group);
            string constants = WriteRootConstants(ctx, group);
            string textureSets = WriteTextureSets(ctx, group);
            string bindings = WriteBindings(ctx, group);
            string vertexLayouts = WriteVertexLayouts(ctx, group);
            string programNames = WriteProgramNames(ctx, group);
            string structs = WriteStructures(ctx, group, ctx.Structures);
            structs += WriteStructures(ctx, group, ctx.ConstantBuffers);

            StringTemplate headerTemplate = group.GetInstanceOf("header");
            headerTemplate.Add("name", filename);
            headerTemplate.Add("structs", structs);
            headerTemplate.Add("cbuffers", constantBuffers);
            headerTemplate.Add("constants", constants);
            headerTemplate.Add("texSets", textureSets);
            headerTemplate.Add("shaderProgs", bindings);
            headerTemplate.Add("vertexLayout", vertexLayouts);
            headerTemplate.Add("shaderProgNames", programNames);
            headerTemplate.Add("shaderPath", "Shaders/" + filename + ".hlsl");

            string outDirHlsl = Program.CppOutputDir + "/sInp/";
            string filenameOut = outDirHlsl + filename + ".h";
            System.IO.File.WriteAllText(filenameOut, headerTemplate.Render());
        }
    }
}
