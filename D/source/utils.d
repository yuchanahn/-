module utils;

import trade_type;

import std.json;
import std.math;
import std.conv;
import std.math.rounding;

static import data;


double yc_round(double value, int precision) {
    return (value * 10^^precision).round / 10^^precision;
}

double s2double(ESide side){
  return side == ESide.Long ? 1 : -1;
}

double calc_fee(double op, double cp, double qty, double fee) {
  const double open_fee = (op * qty) * fee;
  const double close_fee = (cp * qty) * fee;

  return open_fee + close_fee;
}

double calc_margin(double qty, double leverage, double price, ESide side) {
  const double initial_margin = (price * qty) / leverage;
  const double close_price = price * (leverage - 1 * side.s2double()) / leverage;

  return initial_margin + calc_fee(price, close_price, qty, data.market_fee);
}

double calculate_profit(Order order) {
  double open_fee = order.entry_price * order.qty * data.limit_fee;
  double close_fee = order.exit_price * order.qty * data.limit_fee;

  const double initial_margin = (order.entry_price * order.qty) / data.leverage;
  const double base_profit = (order.exit_price - order.entry_price) * order.side.s2double() * order.qty;
  
  return initial_margin + base_profit - open_fee - close_fee;
}

Order[string] get_order_from(JSONValue json, bool reduce_only){
  Order[string] r;
  
  foreach (order; json["result"]["list"].array) {
    
    if(reduce_only != order["reduceOnly"].boolean()) continue;

    auto order_id = order["orderId"].str();
    auto side = order["side"].str() == (reduce_only ? "Sell" : "Buy") ? ESide.Long : ESide.Short;
    auto qty = order["qty"].str().to!double;
    auto price = order["price"].str().to!double;
    auto time = order["createdTime"].str().to!long;
    auto update_time = order["updatedTime"].str().to!long;

    Order o;
    o.id = order_id;
    o.side = side;
    o.qty = qty;
    
    if(reduce_only) {
      o.exit_price = price;
      o.update_time = update_time;
    } else {
      o.entry_price = price;
    }

    o.entry_time = time;

    r[order_id] = o;
  }

  return r;
}

void append_hashmap(ref Order[string] orders, Order[string] new_orders){
  foreach(k, v; new_orders){
    orders[k] = v;
  }
}