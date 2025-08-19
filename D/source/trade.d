module trade;

import std.json;
import std.algorithm;
import std.stdio;
import std.conv;

import trade_helper;
import trade_type;
import utils;

import eventcore.socket;

import bybit;

static import data;

//double wallet_balance = 0;
double position_long_qty = 0;
double position_short_qty = 0;
double agv_price_long = 0;
double agv_price_short = 0;

bool frist_run = true;

long realtime_reset_time = 0;

void run() nothrow
{
    import vibe.d;

    import std.file;
    import std.array;

    try
    {
        reset_callback();
        get_position_web();
        update_position();

        //get_wallet_web();
        //update_wallet_balance();

        if (data.cur_price == 0)
            return;

        if (bybit.get_unix_timestamp_long() <= realtime_reset_time)
            return;

        realtime_reset_time = bybit.get_unix_timestamp_long() + 300;

        auto realtimes = get_realtime_web2();

        if (realtimes.empty)
        {
            logInfo("realtimes is empty");
            return;
        }

        Order[string] open_orders;
        Order[string] close_orders;

        foreach (raw; realtimes)
        {
            auto json = raw.parseJSON();
            open_orders.append_hashmap(json.get_order_from(false));
            close_orders.append_hashmap(json.get_order_from(true));
        }

        Order[string] prev_open_orders;
        Order[string] prev_close_orders;

        auto files = dirEntries("./db/old/", SpanMode.shallow).array();
        if (!files.empty)
        {
            DList!(string) file_names;
            foreach (file; files)
            {
                auto json = file.name.readText().parseJSON();
                prev_open_orders.append_hashmap(json.get_order_from(false));
                prev_close_orders.append_hashmap(json.get_order_from(true));
                file_names.insertBack(file.name);
            }
            foreach (name; file_names)
            {
                import std.file;
                name.remove();
            }
        }

        int i = 0;
        foreach (raw; realtimes)
        {
            "./db/old/%s.txt".format(i++).write(raw);
        }

        foreach (cancel_order; open_orders.array()
            .filter!(order => bybit.get_unix_timestamp_long() - order.entry_time >= data
                .cancel_order_time)
            .filter!(order => !check_canceled(order.id)))
        {
            do_cancel(cancel_order);
        }

        double managed_position_long_qty = 0;
        double managed_position_short_qty = 0;

        foreach (position; prev_open_orders.array().filter!(order => order.id !in open_orders)
            .filter!(order => !check_canceled(order.id)))
        {
            logInfo("position: %s", position);
            const exit_price = position.entry_price + (
                position.entry_price * data.close_order_rate * position.side.s2double);
            pco(position.side, exit_price, position.qty);

            if (position.side == ESide.Long)
                managed_position_long_qty += position.qty;
            else
                managed_position_short_qty += position.qty;
        }

        foreach (position; close_orders)
        {
            if (position.side == ESide.Long)
                managed_position_long_qty += position.qty;
            else
                managed_position_short_qty += position.qty;

        }

        auto qty_l = position_long_qty.yc_round(3) - managed_position_long_qty.yc_round(3);
        auto qty_s = position_short_qty.yc_round(3) - managed_position_short_qty.yc_round(3);

        import std.numeric;

        if (!(qty_l.yc_round(3) == 0 && qty_s.yc_round(3) == 0))
        {
            if (bybit.get_unix_timestamp_long() - data.last_check_position_time > 1000 * 60 * 5)
            {
                data.last_check_position_time = get_unix_timestamp().to!long;
                "./db/last_check_position_time.txt".write(data.last_order_time.to!string);

                const exit_price_long = agv_price_long + (agv_price_long * data.close_order_rate);
                const exit_price_short = agv_price_short - (agv_price_short * data.close_order_rate);

                pco(ESide.Long, max(exit_price_long, data.cur_price), qty_l);
                pco(ESide.Short, min(exit_price_short, data.cur_price), qty_s);
                logInfo("[Trade -- unmanaged position] position: %s, %s", qty_l, qty_s);
            }
        }

        foreach (position; prev_close_orders.array().filter!(order => order.id !in close_orders))
        {
            string path = format("./db/close_order/%s.txt", position.id);
            position.entry_price = position.exit_price / (
                1 + data.close_order_rate * position.side.s2double());

            path.remove();

            auto profit = position.calculate_profit();
            auto open_margin = calc_margin(position.qty, data.leverage, position.entry_price, position
                    .side);
            auto profit_rate = ((profit / open_margin) - 1) * 100;

            logInfo("[수익실현] position: %s, 수익률 : %s->%s(%s%%)", position, open_margin, profit, profit_rate);
        }

        //닫기 주문 수정.
        foreach (id, position; close_orders)
        {
            if (bybit.get_unix_timestamp_long() - position.update_time < data
                .amend_order_update_rate)
                continue;

            string path = format("./db/close_order/%s.txt", id);
            double entry_price = position.exit_price / (
                1 + data.close_order_rate * position.side.s2double());
            if (path.exists)
            {
                entry_price = path.readText().to!double;
            }
            else
            {
                path.write(entry_price.to!string);
            }

            double time = (bybit.get_unix_timestamp_long() - position.entry_time) / 1000.0;
            double rate = min(time * data.close_order_time_delta, data.close_order_rate - data.close_order_rate_limit);

            if (
                (position.exit_price - data.cur_price) * position.side.s2double < data
                .close_order_amend_price_limit)
                continue;

            double exit_price = entry_price + (
                entry_price * (data.close_order_rate - rate) * position.side.s2double);

            import std.math;

            if ((position.exit_price - exit_price.yc_round(1)).abs() > 1.0)
            {
                ao(position.id, exit_price);
                logInfo("[Amended] [%s:entry_price(%s)] amended at a price to [%s]", id, entry_price, exit_price);
            }
        }

        if ((get_unix_timestamp().to!long - data.last_order_time) <= data.config.order_rate)
            return;
        if (data.wait_filled_order_count > 0)
            return;
        if (open_orders.keys.length + close_orders.keys.length > data.config.order_limit)
            return;

        data.last_order_time = get_unix_timestamp().to!long;

        "./db/last_poo_time.txt".write(data.last_order_time.to!string);

        auto pL = data.cur_price - data.config.open_order_offset;
        auto pS = data.cur_price + data.config.open_order_offset;

        auto qtyL = data.config.target_qty;
        auto qtyS = data.config.target_qty;

        poo(ESide.Long, pL, qtyL);
        poo(ESide.Short, pS, qtyS);
    }
    catch (Exception e)
    {
        logFatal("[Trade Run] error: %s", e);
    }
}

