using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    enum BindpointType
    {
        Buffer,
        Texture,
        Sampler,
        UAV
    };
    class BindpointManager
    {
        public BindpointManager()
        {
        }

        static public string GetBindpointPrefix(BindpointType t)
        {
            if (t == BindpointType.Buffer)
                return "c";
            if (t == BindpointType.Texture)
                return "t";
            if (t == BindpointType.Sampler)
                return "s";
            return "u"; // UAV
        }


        public void AssignBindpoints(IEnumerable<IBindable> structs)
        {
            uint i = 0;
            foreach (var s in structs)
            {
                bool foundBindpoint = CheckAnnotations(s);
                if (!foundBindpoint)
                    s.Bindpoint = new BindpointDesc(i++, 0);
            }
        }

        public bool CheckAnnotations(IBindable cb)
        {
            uint register = 0;
            uint space = 0;

            List<Annotation> annotations = cb.Annotations;
            var bindToAnnotation = annotations.FirstOrDefault(annot => annot.Name == "bindTo");
            if (bindToAnnotation.Value != null)
            {
                bool sucess = uint.TryParse(bindToAnnotation.Value, out register);
                if (!sucess)
                    return false;
            }
            var spaceAnnotation = annotations.FirstOrDefault(annot => annot.Name == "space");
            if (spaceAnnotation.Value != null)
                uint.TryParse(spaceAnnotation.Value, out space);
            cb.Bindpoint = new BindpointDesc(register, space);

            return true;
        }
    }
}
