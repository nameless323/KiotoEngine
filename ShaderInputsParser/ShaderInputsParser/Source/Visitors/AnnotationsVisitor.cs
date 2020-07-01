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
            AnnotationValueVisitor valueVisitor = new AnnotationValueVisitor();
            valueVisitor.Visit(context);
            Annotation annotation;
            annotation.Name = name;
            annotation.Value = valueVisitor.Value;
            Annotations.Add(annotation);
            return base.VisitAnnotationMember(context);
        }
        public List<Annotation> Annotations = new List<Annotation>();
    }

    class AnnotationValueVisitor : ShaderInputsParserBaseVisitor<string>
    {
        public override string VisitAnnotationValue(ShaderInputsParser.AnnotationValueContext context)
        {
            if (context.NAME() != null)
            {
                Value = context.NAME().ToString();
                return base.VisitAnnotationValue(context);
            }
            if (context.NUMBER() != null)
            {
                foreach (var n in context.NUMBER())
                    Value += n.ToString();
                return base.VisitAnnotationValue(context);
            }
            Value = "";
            return base.VisitAnnotationValue(context);
        }
        public string Value { get; set; }
    }

}