void do_cancel(Order order)
{
    static import data;
    static import bybit;
    import std.conv;

    import std.file;

    "./db/cancel_orders.txt".append(order.id ~ "\n");

    order.id.co();
}

double sum_qty(Order[string] orders, ESide side)
{
    double sum = 0;
    foreach (id, order; orders)
    {
        if (order.side == side)
            sum += order.qty;
    }
    return sum;
}

string[] get_realtime_web2(string cursor = "")
{
    import std.string;
    import std.digest.sha;
    import std.digest.hmac;
    import std.json;
    import vibe.d;

    auto json = [
        "category": "linear",
        "symbol": "BTCUSDT",
        "limit": "50",
        "cursor": cursor
    ];

    if (cursor == "")
        json.remove("cursor");

    string[] result = [];

    auto res = bybit.wait_private_get("order/realtime/", json);

    auto str = res.bodyReader.readAllUTF8();
    auto header = res.headers;

    if ("X-Bapi-Limit-Status" !in header)
    {
        logInfo("get_realtime_web2: %s", str);
        return result;
    }

    realtime_reset_time = header["X-Bapi-Limit-Status"].to!int < 3 ? bybit
        .get_unix_timestamp_long() + 1000 : realtime_reset_time;

    auto result_json = str.parseJSON();

    result ~= str;

    cursor = result_json["result"]["nextPageCursor"].str;

    if (cursor == "")
    {
        return result;
    }

    return result ~= get_realtime_web2(cursor);
}

