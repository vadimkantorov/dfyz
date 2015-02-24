using System;
using System.Collections;

namespace KnowledgeBase
{
	public class Root : IEnumerable
	{
		private ArrayList r_els;
		private ArrayList r_linkz;
		public Root()
		{
			this.r_els = new ArrayList();
			this.r_linkz = new ArrayList();
		}
		public Link AddLink(Link link)
		{
			int ind = this.r_linkz.Add(link);
			LinkAttribute l1 = new LinkAttribute(link.Name,ind);
			link.Element1.AddAttribute(l1);
			link.Link1 = l1;
			LinkAttribute l2 = new LinkAttribute(link.Name,ind);
			link.Element2.AddAttribute(l2);
			link.Link2 = l2;
			return link;
		}
		public void DeleteLink(int id)
		{
			Link l = (Link)this.r_linkz[id];
			l.Element1.DeleteAttribute(l.Link1.Name,l.Link1.Type);
			l.Element2.DeleteAttribute(l.Link2.Name,l.Link2.Type);
			this.r_linkz.RemoveAt(id);
			for (int i=0;i<this.r_linkz.Count;i++)
			{
				((Link)this.r_linkz[i]).Link1.Value = i;
				((Link)this.r_linkz[i]).Link2.Value = i;
			}
		}
		public Link GetLinkByID(int id)
		{
			return (Link)this.r_linkz[id];
		}
		public Link[] GetLinks()
		{
			return (Link[])this.r_linkz.ToArray(typeof(Link));	
		}
		public bool LinkExists(int id)
		{
			if ( this.r_linkz.Count == 0 || this.r_linkz.Count <= id ) return false;
			else return true;
		}
		public Element[] GetLinkElements(int id)
		{
			Element[] es = new Element[2];
			es[0] = ((Link)this.r_linkz[id]).Element1;
			es[1] = ((Link)this.r_linkz[id]).Element2;
			return es;
		}
		public Element AddElement(string name)
		{
			Element element = new Element(name,null);
			this.r_els.Add(element);
			return element;
		}
		public void DeleteElement(Element element)
		{
			if ( this.r_els.Contains(element) ) this.r_els.Remove(element);
			throw new ElementNotFoundException(element.Name);
		}
		#region Реализация IEnumerator

		public IEnumerator GetEnumerator()
		{
			return this.r_els.GetEnumerator();
		}

		#endregion
	}
}
