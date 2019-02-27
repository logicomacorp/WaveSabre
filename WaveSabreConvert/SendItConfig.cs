using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WaveSabreConvert
{

    /// <remarks/>
    [System.Xml.Serialization.XmlTypeAttribute(AnonymousType = true)]
    [System.Xml.Serialization.XmlRootAttribute(Namespace = "", IsNullable = false)]
    public partial class MYPLUGINSETTINGS
    {

        private int modeField;

        private int channelField;

        private int passThruField;

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int Mode
        {
            get
            {
                return this.modeField;
            }
            set
            {
                this.modeField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int Channel
        {
            get
            {
                return this.channelField;
            }
            set
            {
                this.channelField = value;
            }
        }

        /// <remarks/>
        [System.Xml.Serialization.XmlAttributeAttribute()]
        public int PassThru
        {
            get
            {
                return this.passThruField;
            }
            set
            {
                this.passThruField = value;
            }
        }
    }


}
