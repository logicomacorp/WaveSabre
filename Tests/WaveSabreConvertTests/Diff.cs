using System;
using System.Collections.Generic;
using System.Linq;

namespace WaveSabreConvertTests
{
    public class Diff
    {
        readonly IDiffNode _root;

        public Diff(object a, object b)
        {
            if (a.GetType() != b.GetType())
                throw new InvalidOperationException("Can't diff objects of different types");

            _root = DiffNode(a, b, a.GetType());
        }

        internal static IDiffNode DiffNode(object a, object b, Type type)
        {
            if (type.IsValueType || type == typeof(string))
                return new ValueDiffNode(a, b);

            if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>))
            {
                // At this point, we know a and b are lists, but they could be lists of reference or value
                //  types, so just casting to something like IList<object> actually doesn't work. However,
                //  again, we know that they're lists - so the easiest thing to do is just rely on the DLR
                //  and quack our way to victory!
                dynamic aItems = a;
                dynamic bItems = b;
                var aCount = aItems.Count;
                var bCount = bItems.Count;

                if (aCount != bCount)
                {
                    return new IEnumerableDifferentCountsDiffNode(aCount, bCount);
                }
                else
                {
                    var items = new List<IEnumerableDiffNodeItem>();
                    for (int i = 0; i < aCount; i++)
                    {
                        var aItem = aItems[i];
                        var bItem = bItems[i];
                        items.Add(new IEnumerableDiffNodeItem(i, DiffNode(aItem, bItem, type.GenericTypeArguments.First())));
                    }
                    return new IEnumerableDiffNode(items);
                }
            }

            if (!type.IsGenericType)
                return new ObjectDiffNode(a, b, type);

            throw new NotImplementedException("Unsupported object type: " + type.Name);
        }

        public bool IsEmpty
        {
            get
            {
                return _root.IsEmpty;
            }
        }

        public void Print()
        {
            var p = new Printer();
            _root.Print(p);
        }
    }

    interface IDiffNode
    {
        bool IsEmpty { get; }
        void Print(Printer p);
    }

    class ObjectDiffNode : IDiffNode
    {
        class ObjectDiffNodeMember
        {
            public readonly string Name;
            public readonly IDiffNode Node;

            public ObjectDiffNodeMember(string name, IDiffNode node)
            {
                Name = name;
                Node = node;
            }
        }

        readonly List<ObjectDiffNodeMember> _members = new List<ObjectDiffNodeMember>();

        public ObjectDiffNode(object a, object b, Type type)
        {
            foreach (var field in type.GetFields().Where(f => f.IsPublic))
            {
                _members.Add(
                    new ObjectDiffNodeMember(
                        field.Name,
                        Diff.DiffNode(field.GetValue(a), field.GetValue(b), field.FieldType)));
            }
        }

        public bool IsEmpty
        {
            get {
                return _members.All(m => m.Node.IsEmpty);
            }
        }

        public void Print(Printer p)
        {
            foreach (var member in _members)
            {
                if (member.Node.IsEmpty)
                    continue;

                p.PrintLine(member.Name + ":");
                using (p.Scope())
                {
                    member.Node.Print(p);
                }
            }
        }
    }

    class ValueDiffNode : IDiffNode
    {
        readonly object _a, _b;

        public ValueDiffNode(object a, object b)
        {
            _a = a;
            _b = b;
        }

        public bool IsEmpty
        {
            get {
                return _a.Equals(_b);
            }
        }

        public void Print(Printer p)
        {
            p.PrintLine(_a + ", " + _b);
        }
    }

    class IEnumerableDifferentCountsDiffNode : IDiffNode
    {
        readonly int _aCount, _bCount;

        public IEnumerableDifferentCountsDiffNode(int aCount, int bCount)
        {
            _aCount = aCount;
            _bCount = bCount;
        }

        public bool IsEmpty
        {
            get {
                return false;
            }
        }

        public void Print(Printer p)
        {
            p.PrintLine("Counts differ: " + _aCount + ", " + _bCount);
        }
    }

    class IEnumerableDiffNode : IDiffNode
    {
        readonly List<IEnumerableDiffNodeItem> _items;

        public IEnumerableDiffNode(List<IEnumerableDiffNodeItem> items)
        {
            _items = items;
        }

        public bool IsEmpty
        {
            get {
                return _items.All(i => i.Node.IsEmpty);
            }
        }

        public void Print(Printer p)
        {
            p.PrintLine("- " + _items.Count + " item(s)");

            int diffCount = 0;
            for (int i = 0; i < _items.Count; i++)
            {
                int maxDiffCount = 10;
                if (diffCount > maxDiffCount)
                {
                    p.PrintLine("Different item count exceeds " + maxDiffCount + ", aborting");
                    break;
                }

                var item = _items[i];
                if (item.Node.IsEmpty)
                    continue;

                p.PrintLine("Index " + item.Index + ":");
                using (p.Scope())
                    item.Node.Print(p);
                diffCount++;
            }
        }
    }

    class IEnumerableDiffNodeItem
    {
        public readonly int Index;
        public readonly IDiffNode Node;

        public IEnumerableDiffNodeItem(int index, IDiffNode node)
        {
            Index = index;
            Node = node;
        }
    }
}
