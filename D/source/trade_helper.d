module trade_helper;

import trade_type;
import utils;

import std.json;
import std.math;
import std.conv;
import std.math.rounding;
import std.stdio;
import std.string;
import std.file;
import std.algorithm;

import vibe.d;

static import bybit;
static import data;

bool check_canceled(string in_id){
  foreach(id; "./db/cancel_orders.txt".readText().strip().split("\n")){
    if(id == in_id) {
      return true;
    }
  }
  return false;
}

void poo(ESide side, double price, double qty){
  bybit.rest_private_post("order/create/",
    JSONValue([
      "category": "linear",
      "symbol": "BTCUSDT",
      "side": side == ESide.Long ? "Buy" : "Sell",
      "orderType": "Limit",
      "qty": qty.abs().yc_round(3).to!string,
      "price": price.to!string,
      "reduceOnly": "false",
      "timeInForce": "PostOnly",
      "positionIdx": side == ESide.Long ? "1" : "2"
    ]));
}

void pco(ESide side, double price, double qty){
    bybit.rest_private_post("order/create/",
    JSONValue([
      "category": "linear",
      "symbol": "BTCUSDT",
      "side": side == ESide.Long ? "Sell" : "Buy",
      "orderType": "Limit",
      "qty": qty.abs().yc_round(3).to!string,
      "price": price.to!string,
      "reduceOnly": "true",
      "positionIdx": side == ESide.Long ? "1" : "2"
    ]));
}

void co(string order_id) {
  bybit.rest_private_post("order/cancel/",
  JSONValue([
    "category": "linear",
    "symbol": "BTCUSDT",
    "orderId": order_id
  ]));
}

void ao(string order_id, double price) {
  bybit.rest_private_post("order/amend/",
    JSONValue([
      "category": "linear",
      "symbol": "BTCUSDT",
      "price": price.to!string,
      "orderId": order_id
    ]), order_id);
}