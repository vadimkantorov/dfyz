using System;
using System.Xml;
using System.Reflection;
using System.Collections;

namespace KnowledgeBase
{
	/// <summary>
	/// Сериализатор в XML-файл
	/// </summary>
	public class Saver
	{
		private XmlDocument document;
		private XmlTextWriter wr;
		public Saver()
		{
			document = new XmlDocument();
		}
		private void write_attributes(Element el,XmlElement elem)
		{
			Attribute[] ats = el.GetAttributes();
			if ( ats != null )
			foreach (Attribute at in ats)
			{
				XmlElement e = document.CreateElement("Attribute");
				at.Serialize(e,document);
				elem.AppendChild(e);
			}
		}
		private void write_elements(Element el,XmlElement elem)
		{
			Element[] els = el.GetElements();
			if ( els != null )
			foreach (Element e in els)
			{
				XmlElement eleme = document.CreateElement("Element");
				eleme.SetAttribute("name",e.Name);
				this.write_attributes(e,eleme);
				this.write_datas(e,eleme);
				this.write_elements(e,eleme);
				elem.AppendChild(eleme);
			}
		}
		private void write_datas(Element el,XmlElement elem)
		{
			Data[] dts = el.GetDatas();
			if ( dts != null )
			foreach (Data d in dts)
			{
				XmlElement e = document.CreateElement("Data");
				d.Serialize(e);
				elem.AppendChild(e);
			}
		}
		public void Save(Root root, string filename)
		{
			document = new XmlDocument();
			document.AppendChild(document.CreateXmlDeclaration("1.0","UTF-8",null));
			document.AppendChild(document.CreateComment("This is KnowledgeBase file"));
			XmlElement objel = document.CreateElement("objects");
			document.AppendChild(objel);
			foreach(Element elem in root)
			{
				XmlElement el = document.CreateElement("Element");
				el.SetAttribute("name",elem.Name);
				write_attributes(elem,el);
				write_datas(elem,el);
				write_elements(elem,el);
				objel.AppendChild(el);
			}
			try
			{
				wr = new XmlTextWriter(filename,null);
				wr.Formatting = Formatting.Indented;
				document.PreserveWhitespace = true;
				document.WriteContentTo(wr);
			}
			finally
			{
				wr.Close();	
			}
		}
	}
}
