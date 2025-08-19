import std.stdio;
import std.conv;
import std.regex;
import std.file;
import std.algorithm;
import std.string;
import std.complex;
import std.bitmanip;
import std.typecons;

import std.math.rounding;

import std.container.dlist;

import core.stdcpp.array;
import core.stdc.stdio;

import utils;

//config

const double limit_fee = 0.0002;
const double market_fee = 0.00045;
const double start_money = 60;

double equity = start_money;
double available_balance = start_money;
double leverage = 5;
double order_limit = 0.001;
double close_order_rate = 0.04;
double order_offset = 1.0f;
int limit = 40;

struct TradeConfig
{
	double leverage;

	double order_offset;

	double order_limit;
	double close_order_rate;
	int limit;
}

enum EPositionType
{
	LONG,
	SHORT
}

enum EOrderType
{
	OPEN,
	CLOSE
}

struct Position
{
	double margin;
	double qty;
	double entry_price;
	EPositionType type;

	void add_qty_margin(double qty, double price)
	{
		margin += calc_im(qty, leverage, price, type);
		this.qty += qty;
		entry_price = (entry_price * (this.qty - qty) + price * qty) / this.qty;

		auto value = price * qty;
		margin -= value * market_fee;
		available_balance += value * market_fee - value * limit_fee;
	}

	void remove_qty_margin(double qty)
	{
		auto value = entry_price * qty;
		margin -= calc_im(qty, leverage, entry_price, type) - value * market_fee;
		this.qty -= qty;

		if (margin < 0)
			margin = 0;

		if (this.qty == 0)
			entry_price = 0;
	}

	double evaluate_pnl_margin(Order order)
	{
		auto pnl = calc_pnl(entry_price, order.target_price, order.qty, type);
		auto open_value = order.qty * entry_price;
		auto close_value = order.qty * order.target_price;
		auto margin = calc_im(order.qty, leverage, entry_price, type);
		margin -= open_value * market_fee;
		auto pnl_and_margin = pnl + margin - (close_value * limit_fee);
		remove_qty_margin(order.qty);
		return pnl_and_margin;
	}

	double liq_price()
	{
		auto imr = 1 / leverage;
		auto mmr = 0.005;
		if (type == EPositionType.LONG)
			return entry_price * (1 - imr + mmr);
		else
			return entry_price * (1 + imr - mmr);
	}

	double pnl(double price)
	{
		if (entry_price == 0)
			return 0;
		return calc_pnl(entry_price, price, qty, type);
	}

	string to_string(double price)
	{
		return "[%s]margin(%s)|qty(%s)|e.price(%s)|liq.price(%s)|pnl(%s)(%s%%)".format(
			type.to_string,
			margin,
			qty,
			entry_price,
			liq_price(),
			pnl(price),
			pnl(price) / margin * 100);
	}

	bool is_liq(double price)
	{
		if (entry_price == 0)
			return false;

		return type == EPositionType.LONG ? price <= liq_price() : price >= liq_price();
	}
}

struct Order
{
	EOrderType order_type;
	double target_price;
	double qty;
	long order_start_time;
	EPositionType type;
}

struct TradeResult{
	Position plong;
	Position pshort; 
	DList!Order order_list;
	int long_liq;
	int short_liq;
}

double to_double(EPositionType type)
{
	if (type == EPositionType.LONG)
		return 1;
	else
		return -1;
}

string to_string(EPositionType type)
{
	if (type == EPositionType.LONG)
		return "LONG";
	else
		return "SHORT";
}

bool is_long(EPositionType type)
{
	return type == EPositionType.LONG;
}

bool is_short(EPositionType type)
{
	return type == EPositionType.SHORT;
}

double calc_im(double qty, double leverage, double price, EPositionType side)
{
	const double value = price * qty;
	const double initial_margin = value / leverage;

	double close_price = price * (leverage - 1 * side.to_double()) / leverage;

	return initial_margin + (value * market_fee) + (close_price * qty * market_fee);
}

double calc_pnl(double entry_price, double close_price, double qty, EPositionType side)
{
	return (close_price - entry_price) * qty * side.to_double;
}

auto close_order_trigger_list = DList!Order();

