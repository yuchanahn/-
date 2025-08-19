module utils;
import std.math;
import std.conv;
import std.math.rounding;
import std.typecons;
import std.container.dlist;

double yc_round(double value, int precision) {
    return (value * 10^^precision).round / 10^^precision;
}

DList!T to_list(T)(T[] arr)
{
	DList!T list = DList!T();
	foreach (a; arr)
	{
		list.insertBack(a);
	}
	return list;
}

