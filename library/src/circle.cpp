#include "circle.hpp"
#include "polygon.hpp"

#include <cmath>
#include <iostream>
#include <limits>

Circle::Circle(const Precision_t &r) : Shape(), _radius(r)
{
	_center = Vector2(r, r);
}

const Projection Circle::Project(const Axis &a) const
{
	Precision_t v = a.Dot(GetPos());
	return Vector2(v - GetRadius(), v + GetRadius());
}

const Collision Circle::GetCollision(const Circle &c) const
{

	Vector2 v = c.GetPos() - GetPos();

	Precision_t radiiSum = GetRadius() + c.GetRadius();
	Precision_t dist = v.Magnitude();
	Precision_t tDist = radiiSum - dist;

	Precision_t theta = atan2(v.y, v.x);

	Precision_t x = tDist * std::cos(theta);
	Precision_t y = tDist * std::sin(theta);

	Vector2 translation(x, y);

	if (translation.Dot(v) < 0)
		translation = -translation;

	Precision_t radiiDif = std::abs(GetRadius() - c.GetRadius());
	bool contained = (dist <= radiiDif);
	bool overlap = (dist < radiiSum);

	return Collision(translation, overlap, contained);
}

const Collision Circle::GetCollision(const Polygon &p) const
{
	Precision_t Overlap = std::numeric_limits<Precision_t>::infinity();// really large value;
	Axis smallest;

	Vector2 a = p.NearestVertex(GetPos());
	Vector2 b = p.GetCenter() + p.GetPos();
	Axis ax = a - b;
	ax = ax.Perpendicular().Normal();


	AxesVec axes = p.GetAxes();
	axes.push_back(ax);

	bool contained = false;
	Vector2 translation;

	for (auto && axis : axes)
	{
		const Projection pA = p.Project(axis);
		const Projection pB = Project(axis);

		if (!pA.IsOverlap(pB))
		{
			return Collision(translation, false, false);
		}

		else
		{
			Precision_t o = pA.GetOverlap(pB);

			contained = Contains(p);

			if (contained)
			{
				Precision_t mins = std::abs(pA.x - pB.x);
				Precision_t maxs = std::abs(pA.y - pB.y);

				if (mins < maxs)
					o += mins;

				else
					o += maxs;
			}

			if (o < Overlap)
			{
				Overlap = o;
				smallest = axis;
			}
		}
	}

	Precision_t dist = a.GetDistance(GetCenter());

	contained = (Contains(GetCenter()) && GetRadius() < dist);

	translation = smallest * (Overlap + 1);
	Vector2 distance = a - b;

	if (translation.Dot(distance) < 0)
		translation = -translation;

	return Collision(translation, true, contained);
}

const Precision_t& Circle::GetRadius() const
{
	return _radius;
}

const bool Circle::Contains(const Vector2 &p) const
{
	const Vector2 v = GetPos() - p;
	const Precision_t dist = v.Magnitude();

	return (dist < GetRadius());
}

const bool Circle::Contains(const Polygon &p) const
{
	for (auto && pt : p.GetPoints())
	{
		if (!Contains(pt + p.GetPos()))
			return false;
	}

	return true;
}