void process_tread(ref TradeResult result, double cur_price)
{
	auto inital_margin1 = calc_im(order_limit, leverage, cur_price - order_offset, EPositionType.LONG);
	auto inital_margin2 = calc_im(order_limit, leverage, cur_price + order_offset, EPositionType.SHORT);

	import std.array;

	if (result.order_list.array().length < limit && available_balance > inital_margin1 + inital_margin2)
	{
		available_balance -= inital_margin1;
		available_balance -= inital_margin2;

		result.order_list.insertBack(
			Order(EOrderType.OPEN, cur_price + order_offset, order_limit, 0, EPositionType.SHORT));
		result.order_list.insertBack(
			Order(EOrderType.OPEN, cur_price - order_offset, order_limit, 0, EPositionType.LONG));
	}

	if (result.pshort.is_liq(cur_price))
	{
		//"short liquidated".writeln;
		result.pshort = Position(0, 0, 0, EPositionType.SHORT);
		result.order_list = result.order_list.array().filter!((a) { return a.type.is_long; })
			.DList!Order();
		result.short_liq++;
	}
	if (result.plong.is_liq(cur_price))
	{
		//"long liquidated".writeln;
		result.plong = Position(0, 0, 0, EPositionType.LONG);
		result.order_list = result.order_list.array().filter!((a) { return a.type.is_short; })
			.DList!Order();
		result.long_liq++;
	}

	auto order_list = result.order_list.array();
	
	foreach (order; order_list
		.filter!((a) { return a.order_type == EOrderType.OPEN; })
		.filter!((a) { return (a.type.is_long ? (cur_price <= a.target_price) : (cur_price >= a.target_price)); }))
	{
		result.order_list.insertBack(
			Order(EOrderType.CLOSE, (order.target_price * (1 + close_order_rate * order.type.to_double())), order_limit, 0, order.type));
		(order.type.is_long ? result.plong : result.pshort).add_qty_margin(order_limit, order.target_price);
	}
	
	foreach (order; order_list
		.filter!((a) { return a.order_type == EOrderType.CLOSE; })
		.filter!((a) { return (a.type.is_long ? (cur_price >= a.target_price) : (cur_price <= a.target_price)); }))
	{
		available_balance += (order.type.is_long ? result.plong : result.pshort).evaluate_pnl_margin(order);
	}

	result.order_list = result.order_list.array()
		.filter!((a) { 
			return (a.order_type != EOrderType.OPEN) || 
				(a.type.is_long ? (cur_price > a.target_price) : (cur_price < a.target_price));
			})
		.filter!((a) { 
			return (a.order_type != EOrderType.CLOSE) || 
				(a.type.is_long ? (cur_price < a.target_price) : (cur_price > a.target_price));
			})
		.DList!Order();

	auto new_equity = available_balance + result.plong.margin + result.plong.pnl(
		cur_price) + result.pshort.margin + result.pshort.pnl(cur_price);
	equity = new_equity;
}

void main()
{
	import std.string;

	auto datas = [
		"data/BTCUSDT-Kline-MINUTE-Perpetual-2023-12.csv".readText().strip(),
		"data/BTCUSDT-Kline-MINUTE-Perpetual-2024-01.csv".readText().strip(),
		"data/BTCUSDT-Kline-MINUTE-Perpetual-2024-02.csv".readText().strip(),
		"data/BTCUSDT-Kline-MINUTE-Perpetual-2024-03.csv".readText().strip()
	];

	auto str = (datas[0]~datas[1]~datas[2]~datas[3]).split("\n");
	
	double cur_price = 0;

	auto config = [
		// leverage, order_offset, order_limit, close_order_rate, limit
		TradeConfig(5, 1, 0.02, 0.01, 1000),
		TradeConfig(5, 1, 0.02, 0.02, 1000),
		TradeConfig(5, 1, 0.02, 0.03, 1000),
		TradeConfig(5, 1, 0.02, 0.04, 1000),
	];

	double start_money = 10000;

	int number = 0;
	auto price_cache = new double[](0);

	foreach (price; str[1 .. $])
	{
		foreach (price2; price.split(",")[1 .. 4])
		{
			price_cache ~= price2.to!double;
		}
	}

	foreach (data; config)
	{
		equity = start_money;
		available_balance = start_money;

		double start_equity = equity;
		double end_equity = equity;
		double max_equity = equity;
		double min_equity = equity;
		TradeResult result = TradeResult(
			Position(0, 0, 0, EPositionType.LONG), 
			Position(0, 0, 0, EPositionType.SHORT), 
			DList!Order(), 
			0, 0);

		leverage = data.leverage;
		order_limit = data.order_limit;
		close_order_rate = data.close_order_rate;
		limit = data.limit;
		order_offset = data.order_offset;

		auto count = 0;

		foreach (price; price_cache)
		{
			cur_price = price.to!double;
			
			process_tread(result, cur_price);

			end_equity = equity;
			if (end_equity > max_equity)
			max_equity = end_equity;
			if (end_equity < min_equity)
			min_equity = end_equity;



			if(++count % (4 * 60 * 24 * 7) == 0){
				"[%s]%s|end : %s\t|max : %s\t|min : %s|liq : (l:%s, s:%s)".format(
				number, 
				start_equity.yc_round(1), 
				end_equity.yc_round(1), 
				max_equity.yc_round(1), 
				min_equity.yc_round(1),
				result.long_liq,
				result.short_liq
				)
				.writeln;
				//result.plong.to_string(cur_price).writeln;
				//result.pshort.to_string(cur_price).writeln;
			}
		}

		"[%s]%s|end : %s\t|max : %s\t|min : %s |liq : (l:%s, s:%s)\n".format(
			number++, 
			start_equity.yc_round(1), 
			end_equity.yc_round(1), 
			max_equity.yc_round(1), 
			min_equity.yc_round(1),
			result.long_liq,
			result.short_liq
			)
			.writeln;
	}
}
