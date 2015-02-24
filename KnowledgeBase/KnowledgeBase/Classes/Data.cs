using System;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;
using System.Xml;

namespace KnowledgeBase
{
	public class Data
	{
		private string d_name;
		private FileInfo d_fileinfo;
		private string d_path;
		public string Path
		{
			get {return this.d_fileinfo.FullName;}
		}
		public bool FileExists
		{
			get {return this.d_fileinfo.Exists;}
		}
		public string Name
		{
			get
			{return d_name;}
			set
			{d_name = value;}
		}
		public FileAttributes Attributes
		{
			get
			{return this.d_fileinfo.Attributes;}
		}
		public DateTime CreationTime
		{
			get
			{return this.d_fileinfo.CreationTime;}
		}
		public long Length
		{
			get
			{return this.d_fileinfo.Length;}
		}
		public Data()
		{
			this.d_name = "";
			this.d_fileinfo = null;
			this.d_path = "";
		}
		public Data(string name,string path)
		{
				this.d_fileinfo = new FileInfo(path);
				this.d_name = name;
				this.d_path = path;
		}
		public void EditFile()
		{
			Process.Start("notepad.exe",this.d_fileinfo.FullName);
		}
		public void OpenFileWith(string name)
		{
			Process.Start(name,this.d_fileinfo.FullName);
		}
		public void OpenFile()
		{
			Process ps = new Process();
			ps.StartInfo.FileName = this.d_fileinfo.FullName;
			ps.StartInfo.UseShellExecute = true;
			ps.Start();
		}
		public void Serialize(XmlElement element)
		{
			element.SetAttribute("name",this.d_name);
			element.InnerText = this.Path;
		}
		public void Deserialize(XmlElement element)
		{
			this.d_name = element.GetAttribute("name");
			this.d_fileinfo = new FileInfo(element.InnerText);
		}
		public override string ToString()
		{
			return this.Path;
		}

	}
}
