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
		public InfObjectAlreadyExistsException(string iname) : base("�������������� ������ � ������ "+iname+" ��� ����������",iname)
		{}
	}
	public class ElementAlreadyExistsException : AlreadyExistsException
	{
		public ElementAlreadyExistsException(string ename) : base("������� � ������ "+ename+" ��� ����������",ename)
		{}
	}
	public class DataAlreadyExistsException : AlreadyExistsException
	{
		public DataAlreadyExistsException(string dname) : base("������ � ������ "+dname+" ��� ����������",dname)
		{}
	}
	public class AttributeAlreadyExistsException : AlreadyExistsException
	{
		public AttributeAlreadyExistsException(string aname) : base("������� � ������ "+aname+" ��� ����������",aname)
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
		public AttributeNotFoundException(string aname) : base("�� ������ ������� � ������ "+aname,aname)
		{}
	}
	public class DataNotFoundException : NotFoundException
	{
		public DataNotFoundException(string dname) : base("�� ������� ������ � ������ "+dname,dname)
		{}
	}
	public class ElementNotFoundException : NotFoundException
	{
		public ElementNotFoundException(string ename) : base("�� ������ ������� � ������ "+ename,ename)
		{}
	}
	public class InfObjectNotFoundException : NotFoundException
	{
		public InfObjectNotFoundException(string iname) : base("�� ������ ������� � ������ "+iname,iname)
		{}
	}
}