using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Task3
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_MouseClick(object sender, MouseEventArgs e)
		{
			var loc = e.Location;
			if (e.Button == MouseButtons.Left)
				polygon.Add(loc);
			else if (e.Button == MouseButtons.Right)
				AddPointToRectangle(loc);
			Refresh();
		}

		private void Form1_Paint(object sender, PaintEventArgs e)
		{
			var g = e.Graphics;
			DrawPolygon(g);
			DrawRectangle(g);
		}

		private void Form1_KeyPress(object sender, KeyPressEventArgs e)
		{
			switch (char.ToLower(e.KeyChar))
			{
				case 'r':
					ClearData();
					break;
				case 'c':
					if (originalPolygon == null)
					{
						originalPolygon = polygon;
						polygon = clipper.Clip(polygon, rectangle);
					}
					else
					{
						polygon = originalPolygon;
						originalPolygon = null;
					}
					break;
				case 'f':
					ClearData();
					using (var sr = new StreamReader("input.txt"))
					{
						var n = int.Parse(sr.ReadLine());
						Func<Point> readPoint = () =>
						{
							var nums = sr.ReadLine().Split().Select(int.Parse).ToArray();
							return new Point(nums.First(), nums.Last());
						};
						for (var i = 0; i < n; i++)
							polygon.Add(readPoint());
						for (var i = 0; i < 2; i++)
							AddPointToRectangle(readPoint());
					}
					break;
			}
			Refresh();
		}

		private void AddPointToRectangle(Point p)
		{
			rectangle.Add(p);
			if (rectangle.Count > 2)
				rectangle.RemoveAt(0);
			if (rectangle.Count != 2)
				return;
			var p1 = rectangle.First();
			var p2 = rectangle.Last();
			if (p1.Y > p2.Y)
			{
				var tmp = p1;
				p1 = p2;
				p2 = tmp;
			}
			if (p1.X > p2.X)
			{
				var d = p1.X - p2.X;
				p1.X -= d;
				p2.X += d;
			}
			rectangle = new List<Point> {p1, p2};
		}

		private void ClearData()
		{
			polygon.Clear();
			rectangle.Clear();
		}

		private void DrawRectangle(Graphics g)
		{
			if (rectangle.Count >= 2)
			{
				var p1 = rectangle.First();
				var p2 = rectangle.Last();
				g.DrawRectangle(rectanglePen, p1.X, p1.Y, p2.X - p1.X, p2.Y - p1.Y);
			}
		}

		private void DrawPolygon(Graphics g)
		{
			var polygonPoints = polygon.ToArray();
			foreach (var p in polygonPoints)
			{
				var size = new Size(8, 8);
				var halfSize = new Size(size.Width/2, size.Height/2);
				g.FillEllipse(polygonPen.Brush, new Rectangle(p - halfSize, size));
			}
			if (polygon.Count >= 2)
				g.FillPolygon(polygonPen.Brush, polygonPoints);
		}

		private List<Point> polygon = new List<Point>();
		private List<Point> rectangle = new List<Point>();
		private List<Point> originalPolygon;

		private static readonly Pen polygonPen = Pens.DarkCyan;
		private static readonly Pen rectanglePen = Pens.Red;

		private readonly Clipper clipper = new Clipper();
	}
}
