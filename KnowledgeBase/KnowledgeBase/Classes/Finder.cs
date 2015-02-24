using System;
using System.Collections;
using System.Text.RegularExpressions;

namespace KnowledgeBase
{
	public class Finder
	{
		private static ArrayList als = new ArrayList();
		private static bool searchel(Element e,string s,bool r,bool i)
		{
			if ( !r )
				if ( !i )
					return ( e.Name.IndexOf(s) > 0 )?true:false;
				else
					return ( e.Name.ToLower().IndexOf(s.ToLower()) > 0 )?true:false;
			else
				if ( !i )
					return ( Regex.IsMatch(e.Name,s) )?true:false;
				else
					return ( Regex.IsMatch(e.Name,s,RegexOptions.IgnoreCase) )?true:false;
		}
		private static bool searchatt(Attribute at,string s,bool r,bool i,bool n)
		{
			if ( n )
				if ( !r )
					if ( ! i )
						return ( at.Name.IndexOf(s) > 0 )?true:false;
					else
						return ( at.Name.ToLower().IndexOf(s.ToLower()) > 0 )?true:false;
				else
					if ( ! i )
						return ( Regex.IsMatch(at.Name,s) )?true:false;
					else
						return ( Regex.IsMatch(at.Name,s,RegexOptions.IgnoreCase) )?true:false;
			else
				if ( !r )
					if ( ! i )
						return ( at.ToString().IndexOf(s) > 0 )?true:false;
					else
						return ( at.ToString().ToLower().IndexOf(s.ToLower())> 0 )?true:false;
				else
					if ( ! i )
						return ( Regex.IsMatch(at.ToString(),s) )?true:false;
					else
						return ( Regex.IsMatch(at.ToString(),s,RegexOptions.IgnoreCase) )?true:false;
		}
		private static bool searchda(Data d,string s,bool r,bool i,bool n)
		{
			if ( n )
				if ( !r )
					if ( ! i )
						return ( d.Name.IndexOf(s) > 0 )?true:false;
					else
						return ( d.Name.ToLower().IndexOf(s.ToLower()) > 0 )?true:false;
				else
					if ( ! i )
					return ( Regex.IsMatch(d.Name,s) )?true:false;
				else
					return ( Regex.IsMatch(d.Name,s,RegexOptions.IgnoreCase) )?true:false;
			else
				if ( !r )
					if ( ! i )
						return ( d.ToString().IndexOf(s) > 0 )?true:false;
					else
						return ( d.ToString().ToLower().IndexOf(s.ToLower())> 0 )?true:false;
				else
					if ( ! i )
						return ( Regex.IsMatch(d.ToString(),s) )?true:false;
					else
						return ( Regex.IsMatch(d.ToString(),s,RegexOptions.IgnoreCase) )?true:false;
		}
		private static void findsubels(Element e,string s,bool r,bool i)
		{
			if ( searchel(e,s,r,i) ) als.Add(e);
			Element[] es = e.GetElements();
			if ( es != null )
			foreach (Element el in es)
			{
					findsubels(el,s,r,i);
			}
		}
		private static void findas(Element e, string s, bool r, bool i,bool n)
		{
			Attribute[] ats = e.GetAttributes();
			if ( ats != null )
				foreach ( Attribute a in ats)
				{
					if ( searchatt(a,s,r,i,n) ) als.Add(a);
				}
			Element [] es = e.GetElements();
			if ( es != null )
			{
				foreach(Element ele in es)
				{
					findas(ele,s,r,i,n);
				}
			}
		}
		private static void finddats(Element e, string s, bool r, bool i,bool n)
		{
			Data[] ds = e.GetDatas();
			if ( ds != null )
				foreach ( Data d in ds)
				{
					if ( searchda(d,s,r,i,n) ) als.Add(d);
				}
			Element [] es = e.GetElements();
			if ( es != null )
			{
				foreach(Element ele in es)
				{
					finddats(ele,s,r,i,n);
				}
			}	
		}
		public static Element[] FindElements(Root root,string searchstr,bool useregexp,bool ignorecase)
		{
			foreach(Element el in root)
			{
				findsubels(el,searchstr,useregexp,ignorecase);
			}
			return ( als.Count == 0 )?null:(Element[])als.ToArray(typeof(Element));
		} 
		public static Attribute[] FindAttributes(Root root,string searchstr,bool useregexp,bool ignorecase,bool byname)
		{
			foreach (Element e in root)
			{
				findas(e,searchstr,useregexp,ignorecase,byname);
			}
			return ( als.Count == 0 )?null:(Attribute[])als.ToArray(typeof(Attribute));
		}
		public static Data[] FindDatas(Root root,string searchstr,bool useregexp,bool ignorecase,bool byname)
		{
			foreach (Element e in root)
			{
				finddats(e,searchstr,useregexp,ignorecase,byname);				
			}
			return ( als.Count == 0 )?null:(Data[])als.ToArray(typeof(Data));
		}
	}
}
