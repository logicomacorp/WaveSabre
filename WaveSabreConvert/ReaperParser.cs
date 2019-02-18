using ReaperParser.ReaperElements;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

namespace ReaperParser
{
    public class ReaperParser
    {
        private Dictionary<string, Type> _elements;

        public ReaperProject ParseProject(string fileName)
        {
            PopulateElements();

            var project = new ReaperProject();
            var lines = File.ReadAllLines(fileName);
            lines = lines.Select(l => l.TrimStart()).ToArray();

            ReaperElement current = null;

            foreach (var data in lines)
            {
                if (data.StartsWith("<"))  // new element and up a level
                {
                    if (current == null)
                    {
                        current = CreateElement(data, null);
                    }
                    else
                    {
                        var newElement = CreateElement(data, current);
                        current.AddElement(newElement);
                        current = newElement;
                    }
                }
                else if (data.StartsWith(">")) // down a level
                {
                    current.CompleteParse();
                    if (current.ParentElement != null)
                        current = current.ParentElement;
                }
                else if (data.StartsWith("WAK"))
                {
                    // this is a nasty hack as the file format gets weird around
                    // the effects chain
                    current.CompleteParse();
                    var currentType = current.ElementName;
                    current = current.ParentElement;
                    var newElement = CreateElement(currentType, current);
                    current.AddElement(newElement);
                    current = newElement;
                }
                else  // current level
                {
                    var newElement = CreateElement(data, current);
                    current.AddProperty(data);
                    current.AddElement(newElement);
                }
            }

            return (ReaperProject)current;
        }

        // create new element based on type and parse
        public ReaperElement CreateElement(string data, ReaperElement parent)
        {
            var name = GetElementName(data);

            Type thisType = null;
            ReaperElement newElement;
            _elements.TryGetValue(name, out thisType);
            if (thisType == null)
                newElement = new ReaperElement();
            else
                newElement = (ReaperElement)Activator.CreateInstance(thisType);

            newElement.ParentElement = parent;
            newElement.Parse(data);
            return newElement;
        }

        // gets clean element name
        private string GetElementName(string name)
        {
            name = name.Replace("<", "");
            name = name.Split(' ')[0];
            return name;
        }

        // generates dictionary of types and matching tag names
        private void PopulateElements()
        {
            _elements = new Dictionary<string, Type>();
            var typeList = Assembly.GetExecutingAssembly().GetTypes()
                      .Where(t => String.Equals(t.Namespace, "ReaperParser.ReaperElements", StringComparison.Ordinal))
                      .ToArray();

            foreach (Type t in typeList)
            {
                var tag = (ReaperTag)t.GetCustomAttributes(typeof(ReaperTag), false).FirstOrDefault();
                if (tag != null)
                    _elements.Add(tag.Tag, t);
            }
        }
    }
}
