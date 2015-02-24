using System;
using System.Windows.Forms;
using System.Xml;
using System.Net;
using System.Text;
using System.Reflection;
using System.Collections; 

namespace KnowledgeBase
{
	//�������� (.NET-������) , ������� ����������, ��� ��������(���� ������)- ���������
	[AttributeUsage(AttributeTargets.Class,Inherited=true)]
	public class ScalarAttributeAttribute : System.Attribute
	{}
	[AttributeUsage(AttributeTargets.Class,Inherited=true)]
	public class MultipleAttributeAttribute : System.Attribute
	{}
	/*��� ����������� ����� ���������, �� �������� ����������� ��� ���������
	���������*/
	public abstract class Attribute
	{
		protected string a_name; 
		public virtual string Type
		{
			get {return "";}
		} 
		public bool Multiplicity; //���������������(��/���). 
		public virtual bool IsDefined //����������, ��������� �� �������� (����� �� ����������)
		{
			get {return false;}
		}
		public string Name //�������� � ������ 
		{
			get
			{ return this.a_name; }
			set
			{ this.a_name = value;}
		}
		//�����������. ��� �������������� ������ ���������� ��� ��� ������� ����� � ���������������
		public Attribute(string name,bool multi) 
		{
			this.a_name = name;
			this.Multiplicity = multi;
		}
		//��������� ������� �������������/��������������� �� XML-�����
		public abstract void Serialize(XmlElement element,XmlDocument document);
		public abstract void Deserialize(XmlElement element);
	}
	/*������� ����� ��� ���������� ���������, �� ���� ��� ��������� �� ����������� 
	���������������� � �������� ���*/
	[ScalarAttribute]
	public class ScalarAttribute : Attribute
	{
		public ScalarAttribute():base("",false)
		{}
		/*�������� ����������� �������� ������ � �������, ��� �� �� �������������
		(Multiplicity = false)*/
		public ScalarAttribute(string name): base(name,false)
		{}
		/*��������� �������� ����� ��� � ���. ������� ��� � ��� ������������� ����� �����,
		� ������ �� ���������� �������� ����������� �� �����-�������*/
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			element.SetAttribute("name",this.a_name);
			element.SetAttribute("multiplicity",this.Multiplicity.ToString());
		}
		//������������� ���
		public override void Deserialize(XmlElement element)
		{
			this.a_name = element.GetAttribute("name");	
		}
	}
	/*������ ���� ���� �� ������ ������-���������� ���������(!) ���������. � ������� �� ���
	���� �������� �� ���������(��� �������� ����� ���� ����� - ������, �����, URL,
	����, � �.�.) ������ ����� ���������� ��� ��������� ������� � ���������� ��������
	IsDefined. ��� ���������� �� ���������, �������� ����� ��������� ���, ����� ������
	��������, null �� ��� ���, � ���������� �������������� false � true. ��� ����������
	� ����� �� �������� ��� �� �������. ������� � ��� ��������� ����������� ��������� ����,
	��� �����������, ��������� �������� ��� ��� (����� �������� �� ����������).
	� �������������� ������ ������ ��� ����. ������ �� ��� - ��� ������. ��� �������� 
	��������� �����. ������ �� - ���� �� �������� - ��� ��� �� ��������.*/
	public class TextScalarAttribute : ScalarAttribute //��������-������, ��������� ���
	{
		private string ssa_value; //���� �� ���������
		public override string Type
		{
			get {return "TEXT";}
		}
		public override bool IsDefined //��������, ������������, ��������� �������� ��� ���
		{
			get {return ( this.ssa_value == null )?false:true;} //���� �������� �� null, ���������� true
		}
		public string Value //��� �������� � ���� �� ���������. 
		{
			get 
			{
				//��� ������� �������� �������������� ��������, ��������� ����������
				if ( this.ssa_value == null ) throw new NullReferenceException("�������� ������ �� ����������");
				return this.ssa_value; 
			}
			set {this.ssa_value = value;}	
		}
		public TextScalarAttribute():base()
		{}
		//�����������, � ������� ���������� ������ ��� (��� ��������)
		public TextScalarAttribute(string name) : base(name)
		{
			this.ssa_value=null;//�������� �� ���������� 
		}
		//����������� � ������ � ���������
		public TextScalarAttribute(string name,string val) : base(name)
		{
			this.ssa_value = val; //����������� �������� ���������
		}
		//�������������
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			/*����� ������������� ����������� ��� �������� ����� � ��������� �� �������� 
			��������� � �������� �� �� \n � \t*/
			string stripped = this.ssa_value.Replace("\r","").Replace("\n",@"\n").Replace("\t",@"\t");
			base.Serialize(element,document);
			element.SetAttribute("type","TEXT");//��������� ���������� � ���� 
			element.InnerText = stripped;	 
		}
		//���������������
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			/*�������� ��������, �������� ��������� � ������ ������������*/
			string antistripped = element.InnerText.Replace(@"\n","\r\n").Replace(@"\t","\t");
			//����������� �������� �� XML-����
			this.ssa_value = antistripped;
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			return this.ssa_value;
		}

	}
	public class DateTimeScalarAttribute : ScalarAttribute //��������-����/�����. ��� �������� - �� �������� (����� ����������)
	{
		private bool dtsa_defined; //����������� ����, ����������, ��������� �������� ��� ��� 
		private DateTime dtsa_datetime; //��������
		public override string Type
		{
			get {return "DATETIME";}
		}
		public override bool IsDefined //���������� ������������� ����
		{
			get {return this.dtsa_defined;}
		}
		public DateTime Value
		{
			/*��������-��������. ��� ����� ��� �� ��� � ������, �� ����� �� �������������
			������������� ����������� ���� � true ��� ���������� �������� ���������*/
			get 
			{
				if ( !this.dtsa_defined ) throw new NullReferenceException("�������� ����/������� �� ����������");
				return this.dtsa_datetime;
			}
			set 
			{
				this.dtsa_datetime = value;
				this.dtsa_defined = true;
			}
		}
		public DateTimeScalarAttribute() : base()
		{}
		/*������������(� ��� ������������ ���� ������������� �������� true ��� false � �����������
		�� ����, ���� �� ��������� ��������)*/
		public DateTimeScalarAttribute(string name) : base(name)
		{
			this.dtsa_defined = false; 	
		}
		public DateTimeScalarAttribute(string name,DateTime date) : base(name)
		{
			this.dtsa_datetime = date;
			this.dtsa_defined = true;
		}
		/*������������ � ��������������. ��� �������� ��� ��, ��� � ������, �� � ���������������,
		�� ����������� ������������ ���� �������� tru, ��������� �������� ��� ���������*/ 
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			string dt = this.dtsa_datetime.ToString();
			base.Serialize(element,document);
			element.SetAttribute("type","DATETIME");
			element.InnerText = dt;
		}
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			this.dtsa_datetime = DateTime.Parse(element.InnerText);
			this.dtsa_defined = true;
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			return this.dtsa_datetime.ToString();
		}

	}
	/*���������� ��������� ����*/
	public class URLScalarAttribute : ScalarAttribute //URL, �������� - ���������
	{
		private Uri usa_url;
		public override bool IsDefined
		{
			get {return ( this.usa_url == null )?false:true;}
		}
		public override string Type
		{
			get {return "URL";}
		}
		public Uri Value
		{
			get 
			{
				if ( this.usa_url == null ) throw new NullReferenceException("�������� URL �� ����������");
				return this.usa_url;
			}
			set {this.usa_url = value;}
		}
		public URLScalarAttribute():base()
		{}
		public URLScalarAttribute(string name) : base(name)
		{
			this.usa_url = null;
		}
		public URLScalarAttribute(string name,Uri uri) : base(name)
		{
			this.usa_url = uri;
		}
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			base.Serialize(element,document);
			element.SetAttribute("type","URL");
			element.InnerText = this.usa_url.ToString();
		}
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			this.usa_url = new Uri(element.InnerText);
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			return this.usa_url.ToString();
		}

	}
	public class IPScalarAttribute : ScalarAttribute //IP-�����, �������� - ���������
	{
		private IPAddress isa_ip;
		public override string Type
		{
			get {return "IP";}
		}
		public override bool IsDefined
		{
			get {return ( this.isa_ip == null )?false:true;}
		}
		public IPAddress Value
		{
			get {return this.isa_ip;}
			set 
			{
				if ( this.isa_ip == null ) throw new NullReferenceException("�������� IP-������ �� ����������");
				this.isa_ip = value;
			}
		}
		public IPScalarAttribute():base()
		{}
		public IPScalarAttribute(string name) : base(name)
		{
			this.isa_ip = null;
		}
		public IPScalarAttribute(string name,IPAddress ip) : base(name)
		{
			this.isa_ip = ip;
		}
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			base.Serialize(element,document);
			element.SetAttribute("type","IP");
			element.InnerText = this.isa_ip.ToString();
		}
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			this.isa_ip = IPAddress.Parse(element.InnerText);
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			return this.isa_ip.ToString();
		}

	}
	public class BooleanScalarAttribute : ScalarAttribute //���������� ��������(true/false),��� �������� - �� ��������
	{
		private bool bsa_defined;
		private bool bsa_bool;
		public override string Type
		{
			get {return "BOOLEAN";}
		}
		public override bool IsDefined
		{
			get {return this.bsa_defined;}
		}
		public bool Value
		{
			get 
			{
				if ( !this.bsa_defined ) throw new NullReferenceException("�������� ����������� ��������� �� ����������!");
				return this.bsa_bool;
			}
			set {this.bsa_bool = value;}
		}
		public BooleanScalarAttribute():base()
		{}
		public BooleanScalarAttribute(string name) : base(name)
		{
			this.bsa_defined = false;
		}
		public BooleanScalarAttribute(string name,bool logic) : base(name)
		{
			this.bsa_bool = logic;
			this.bsa_defined = true;
		}
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			base.Serialize(element,document);
			element.SetAttribute("type","BOOLEAN");
			element.InnerText = this.bsa_bool.ToString();
		}
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			this.bsa_bool = Boolean.Parse(element.InnerText);
			this.bsa_defined = true;
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			if ( this.bsa_bool ) return "������";
			else return "����";
		}

	}
	public class NumberScalarAttribute : ScalarAttribute //�����,��� �������� - �� ��������
	{
		private bool nsa_defined;
		private double nsa_number;
		public override string Type
		{
			get {return "NUMBER";}
		}
		public override bool IsDefined
		{
			get {return this.nsa_defined;}
		}

		public double Value
		{
			get 
			{
				if ( !this.nsa_defined ) throw new NullReferenceException("�������� ����� �� ����������");
				return this.nsa_number;
			}
			set {this.nsa_number = value;}
		}
		public NumberScalarAttribute():base()
		{}
		public NumberScalarAttribute(string name) : base(name)
		{
			this.nsa_defined = false;
		}
		public NumberScalarAttribute(string name,double number) : base(name)
		{
			this.nsa_number = number;
			this.nsa_defined = true;
		}
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			base.Serialize(element,document);
			element.SetAttribute("type","NUMBER");
			element.InnerText = this.nsa_number.ToString();
		}
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			this.nsa_number = Double.Parse(element.InnerText);
			this.nsa_defined = true;
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("�� ��������� �������� ��������");
			return this.nsa_number.ToString();
		}

	}
	
	public class MultipleAttribute : Attribute,IEnumerable 
	{
		private Type lma_type;
		private ArrayList lma_attrs = new ArrayList();
		public override string Type
		{
			get {return this.lma_type.Name.Replace("ScalarAttribute","").ToUpper();}
		}

		public int Count
		{
			get {return this.lma_attrs.Count;}
		}
		public MultipleAttribute(string name) : base(name,true)
		{
			this.lma_type = null;
		}
		public MultipleAttribute(string name,Type type) : base(name,true)
		{
			this.lma_type = type;
		}
		public void Remove(int i)
		{
			if ( this.lma_attrs.Count == 0 || this.lma_attrs.Count <= i ) throw new NullReferenceException("�� ���������� ����������� � ����� ��������");
			else this.lma_attrs.RemoveAt(i);
		}
		public ScalarAttribute Pop()
		{
			if ( this.lma_attrs.Count == 0 ) return null;
			this.lma_attrs.RemoveAt(this.lma_attrs.Count-1);
			return (ScalarAttribute)this.lma_attrs[this.lma_attrs.Count-1];
		}
		public void Push(ScalarAttribute attribute)
		{
			if ( this.lma_type == null ) throw new InvalidOperationException("�� ��������� ��� �������������� ��������!");
			if ( attribute.GetType() != this.lma_type ) throw new InvalidCastException("�������� ���!"); 
			foreach (ScalarAttribute sa in this.lma_attrs)
			{
				if ( sa.Name == attribute.Name && sa.Type == attribute.Type ) throw new  AttributeAlreadyExistsException(attribute.Name);
			}
			this.lma_attrs.Insert(this.lma_attrs.Count,attribute);
		}
		public ScalarAttribute this[int index]
		{
			get
			{
				if ( this.lma_attrs.Count == 0 || this.lma_attrs.Count <= index ) return null;
				return (ScalarAttribute)this.lma_attrs[index];
			}
			set
			{
				if ( this.lma_attrs.Count == 0 || this.lma_attrs.Count <= index ) throw new NullReferenceException("�� ���������� ����������� � ����� ��������");
				this.lma_attrs[index] = value;
			}
		}
		public IEnumerator GetEnumerator()
		{return this.lma_attrs.GetEnumerator();}
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			element.SetAttribute("type",this.lma_type.Name);
			foreach (ScalarAttribute sa in this)
			{
				element.SetAttribute("name",this.a_name);
				element.SetAttribute("multiplicity",this.Multiplicity.ToString());
				XmlElement values = document.CreateElement("SubAttribute");
				sa.Serialize(values,document);
				element.AppendChild(values);
			}
		}
		public override void Deserialize(XmlElement element)
		{
			string tp = element.GetAttribute("type");
			this.lma_type = System.Type.GetType("KnowledgeBase."+tp);
			foreach (XmlElement xma in element.GetElementsByTagName("SubAttribute"))
			{
				this.Name = element.GetAttribute("name");
				ScalarAttribute sa = (ScalarAttribute)Activator.CreateInstance(this.lma_type);
				sa.Deserialize(xma);
				this.Push(sa);						
			}
		}
		public override string ToString()
		{
			StringBuilder s = new StringBuilder(""); 
			if ( this.lma_attrs.Count == 0 )
			throw new InvalidOperationException("�� ��������� �������� ��������");
			foreach (ScalarAttribute sa in this.lma_attrs)
			{											
				s.Append(sa.ToString());	
			}
			return s.ToString();
		}
	}
	public class LinkAttribute   : Attribute
	{
		private int l_id; 
		public int Value
		{
			get {return this.l_id;}
			set {this.l_id=value;}
		}
		public LinkAttribute():base("",false)
		{}
		public LinkAttribute(string name,int id):base(name,false)
		{
			this.l_id = id;
		}
		public override string Type
		{get {return "LINK";}}
		public override void Serialize(XmlElement element, XmlDocument document)
		{
			element.SetAttribute("name",this.Name);
			element.SetAttribute("multiplicity",this.Multiplicity.ToString());
			element.SetAttribute("type","LINK");
			element.InnerText = this.l_id.ToString();
		}
		public override void Deserialize(XmlElement element)
		{
			this.Name = element.GetAttribute("name");
			this.l_id = int.Parse(element.InnerText);
		}
	}
}