using System;

namespace ReaperParser
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Property)]
    internal class ReaperTag : Attribute
    {
        public string Tag { get; set; }
        public ReaperTag(string tag)
        {
            Tag = tag;
        }
        public override string ToString()
        {
            return Tag;
        }
    }
}