void get_position_web()
{
    static import data;
    import std.container;
    import std.range;

    string end_point = "position/list/";

    if (end_point in bybit.rest_state_of_endpoint && (
            bybit.rest_state_of_endpoint[end_point].order_queue)[].walkLength != 0)
        return;

    bybit.rest_private_get(end_point,
        [
        "category": "linear",
        "symbol": "BTCUSDT"
    ]);
}

string wallet_callback()
{
    import std.file;
    import std.array;
    import std.format;
    import vibe.d;

    auto files = dirEntries("./db/cb/account/wallet-balance/", SpanMode.shallow).array();
    if (files.length == 0)
        return "";

    files.sort!((a, b) => a.timeCreated > b.timeCreated);
    auto file = files[0];
    string rep = file.name.readText();
    auto json = rep.parseJSON();
    if ("list" !in json["result"])
    {
        logInfo("wallet_callback: %s", rep);
        file.name.remove();
        return "";
    }

    //wallet_balance = json["result"]["list"][0]["coin"][0]["availableToWithdraw"].str().to!double;
    file.name.remove();
    return rep;
}

import std.typecons;

const JSONNULL = Nullable!JSONValue.init;
const STRNULL = Nullable!string.init;

import std.container;

void update_wallet_balance()
{
    import std.file;
    import std.array;
    import std.format;
    import vibe.d;

    auto files = dirEntries("./db/cb/account/wallet-balance/", SpanMode.shallow).array();
    if (files.length == 0)
        return;

    files.sort!((a, b) => a.timeCreated > b.timeCreated);
    auto file = files[0];
    string rep = file.name.readText();

    if ("list" !in rep.parseJSON()["result"])
    {
        logInfo("update_wallet_balance: %s", rep);
        file.name.remove();
        return;
    }

    //wallet_balance = rep.parseJSON()["result"]["list"][0]["coin"][0]["availableToWithdraw"].str().to!double;

    foreach (file2; files.filter!(x => x.name != file.name))
    {
        file2.name.remove();
    }
}

void update_position()
{
    import std.file;
    import std.array;
    import std.format;
    import vibe.d;

    auto files = dirEntries("./db/cb/position/list/", SpanMode.shallow).array();
    if (files.length == 0)
        return;

    files.sort!((a, b) => a.timeCreated > b.timeCreated);
    auto file = files[0];
    string rep = file.name.readText();

    if ("list" !in rep.parseJSON()["result"])
    {
        logInfo("update_position: %s", rep);
        file.name.remove();
        return;
    }

    position_long_qty = rep.parseJSON()["result"]["list"][0]["size"].str().to!double;
    position_short_qty = rep.parseJSON()["result"]["list"][1]["size"].str().to!double;
    agv_price_long = rep.parseJSON()["result"]["list"][0]["avgPrice"].str().to!double;
    agv_price_short = rep.parseJSON()["result"]["list"][1]["avgPrice"].str().to!double;

    foreach (file2; files.filter!(x => x.name != file.name))
    {
        file2.name.remove();
    }
}

void reset_callback()
{
    import std.file;
    import std.array;
    import std.format;
    import vibe.d;

    auto amend_files = dirEntries("./db/cb/order/amend/", SpanMode.shallow).array();
    auto cancel_files = dirEntries("./db/cb/order/cancel/", SpanMode.shallow).array();
    auto create_files = dirEntries("./db/cb/order/create/", SpanMode.shallow).array();

    foreach (file; amend_files)
    {
        file.name.remove();
    }
    foreach (file; cancel_files)
    {
        file.name.remove();
    }
    foreach (file; create_files)
    {
        //string rep = file.name.readText();
        //if(rep.parseJSON()["retMsg"].str() == "OK"){
        file.name.remove();
        //}
    }
}
