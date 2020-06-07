using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source.HeaderWriters
{
    interface IHeaderWriter
    {
        public void WriteHeaders(ShaderOutputContext ctx, string filename);
    }
}
