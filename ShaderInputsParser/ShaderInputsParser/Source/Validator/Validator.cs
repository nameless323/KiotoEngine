using ShaderInputsParserApp.Source.Types;
using System;
using System.Collections.Generic;
using System.Text;

namespace ShaderInputsParserApp.Source
{
    public class DuplicateNameException : System.Exception
    {
        public DuplicateNameException() : base() { }
        public DuplicateNameException(string message) : base(message) { }
        public DuplicateNameException(string message, System.Exception inner) : base(message, inner) { }
    }

    class Validator
    {
        public Validator()
        { }

        public void Validate(IEnumerable<IStructureType> structures)
        {
            foreach (var s in structures)
            {
                if (m_structNames.Contains(s.Typename))
                    throw new DuplicateNameException("Name " + s.Typename + " is duplicated.");
                m_structNames.Add(s.Typename);
                if (s.Members != null)
                    ValidateMembers(s);
            }
        }

        void ValidateMembers(IStructureType s)
        {
            HashSet<string> names = new HashSet<string>();
            foreach(var m in s.Members)
            {
                if (names.Contains(m.Name))
                    throw new DuplicateNameException("Type " + s.GetType() + " with a name " + s.Typename + " contains duplicate member with name " + m.Name);
                names.Add(m.Name);
            }
        }

        HashSet<string> m_structNames = new HashSet<string>();
    }
}
