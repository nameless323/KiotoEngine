using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    using Bindings = Dictionary<uint, List<uint>>;

    public class DuplicateBindpointException : System.Exception
    {
        public DuplicateBindpointException() : base() { }
        public DuplicateBindpointException(string message) : base(message) { }
        public DuplicateBindpointException(string message, System.Exception inner) : base(message, inner) { }
    }

    class BindpointManager
    {
        private Dictionary<BindpointType, Bindings> m_bindings = new Dictionary<BindpointType, Bindings>();
        private const uint MaxBindpointRegistersInSpace = 256;
        private uint m_nextAutoAssignedBindpoint = 0;

        public BindpointManager()
        {
            m_bindings.Add(BindpointType.Buffer, new Bindings());
            m_bindings.Add(BindpointType.Texture, new Bindings());
            m_bindings.Add(BindpointType.Sampler, new Bindings());
            m_bindings.Add(BindpointType.UAV, new Bindings());
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
            AssignBindpointsFromAnnotations(structs);
            AutomaticallyAssignBindpoints(structs);
        }

        void ValidateAnnotation(BindpointDesc d)
        {
            List<uint> bindings;
            if (m_bindings.TryGetValue(d.Space, out bindings))
            {
                if (bindings.Contains(d.Reg))
                    throw new DuplicateBindpointException("Bindpoint of type " + d.GetType() + " in duplicated in space " + d.Space + ", register " + d.Reg);
                bindings.Add(d.Reg);
            }
            m_bindings.Add(d.Space, new List<uint> { d.Reg });
        }

        void AssignBindpointsFromAnnotations(IEnumerable<IBindable> structs)
        {
            foreach (var s in structs)
            {
                BindpointDesc d = new BindpointDesc();
                if (CheckAnnotations(s, ref d))
                {
                    s.Bindpoint = d;
                }
            }
        }

        void AutomaticallyAssignBindpoints(IEnumerable<IBindable> structs)
        {
            List<uint> regs;
            if (!m_bindings.TryGetValue(0, out regs))
            {
                regs = new List<uint> { 0 };
                m_bindings.Add(0, regs);
            }
            foreach (var s in structs)
            {
                uint reg = 0;
                for (uint i = m_nextAutoAssignedBindpoint; i < MaxBindpointRegistersInSpace; ++i)
                {
                    if (regs.Contains(i))
                        continue;
                    reg = i;
                    m_nextAutoAssignedBindpoint = i + 1;
                }
                s.Bindpoint = new BindpointDesc(reg, 0);
            }
        }

        public bool CheckAnnotations(IBindable cb, ref BindpointDesc outDesc)
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
            outDesc = new BindpointDesc(register, space);

            ValidateAnnotation(outDesc);

            return true;
        }
    }
}
