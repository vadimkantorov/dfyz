using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;

namespace Task3
{
	public class Clipper
	{
		public List<Point> Clip(List<Point> polygon, List<Point> rectangle)
		{
			var result = polygon;
			foreach (var clipEdge in GetEdges(rectangle))
			{
				var newResult = new List<Point>();
				var s = result.Last();
				foreach (var p in result)
				{
					Action addIntersection = () => newResult.Add(clipEdge.GetIntersection(s, p));
					bool sInside = clipEdge.IsInside(s);
					if (clipEdge.IsInside(p))
					{
						if (!sInside)
							addIntersection();
						newResult.Add(p);
					}
					else if (sInside)
						addIntersection();
					s = p;
				}
				result = newResult;
			}
			return result;
		}

		private static IEnumerable<ClipEdge> GetEdges(IEnumerable<Point> rectangle)
		{
			var topLeft = rectangle.First();
			var bottomRight = rectangle.Last();
			yield return new ClipEdge
			{
				IsInside = p => p.Y >= topLeft.Y,
				GetIntersection = (p0, p1) =>
				{
					double yDiff = p1.Y - p0.Y;
					var t = (topLeft.Y - p0.Y) / yDiff;
					return new Point((int) (p0.X + t * (p1.X - p0.X)), topLeft.Y);
				},
			};
			yield return new ClipEdge
			{
				IsInside = p => p.X >= topLeft.X,
				GetIntersection = (p0, p1) =>
				{
					double xDiff = p1.X - p0.X;
					var t = (topLeft.X - p0.X) / xDiff;
					return new Point(topLeft.X, (int) (p0.Y + t * (p1.Y - p0.Y)));
				},
			};
			yield return new ClipEdge
			{
				IsInside = p => p.Y <= bottomRight.Y,
				GetIntersection = (p0, p1) =>
				{
					double yDiff = p1.Y - p0.Y;
					var t = (bottomRight.Y - p0.Y) / yDiff;
					return new Point((int) (p0.X + t * (p1.X - p0.X)), bottomRight.Y);
				},
			};
			yield return new ClipEdge
			{
				IsInside = p => p.X <= bottomRight.X,
				GetIntersection = (p0, p1) =>
				{
					double xDiff = p1.X - p0.X;
					var t = (bottomRight.X - p0.X) / xDiff;
					return new Point(bottomRight.X, (int) (p0.Y + t * (p1.Y - p0.Y)));
				},
			};
		}

		private class ClipEdge
		{
			public Func<Point, bool> IsInside;
			public Func<Point, Point, Point> GetIntersection;
		}
	}
}