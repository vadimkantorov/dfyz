using System;
using System.Xml;
using System.IO;
using System.Collections;
using System.Reflection;

namespace KnowledgeBase
{
	public class Loader
	{
		private Hashtable scalars;
		private XmlDocument document; 
		private Hashtable ls = new Hashtable();
		public Loader()
		{
			this.scalars = new Hashtable();
			this.document = new XmlDocument();
			Assembly assem = Assembly.GetExecutingAssembly();
			foreach (Type t in assem.GetTypes())
			{
				if ( t.IsDefined(typeof(ScalarAttributeAttribute),true) )				
				{
					this.scalars.Add(t.Name.Replace("ScalarAttribute","").ToUpper(),t);	
				}
			}
		}
		private void deser_elems(Element e,XmlNode x,Root r)
		{
			foreach (XmlNode n in x.ChildNodes)
			{
				XmlElement xe = (XmlElement)n;
				if ( xe.Name == "Element" )
				{
					Element em = e.AddElement(xe.GetAttribute("name"));
					deser_attrs(em,xe,r);
					deser_datas(em,xe);
					deser_elems(em,xe,r);
				}
			}
		}
		private void deser_datas(Element e,XmlNode x)
		{
			foreach (XmlNode n in x.ChildNodes)
			{
				XmlElement xe = (XmlElement)n;
				if ( xe.Name == "Data" )
				{
					Data d = new Data();
					d.Deserialize(xe);
					e.AddData(d);
				}
			}
		}
		private void deser_attrs(Element e,XmlNode x,Root r)
		{
			foreach(XmlNode n in x.ChildNodes)
			{
				Type t = null;
				XmlElement xe = (XmlElement)n;
				if ( xe.Name == "Attribute" )
				{
					if ( xe.GetAttribute("type") == "LINK" )
					{
						int i = int.Parse(xe.InnerText);
						if ( !ls.ContainsKey(i) )
						{
							ls[i] = new Link(e,xe.GetAttribute("name"));
						}
						else
						{
							((Link)ls[i]).Element2 = e;
						}
						continue;
					}
					t = (Type)this.scalars[xe.GetAttribute("type")];
					if ( xe.GetAttribute("multiplicity") == "False" )
					{
						ScalarAttribute s = (ScalarAttribute)Activator.CreateInstance(t);
						s.Deserialize(xe);
						e.AddAttribute(s);
					}
					else if ( xe.GetAttribute("multiplicity") == "True" )
					{
						MultipleAttribute lma = new MultipleAttribute("");
						lma.Deserialize(xe);
						e.AddAttribute(lma);
					}
				}
			}
		}
		public Root Load(string filename)
		{
			StreamReader str = new StreamReader(filename);
			XmlTextReader rdr = new XmlTextReader(str);
			this.document.Load(rdr);
			Root root = new Root();
			XmlNodeList list = document.DocumentElement.ChildNodes;
			foreach (XmlNode n in list)
			{
				if ( n.Name == "Element" )
				{
					Element e = root.AddElement(((XmlElement)n).GetAttribute("name"));
					deser_attrs(e,n,root);
					deser_datas(e,n);
					deser_elems(e,n,root);
				}
			}
			for (int i=0;i<this.ls.Count;i++)
			{
				if ( ls.ContainsKey(i) ) root.AddLink((Link)ls[i]);
			}
			return root;
			rdr.Close();
		}
	}
}
