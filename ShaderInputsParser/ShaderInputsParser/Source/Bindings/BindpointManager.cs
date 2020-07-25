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
        private Dictionary<BindpointType, Bindings> m_allBindings = new Dictionary<BindpointType, Bindings>();
        private const uint MaxBindpointRegistersInSpace = 256;

        public BindpointManager()
        {
            m_allBindings.Add(BindpointType.Buffer, new Bindings());
            m_allBindings.Add(BindpointType.Texture, new Bindings());
            m_allBindings.Add(BindpointType.Sampler, new Bindings());
            m_allBindings.Add(BindpointType.UAV, new Bindings());
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

        public void AssignBindpoints(ShaderOutputContext ctx)
        {
            AssignBindpointsFromAnnotations(ctx.ConstantBuffers);
            AutomaticallyAssignBindpoints(ctx.ConstantBuffers, m_allBindings[BindpointType.Buffer]);

            // [a_vorontcov] Root constants have the same bindpoints as constant buffers.
            AssignBindpointsFromAnnotations(ctx.RootConstants);
            AutomaticallyAssignBindpoints(ctx.RootConstants, m_allBindings[BindpointType.Buffer]);

            AssignBindpointsFromAnnotations(ctx.Textures);
            AutomaticallyAssignBindpoints(ctx.Textures, m_allBindings[BindpointType.Texture]);

            AssignBindpointsFromAnnotations(ctx.Samplers);
            AutomaticallyAssignBindpoints(ctx.Samplers, m_allBindings[BindpointType.Sampler]);
        }

        void ValidateAnnotation(BindpointDesc d, Bindings bindings)
        {
            List<uint> bindingsForSpace;
            if (bindings.TryGetValue(d.Space, out bindingsForSpace))
            {
                if (bindingsForSpace.Contains(d.Reg))
                    throw new DuplicateBindpointException("Bindpoint of type " + d.GetType() + " in duplicated in space " + d.Space + ", register " + d.Reg);
                bindingsForSpace.Add(d.Reg);
                return;
            }
            bindings.Add(d.Space, new List<uint> { d.Reg });
        }

        void AssignBindpointsFromAnnotations(IEnumerable<IBindable> bindableList)
        {
            foreach (var b in bindableList)
            {
                BindpointDesc d;
                if (CheckAnnotations(b, out d))
                {
                    b.Bindpoint = d;
                }
            }
        }

        void AutomaticallyAssignBindpoints(IEnumerable<IBindable> bindableList, Bindings bindings)
        {
            List<uint> regs;
            if (!bindings.TryGetValue(0, out regs))
            {
                regs = new List<uint>();
                bindings.Add(0, regs);
            }
            uint nextAutoAssignedBindpoint = 0;
            foreach (var b in bindableList)
            {
                if (b.Bindpoint != null)
                    continue;

                uint reg = 0;
                for (uint i = nextAutoAssignedBindpoint; i < MaxBindpointRegistersInSpace; ++i)
                {
                    if (regs.Contains(i))
                        continue;
                    reg = i;
                    nextAutoAssignedBindpoint = i + 1;
                    break;
                }
                regs.Add(reg);
                b.Bindpoint = new BindpointDesc(reg, 0);
            }
        }

        public bool CheckAnnotations(IBindable cb, out BindpointDesc outDesc)
        {
            uint register = 0;
            uint space = 0;
            outDesc = null;

            List<Annotation> annotations = cb.Annotations;
            if (annotations == null)
                return false;

            var bindToAnnotation = annotations.FirstOrDefault(annot => annot.Name == "bindTo");
            bool regParseSucess = false;
            if (bindToAnnotation.Value != null)
            {
                regParseSucess = uint.TryParse(bindToAnnotation.Value, out register);
            }

            if (!regParseSucess)
                return false;

            var spaceAnnotation = annotations.FirstOrDefault(annot => annot.Name == "space");
            if (spaceAnnotation.Value != null)
                uint.TryParse(spaceAnnotation.Value, out space);
            outDesc = new BindpointDesc(register, space);

            ValidateAnnotation(outDesc, m_allBindings[cb.ResourceBindpointType]);

            return true;
        }
    }
}
