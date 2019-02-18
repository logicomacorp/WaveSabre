using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

namespace ReaperParser.ReaperElements
{
    public class ReaperElement
    {
        public ReaperElement ParentElement { get; set; }
        public List<ReaperElement> ChildElements { get; set; }
        public string ElementName { get; set; }
        public List<string> Data { get; set; }

        public ReaperElement()
        {
            ChildElements = new List<ReaperElement>();
        }

        // override for customer parse of data
        public virtual void CompleteParse()
        {

        }

        // finds matching element
        public void AddElement(ReaperElement element)
        {
            if (element.GetType() == typeof(ReaperElement))
            {
                this.ChildElements.Add(element);
                return;
            }

            var props = this.GetType().GetProperties();
            foreach (var prop in props)
            {
                if (prop.PropertyType == element.GetType())
                {
                    prop.SetValue(this, element, null);
                    return;
                }
                else if (prop.PropertyType.IsGenericType && prop.PropertyType.GetGenericTypeDefinition() == typeof(List<>))
                {
                    if (prop.PropertyType.GetGenericArguments()[0] == element.GetType())
                    {
                        var list = prop.GetValue(this, null);
                        prop.PropertyType.GetMethod("Add").Invoke(list, new [] { element });
                        return;
                    }
                }
            }
        }

        public void AddProperty(string data)
        {
            var values = SplitData(data);
            if (values.Count > 1)
            {
                var name = values[0];
                var value = values[1];

                var props = this.GetType().GetProperties();
                foreach (var prop in props)
                {
                    var tag = (ReaperTag)prop.GetCustomAttributes(typeof(ReaperTag), false).FirstOrDefault();
                    if (tag != null && tag.Tag == name)
                    {
                        SetProperty(prop, value);
                    }
                }
            }
        }

        private List<string> SplitData(string data)
        {
            return Regex.Matches(data, @"[\""].+?[\""]|[^ ]+")
                .Cast<Match>()
                .Select(m => m.Value)
                .ToList();
        }

        // parse element properties from string
        public void Parse(string data)
        {
            var values = SplitData(data);

            ElementName = values[0];

            values.RemoveAt(0); // remove element name
            Data = values;

            if (this.GetType() != typeof(ReaperElement))
            {
                var props = this.GetType().GetProperties();

                for (var i = 0; i < values.Count; i++)
                {
                    if (props.Count() <= i)  // ran out of properties so quit
                    {
                        break;
                    }

                    SetProperty(props[i], values[i]);
                }
            }
        }

        private void SetProperty(PropertyInfo prop, object value)
        {
            var propTypeName = (Nullable.GetUnderlyingType(prop.PropertyType) ?? prop.PropertyType).Name;

            // Enum handling
            if (prop.PropertyType.BaseType == typeof(Enum))
            {
                try
                {
                    var result = Enum.ToObject(prop.PropertyType, Convert.ToInt32(value));
                    prop.SetValue(this, result, null);
                }
                catch (Exception e)
                {
                    Console.WriteLine("Can't parse enum {0}: value: {1}: error: {2}", propTypeName, value, e.Message);
                }
            }
            else
            {
                switch (propTypeName)
                {
                    case "String":
                        prop.SetValue(this, value, null);
                        break;
                    case "Int16":
                        prop.SetValue(this, Convert.ToInt16(value), null);
                        break;
                    case "Int32":
                        prop.SetValue(this, Convert.ToInt32(value), null);
                        break;
                    case "Int64":
                        prop.SetValue(this, Convert.ToInt64(value), null);
                        break;
                    case "Boolean":
                        prop.SetValue(this, value.ToString() == "0" ? false : true, null);
                        break;
                    case "DateTime":
                        prop.SetValue(this, Convert.ToDateTime(value), null);
                        break;
                    case "Decimal":
                        prop.SetValue(this, Convert.ToDecimal(value), null);
                        break;
                    case "Single":
                        prop.SetValue(this, Convert.ToSingle(value), null);
                        break;
                    case "Double":
                        prop.SetValue(this, Convert.ToDouble(value), null);
                        break;
                    default:
                        //Console.WriteLine(propTypeName);
                        break;
                }
            }
        }

    }
}
