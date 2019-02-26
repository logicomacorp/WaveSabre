using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WaveSabreConvert
{
    static class Utils
    {
        const string encodingTable = ".ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+";

        public static byte[] Dejucer(string s)
        {
            var size = -1;
            if (!int.TryParse(s.Substring(0, s.IndexOf('.')), out size))
                return null;
            
            var b64str = s.Substring(s.IndexOf('.') + 1, s.Length - s.IndexOf('.') - 1);

            var pos = 0;
            var data = new byte[size];

            for (var i = 0; i < b64str.Length; ++i) 
            {
                var c = b64str[i];
                for (var j = 0; j < 64; ++j)
                {
                    if (encodingTable[j] == c)
                    {
                        SetBitRange (size, data, pos, 6, j);
                        pos += 6;
                        break;
                    }
                }
            }
            return data;
        }

        private static void SetBitRange(int size, byte[] data, int bitRangeStart, int numBits, int bitsToSet)
        {
            var bite = bitRangeStart >> 3;
            var offsetInByte = ( bitRangeStart & 7 );
            var mask = ~(((  0xffffffff) << (32 - numBits)) >> (32 - numBits));

            while (numBits > 0 && bite < size)
            {
                var bitsThisTime = Math.Min( numBits, 8 - offsetInByte);
                var tempMask = (mask << offsetInByte) | ~(((0xffffffff) >> offsetInByte) << offsetInByte);
                var tempBits = bitsToSet << offsetInByte;

                data[bite] = (byte)((data[bite] & tempMask) | tempBits);
                ++bite;
                numBits -= bitsThisTime;
                bitsToSet >>= bitsThisTime;
                mask >>= bitsThisTime;
                offsetInByte = 0;
            }
        }

        public static string Serializer(object o)
        {
            var data = "";

            try
            {
                var x = new System.Xml.Serialization.XmlSerializer(o.GetType());
                using (StringWriter writer = new StringWriter())
                {
                    x.Serialize(writer, o);
                    data = writer.ToString();
                }
            }
            catch
            {
            }

            return data;
        }

        public static object Deserializer(string data, Type type)
        {
            object output;

            try
            {
                var x = new System.Xml.Serialization.XmlSerializer(type);
                using (StringReader reader = new StringReader(data))
                {
                    output = x.Deserialize(reader);
                }
            }
            catch
            {
                output = null;
            }

            return output;
        }

    }
}
