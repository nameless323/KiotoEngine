using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    class ShadersBindingVisitor : ShaderInputsParserBaseVisitor<object>
    {
        public override object VisitShaderBind(ShaderInputsParser.ShaderBindContext context)
        {
            string type = context.SHADER_TYPE().ToString();
            string fun = context.NAME().ToString();
            ShaderBindings.Bindings.Add(new ShaderBinding { ShaderType = type, EntryPointName = fun });

            return base.VisitShaderBind(context);
        }

        public ShadersBinding ShaderBindings = new ShadersBinding();
    }

}
