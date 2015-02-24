using System;
using System.Windows.Forms;
using System.Xml;
using System.Net;
using System.Text;
using System.Reflection;
using System.Collections; 

namespace KnowledgeBase
{
	//аттрибут (.NET-овский) , который показывает, что аттрибут(базы знаний)- скалярный
	[AttributeUsage(AttributeTargets.Class,Inherited=true)]
	public class ScalarAttributeAttribute : System.Attribute
	{}
	[AttributeUsage(AttributeTargets.Class,Inherited=true)]
	public class MultipleAttributeAttribute : System.Attribute
	{}
	/*Это абстрактный класс аттрибута, от которого наследуются все остальные
	аттрибуты*/
	public abstract class Attribute
	{
		protected string a_name; 
		public virtual string Type
		{
			get {return "";}
		} 
		public bool Multiplicity; //множественность(да/нет). 
		public virtual bool IsDefined //определяет, определен ли аттрибут (сорри за тавтологию)
		{
			get {return false;}
		}
		public string Name //свойство с именем 
		{
			get
			{ return this.a_name; }
			set
			{ this.a_name = value;}
		}
		//конструктор. Все унаследованные классы используют его для задания имени и множественности
		public Attribute(string name,bool multi) 
		{
			this.a_name = name;
			this.Multiplicity = multi;
		}
		//прототипы методов сериализовать/десериализовать из XML-файла
		public abstract void Serialize(XmlElement element,XmlDocument document);
		public abstract void Deserialize(XmlElement element);
	}
	/*базовый класс для скалярного аттрибута, то есть для аттрибута не обладающего 
	множественностью и имеющего тип*/
	[ScalarAttribute]
	public class ScalarAttribute : Attribute
	{
		public ScalarAttribute():base("",false)
		{}
		/*вызываем конструктор базового класса и говорим, что мы не множественные
		(Multiplicity = false)*/
		public ScalarAttribute(string name): base(name,false)
		{}
		/*скалярный аттрибут имеет имя и тип. Поэтому имя и тип сериализуются прямо здесь,
		а работа по сохранению значения возлагается на класс-потомок*/
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			element.SetAttribute("name",this.a_name);
			element.SetAttribute("multiplicity",this.Multiplicity.ToString());
		}
		//десериализуем имя
		public override void Deserialize(XmlElement element)
		{
			this.a_name = element.GetAttribute("name");	
		}
	}
	/*Дальше идут один за другим классы-конкретные скалярные(!) аттрибуты. У каждого из них
	есть свойство со значением(тип которого может быть любым - строка, число, URL,
	дата, и т.д.) Однако здесь существуют два различных подхода к реализации свойства
	IsDefined. Для аттрибутов со значением, являющим собой ссылочный тип, можно просто
	смотреть, null он или нет, и возвращать соответственно false и true. Для аттрибутов
	с типом по значению это не пройдет. Поэтому в них создается специальное приватное поле,
	для определения, определен аттрибут или нет (снова извините за тавтологию).
	Я прокоменнтирую только первые два типа. Первый из них - это строка. Она является 
	ссылочным типом. Второй же - дата со временем - это тип по значению.*/
	public class TextScalarAttribute : ScalarAttribute //аттрибут-строка, ссылочный тип
	{
		private string ssa_value; //поле со значением
		public override string Type
		{
			get {return "TEXT";}
		}
		public override bool IsDefined //свойство, показывающее, определен аттрибут или нет
		{
			get {return ( this.ssa_value == null )?false:true;} //если значение не null, возвращаем true
		}
		public string Value //это оболочка к полю со значением. 
		{
			get 
			{
				//При попытке получить неопределенное значение, бросается исключение
				if ( this.ssa_value == null ) throw new NullReferenceException("Значение строки не определено");
				return this.ssa_value; 
			}
			set {this.ssa_value = value;}	
		}
		public TextScalarAttribute():base()
		{}
		//конструктор, в котором передается только имя (без значения)
		public TextScalarAttribute(string name) : base(name)
		{
			this.ssa_value=null;//значение не определено 
		}
		//конструктор с именем и значением
		public TextScalarAttribute(string name,string val) : base(name)
		{
			this.ssa_value = val; //присваиваем значение аттрибуту
		}
		//Сериализуемся
		public override void Serialize(XmlElement element,XmlDocument document)
		{
			/*Перед сериализацией выбрасываем все переводы строк и табуляции из значения 
			аттрибута и заменяем их на \n и \t*/
			string stripped = this.ssa_value.Replace("\r","").Replace("\n",@"\n").Replace("\t",@"\t");
			base.Serialize(element,document);
			element.SetAttribute("type","TEXT");//добавляем информацию о типе 
			element.InnerText = stripped;	 
		}
		//Десериализуемся
		public override void Deserialize(XmlElement element)
		{
			base.Deserialize(element);
			/*Проводим операцию, обратную описанной в методе сериализации*/
			string antistripped = element.InnerText.Replace(@"\n","\r\n").Replace(@"\t","\t");
			//присваиваем значение из XML-тега
			this.ssa_value = antistripped;
		}
		public override string ToString()
		{
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
			return this.ssa_value;
		}

	}
	public class DateTimeScalarAttribute : ScalarAttribute //аттрибут-дата/время. Тип значения - по значению (опять тавтология)
	{
		private bool dtsa_defined; //специальное поле, показывает, определен аттрибут или нет 
		private DateTime dtsa_datetime; //значение
		public override string Type
		{
			get {return "DATETIME";}
		}
		public override bool IsDefined //возвращаем вышеуказанное поле
		{
			get {return this.dtsa_defined;}
		}
		public DateTime Value
		{
			/*свойство-значение. Все точно так же как в строке, но здесь мы дополнительно
			устанавливаем специальное поле в true при присвоении значения аттрибуту*/
			get 
			{
				if ( !this.dtsa_defined ) throw new NullReferenceException("Значение даты/времени не определено");
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
		/*конструкторы(в них специальному полю присваивается значение true или false в зависимости
		от того, было ли присвоено значение)*/
		public DateTimeScalarAttribute(string name) : base(name)
		{
			this.dtsa_defined = false; 	
		}
		public DateTimeScalarAttribute(string name,DateTime date) : base(name)
		{
			this.dtsa_datetime = date;
			this.dtsa_defined = true;
		}
		/*сериализатор и десериализатор. Все примерно так же, как в строке, но в десериализаторе,
		мы присваиваем специальному полю значение tru, поскольку аттрибут был определен*/ 
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
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
			return this.dtsa_datetime.ToString();
		}

	}
	/*дальнейшие скалярные типы*/
	public class URLScalarAttribute : ScalarAttribute //URL, значение - ссылочное
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
				if ( this.usa_url == null ) throw new NullReferenceException("Значение URL не определено");
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
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
			return this.usa_url.ToString();
		}

	}
	public class IPScalarAttribute : ScalarAttribute //IP-адрес, значение - ссылочное
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
				if ( this.isa_ip == null ) throw new NullReferenceException("Значение IP-адреса не определено");
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
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
			return this.isa_ip.ToString();
		}

	}
	public class BooleanScalarAttribute : ScalarAttribute //логическое значение(true/false),тип значения - по значению
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
				if ( !this.bsa_defined ) throw new NullReferenceException("Значение логического выражения не определено!");
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
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
			if ( this.bsa_bool ) return "Истина";
			else return "Ложь";
		}

	}
	public class NumberScalarAttribute : ScalarAttribute //число,тип значения - по значению
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
				if ( !this.nsa_defined ) throw new NullReferenceException("Значение числа не определено");
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
			if ( !this.IsDefined ) throw new InvalidOperationException("Не присвоено значение атрибута");
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
			if ( this.lma_attrs.Count == 0 || this.lma_attrs.Count <= i ) throw new NullReferenceException("Не существует податрибута с таким индексом");
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
			if ( this.lma_type == null ) throw new InvalidOperationException("Не определен тип множественного атрибута!");
			if ( attribute.GetType() != this.lma_type ) throw new InvalidCastException("Неверный тип!"); 
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
				if ( this.lma_attrs.Count == 0 || this.lma_attrs.Count <= index ) throw new NullReferenceException("Не существует податрибута с таким индексом");
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
			throw new InvalidOperationException("Не присвоено значение атрибута");
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