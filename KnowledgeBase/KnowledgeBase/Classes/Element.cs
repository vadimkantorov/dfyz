using System;
using System.Collections;

namespace KnowledgeBase
{
	public class Element
	{
		private ArrayList e_attribs;
		private ArrayList e_datas;
		private ArrayList e_chelems;
		private string e_name;
		public readonly Element Parent; 
		
		public bool HasAttributes
		{
			get {return ( this.e_attribs.Count == 0 )?false:true;}
		}
		public bool HasData
		{
			get {return ( this.e_datas.Count == 0 )?false:true;}	
		}
		public bool HasChilds
		{
			get {return ( this.e_chelems.Count == 0 )?false:true;}
		}
		public string Name
		{
			get {return this.e_name;}
			set {this.e_name = value;}
		}
		
		public Element(string name,Element parent)
		{
			this.e_attribs = new ArrayList();
			this.e_datas = new ArrayList();
			this.e_chelems = null;
			this.e_name = name;
			this.Parent = parent;
		}
		
		#region функции для работы с вложенными элементами
		public Element AddElement(string name)
		{
			if ( this.e_chelems == null )
			{
				this.e_chelems = new ArrayList();
			}
			Element element = new Element(name,this);
			this.e_chelems.Add(element);
			return element;
		}
		
		public void DeleteElement(Element element)
		{
			if ( this.e_chelems.Contains(element) )
			{
				this.e_chelems.Remove(element);
				return;
			}
			else throw new ElementNotFoundException(element.Name);
		}
		
		public Element[] GetElements()
		{
			if ( this.e_chelems == null ) return null;
			return (Element[])this.e_chelems.ToArray(typeof(Element));
		}
		#endregion
		
		#region функции для работы с атрибутами
		
		public Attribute AddAttribute(Attribute attribute)
		{
			foreach (Attribute a in this.e_attribs)
				if ( a.Name == attribute.Name ) throw new AttributeAlreadyExistsException(attribute.Name);
			this.e_attribs.Add(attribute);
			return attribute;
		}
		
		public void DeleteAttribute(string name,string type)
		{
			for (int i=0; i<this.e_attribs.Count; i++)
			{
				Attribute a = (Attribute)this.e_attribs[i];
				if ( a.Name == name && a.Type == type) 
				{
					this.e_attribs.RemoveAt(i);
					return;
				}
			}
			throw new AttributeNotFoundException(name);
		}
		public bool AttributeExists(string name)
		{
			foreach (Attribute att in this.e_attribs)
			{
				if ( att.Name == name ) return true;
			}
			return false;
		}
		
		public bool AttributeExists(string name,string type)
		{
			foreach (Attribute att in this.e_attribs)
			{
				if ( att.Name == name && att.Type == type) return true;
			}
			return false;			
		}
		
		public Attribute[] GetAttributes()
		{
			if ( this.e_attribs == null ) return null;
			return (Attribute[])this.e_attribs.ToArray(typeof(Attribute));
		}
		#endregion

		#region функции для работы с данными(Data)
		public Data AddData(Data data)
		{
			foreach (Data d in this.e_datas)
				if ( d.Name == data.Name ) throw new DataAlreadyExistsException(data.Name);
			this.e_datas.Add(data);
			return data;
		}
		
		public void DeleteData(string name)
		{
			for(int i=0; i < this.e_datas.Count; i++)
			{
				if ( ((Data)this.e_datas[i]).Name == name )
					this.e_datas.RemoveAt(i);
			}
			throw new DataNotFoundException(name);
		}
		
		public bool DataExists(string name)
		{
			foreach (Data d in this.e_datas)
			{
				if ( d.Name == name ) return true; 
			}
			return false;
		}
		
		public Data[] GetDatas()
		{
			if ( this.e_datas == null ) return null;
			return (Data[])this.e_datas.ToArray(typeof(Data));
		}
		
		#endregion 
	}
}

