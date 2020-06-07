using antlrGenerated;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    struct Annotation
    {
        public string Name;
        public string Value;
    }
    class AnnotationsVisitor : ShaderInputsParserBaseVisitor<string>
    {
        public override string VisitAnnotationMember(ShaderInputsParser.AnnotationMemberContext context)
        {
            string name = context.NAME().ToString();
            string value = context.VALUE()?.ToString() ?? "";
            Annotation annotation;
            annotation.Name = name;
            annotation.Value = value;
            Annotations.Add(annotation);
            return base.VisitAnnotationMember(context);
        }
        public List<Annotation> Annotations = new List<Annotation>();
    }
}
