using System;

namespace KnowledgeBase
{
	public class Link
	{
		private Element l_e1,l_e2;
		private LinkAttribute l_a1,l_a2;
		private string l_name;
		public string Name
		{
			get {return this.l_name;}
			set {this.l_name = value;}
		}
		public LinkAttribute Link1
		{
			get {return this.l_a1;}
			set {this.l_a1=value;}
		}
		public LinkAttribute Link2
		{
			get {return this.l_a2;}
			set {this.l_a2=value;}	
		}
		public Element Element1
		{
			get {return this.l_e1;}
			set {this.l_e1=value;}
		}
		public Element Element2
		{
			get {return this.l_e2;}
			set {this.l_e2=value;}
		}
		public Link(string name)
		{this.l_name=name;}
		public Link(Element e,string name)
		{
			this.l_e1 = e;
			this.l_name=name;
		}
		public Link(Element e1,Element e2,string name)
		{
			this.l_e1 = e1;
			this.l_e2 = e2;
			this.l_name=name;
		}
	}
}
