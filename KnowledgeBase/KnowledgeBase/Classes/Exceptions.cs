using System;

namespace KnowledgeBase
{
	public abstract class AlreadyExistsException : ApplicationException
	{
		public readonly string WhatAlreadyExists;
		public AlreadyExistsException(string errormessage, string whatalreadyexists) : base(errormessage)
		{
			this.WhatAlreadyExists = whatalreadyexists;
		}
	}
	
	public class InfObjectAlreadyExistsException : AlreadyExistsException
	{
		public InfObjectAlreadyExistsException(string iname) : base("Информационный объект с именем "+iname+" уже существует",iname)
		{}
	}
	public class ElementAlreadyExistsException : AlreadyExistsException
	{
		public ElementAlreadyExistsException(string ename) : base("Элемент с именем "+ename+" уже существует",ename)
		{}
	}
	public class DataAlreadyExistsException : AlreadyExistsException
	{
		public DataAlreadyExistsException(string dname) : base("Данные с именем "+dname+" уже существуют",dname)
		{}
	}
	public class AttributeAlreadyExistsException : AlreadyExistsException
	{
		public AttributeAlreadyExistsException(string aname) : base("Атрибут с именем "+aname+" уже существуют",aname)
		{}
	}

	public abstract class NotFoundException : ApplicationException
	{
		public readonly string WhatNotFound;
		public NotFoundException(string errormessage, string whatnotfound) : base(errormessage) 
		{
			this.WhatNotFound = whatnotfound;
		}
	}
	
	public class AttributeNotFoundException : NotFoundException
	{
		public AttributeNotFoundException(string aname) : base("Не найден атрибут с именем "+aname,aname)
		{}
	}
	public class DataNotFoundException : NotFoundException
	{
		public DataNotFoundException(string dname) : base("Не найдены данные с именем "+dname,dname)
		{}
	}
	public class ElementNotFoundException : NotFoundException
	{
		public ElementNotFoundException(string ename) : base("Не найден элемент с именем "+ename,ename)
		{}
	}
	public class InfObjectNotFoundException : NotFoundException
	{
		public InfObjectNotFoundException(string iname) : base("Не найден элемент с именем "+iname,iname)
		{}
	}
}