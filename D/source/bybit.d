module bybit;

import std.json;
import vibe.d;
static import data;
static import bybit_url;

private long server_time_offset = 0;


const bool[string] no_log = [ "order/realtime/" : true, "account/wallet-balance/" : true, "position/list/" : true];


void function(JSONValue json)[string] wss_topic_handler;

static this()
{
  sync_server_time();
}

void sync_server_time()
{
  requestHTTP(
    URL(bybit_url.url_rest ~ "market/time"),
    (scope req) {
      req.method = HTTPMethod.GET;
    },
    (scope res) {
      immutable json = res.bodyReader.readAllUTF8().parseJSON();
      immutable server_time = json["result"]["timeNano"].str().to!long / 1_000_000;

      logInfo("[bybit rest api] server time: %s", server_time);
      server_time_offset = (Clock.currTime() - SysTime(unixTimeToStdTime(0))).total!"msecs" - server_time;
    });
}


long get_unix_timestamp_long()
{
  return ((Clock.currTime() - SysTime(unixTimeToStdTime(0))).total!"msecs" - server_time_offset).to!long;
}

string get_unix_timestamp()
{
  return ((Clock.currTime() - SysTime(unixTimeToStdTime(0))).total!"msecs" - server_time_offset).to!string;
}



void wss_loop(string url, void function(string) on_data_recv) nothrow
{
  while (true)
  {
    try
    {
      logInfo("[bybit wss api] connecting... (%s)", url);

      auto ws = connectWebSocket(URL(url));
      logInfo("[bybit wss api] connected! (%s)", url);

      // heartbeat
      auto heartbeat_handle = setTimer(10.seconds, () {
        logInfo("[bybit wss api] send ping (heartbeat)");
        ws.send(`{"op": "ping"}`);
      }, true);
      scope(exit) heartbeat_handle.stop();

      // subscribe topics
      ws.send(`{"op":"subscribe","args":["kline.1.BTCUSDT"]}`);

      // recive loop
      while (ws.connected())
      {
        if (ws.waitForData())
        {
          auto recv_text = ws.receiveText();
          on_data_recv(recv_text);

          auto is_json = true;
          JSONValue json;

          try
          {
            json = parseJSON(recv_text);
          }
          catch (Exception e)
          {
            is_json = false;
            logInfo("[bybit wss api] non json data recived: %s", recv_text);
          }

          if (is_json)
          {
            if ("topic" in json)
            {
              auto topic_str = json["topic"].str();
              if (topic_str in wss_topic_handler)
                wss_topic_handler[topic_str](json);
            }
            else if ("op" in json)
            {
              switch (json["op"].str())
              {
                case "ping":
                  logInfo("[bybit wss api] ping response: %s", json);
                  break;
                case "subscribe":
                  logInfo("[bybit wss api] subscribe response: %s", json);
                  break;
                default:
                  logInfo("[bybit wss api] json recived: %s", json);
              }
            }
            else
            {
              logInfo("[bybit wss api] json recived: %s", json);
            }
          }
        }
        else
        {
          ws.close();
          logFatal("[bybit wss api] connection lost!");
          break;
        }
      }
    }
    catch (Exception e)
    {
      logFatal("[bybit wss api] wss_loop exception: %s", e);
    }
  }
}

import std.container;
import std.range;
import std.internal.digest.sha_SSSE3;
import std.regex;

struct RestState {
  long reset_time = 0;
  int limit = 10;
  bool wait_response = false;
  DList!(void delegate()) order_queue;
}

RestState[string] rest_state_of_endpoint;

void run_rest_order_queue()
{
  foreach (key, state; rest_state_of_endpoint) {
    if (state.order_queue.empty) continue;
    if (state.wait_response) {
      //logInfo("[wait response] %s", key);
    }
    if (state.limit <= 2 && (get_unix_timestamp().to!long < state.reset_time + (state.limit ? 1 : 1000))) continue;

    import std.stdio;
    import std.container;

    if(key == "account/wallet-balance/") { 
      //logInfo("[wallet balance header] %s", state);
    }

    rest_state_of_endpoint[key].wait_response = true;

    for (int i = 0; i < state.limit - 2; i++)
    {
      state.order_queue.front()();
      state.order_queue.removeFront();
      if (state.order_queue.empty) break;
    }

    if(key !in no_log) logInfo("[Run queue] %s : %s", key, state.order_queue[].walkLength);
  }
}

void rest_private_get(string end_point, string[string] json, bool front = false, string file_name = ""){
  if(end_point !in rest_state_of_endpoint)
    rest_state_of_endpoint[end_point] = RestState();

  if(front){
    rest_state_of_endpoint[end_point].order_queue.insertFront(() {
      impl_rest_private_get(end_point, json, file_name);
    });
    return;
  }
  rest_state_of_endpoint[end_point].order_queue.insertBack(() {
    impl_rest_private_get(end_point, json, file_name);
  });
}

void impl_rest_private_get(string end_point, string[string] json, string file_name = ""){
  import std.string;
  import std.digest.sha;
  import std.digest.hmac;

  auto query_params = "";
  foreach (kv; json.byKeyValue)
  {
    query_params ~= format("%s=%s&", urlEncode(kv.key), urlEncode(kv.value));
  }
  query_params = query_params.stripRight("&");

  auto url = bybit_url.url_rest ~ end_point;
  if (!json.empty())
  {
    url ~= "?" ~ query_params;
  }

  immutable timestamp = get_unix_timestamp();
  immutable recvWindow = "5000";
  immutable param_str = format("%s%s%s%s", timestamp, data.public_key, recvWindow, query_params);
  immutable sign =
    HMAC!SHA256(data.private_key.representation)
    .put(param_str.representation)
    .finish()
    .toHexString();

  requestHTTP(
    URL(url),
    (scope req) {
      req.headers["X-BAPI-SIGN-TYPE"] = "2";
      req.headers["X-BAPI-SIGN"] = sign;
      req.headers["X-BAPI-API-KEY"] = data.public_key;
      req.headers["X-BAPI-TIMESTAMP"] = timestamp;
      req.headers["X-BAPI-RECV-WINDOW"] = recvWindow;

      //if(end_point !in no_log) logInfo("[bybit rest api] send: \"%s?%s\"", end_point, query_params);
    },
    (scope res) {
      auto str = res.bodyReader.readAllUTF8();
      auto header = res.headers;




      if(end_point != "account/wallet-balance/"){
        if("X-Bapi-Limit-Status" !in header) {
          rest_private_get(end_point, json, true, file_name);
          return;
        }

        rest_state_of_endpoint[end_point].limit = header["X-Bapi-Limit-Status"].to!int;
        rest_state_of_endpoint[end_point].reset_time = header["X-Bapi-Limit-Reset-Timestamp"].to!long + 50;
        rest_state_of_endpoint[end_point].wait_response = false;
      }
      if(str.parseJSON()["retMsg"].str() != "OK") {
        logInfo("[bybit rest api] header: \"%s\" %s", end_point, header);
        logInfo("[bybit rest api] response: \"%s\" %s", end_point, str);
        return;
      }
      
      import std.uuid;
      import std.file;

      if (!exists("./db/cb"))
      {
        mkdir("./db/cb");
      }
      if (!exists(format("./db/cb/%s", end_point.split("/")[0])))
      {
        mkdir(format("./db/cb/%s", end_point.split("/")[0]));
      }
      if (!exists(format("./db/cb/%s", end_point)))
      {
        mkdir(format("./db/cb/%s", end_point));
      }

      "./db/cb/%s%s.txt".format(end_point, (file_name != "") ? file_name : randomUUID.to!string)
                        .write(str);
    });
}


HTTPClientResponse wait_private_get(string end_point, string[string] json) {
  import std.string;
  import std.digest.sha;
  import std.digest.hmac;
  import std.json;
  import vibe.d;
  static import data;
  static import bybit_url;
  
  auto query_params = "";
  
  foreach (kv; json.byKeyValue)
  {
    query_params ~= format("%s=%s&", urlEncode(kv.key), urlEncode(kv.value));
  }
  query_params = query_params.stripRight("&");

  auto url = bybit_url.url_rest ~ end_point;
  if (!json.empty())
  {
    url ~= "?" ~ query_params;
  }

  immutable timestamp = get_unix_timestamp();
  immutable recvWindow = "5000";
  immutable param_str = format("%s%s%s%s", timestamp, data.public_key, recvWindow, query_params);
  immutable sign =
    HMAC!SHA256(data.private_key.representation)
    .put(param_str.representation)
    .finish()
    .toHexString();
  
  HTTPClientResponse res = requestHTTP(URL(url), (scope req) {
      req.headers["X-BAPI-SIGN-TYPE"] = "2";
      req.headers["X-BAPI-SIGN"] = sign;
      req.headers["X-BAPI-API-KEY"] = data.public_key;
      req.headers["X-BAPI-TIMESTAMP"] = timestamp;
      req.headers["X-BAPI-RECV-WINDOW"] = recvWindow;
    });

  return res;
}

auto print_json = false;
bool[string] flags;

import trade_type;

void rest_private_post(string end_point, JSONValue json, string flag = "", bool front = false) {
  if(end_point !in rest_state_of_endpoint)
    rest_state_of_endpoint[end_point] = RestState();
 
  if((flag != "") && (flag in flags)) return;

  flags[flag] = true;

  //logInfo("[bybit rest api] [%s] flag: %s", end_point, flag);

  if(front){
    rest_state_of_endpoint[end_point].order_queue.insertFront(() {
      impl_rest_private_post(end_point, json, flag);
      if(flag in flags) flags.remove(flag);
    });
    return;
  }
  rest_state_of_endpoint[end_point].order_queue.insertBack(() {
    impl_rest_private_post(end_point, json, flag);
    if(flag in flags) flags.remove(flag);
  });
}

void impl_rest_private_post(string end_point, JSONValue json, string flag = "")
{
  import std.digest.sha;
  import std.digest.hmac;

  immutable timestamp = get_unix_timestamp();
  immutable recvWindow = "5000";
  immutable json_str = json.toString();
  immutable param_str = format("%s%s%s%s", timestamp, data.public_key, recvWindow, json_str);
  immutable sign =
    HMAC!SHA256(data.private_key.representation)
    .put(param_str.representation)
    .finish()
    .toHexString();

  requestHTTP(
    URL(bybit_url.url_rest ~ end_point),
    (scope req) {
      req.method = HTTPMethod.POST;
      req.headers["X-BAPI-SIGN-TYPE"] = "2";
      req.headers["X-BAPI-SIGN"] = sign;
      req.headers["X-BAPI-API-KEY"] = data.public_key;
      req.headers["X-BAPI-TIMESTAMP"] = timestamp;
      req.headers["X-BAPI-RECV-WINDOW"] = recvWindow;
      req.headers["Content-Type"] = "application/json";
      req.writeBody(json_str.representation);

      //if(end_point !in no_log) logInfo("[bybit rest api] send: \"%s\" %s", end_point, json.toString());
    },
    (scope res) {
      auto str = res.bodyReader.readAllUTF8();
      auto header = res.headers;


      


      rest_state_of_endpoint[end_point].wait_response = false;

      if("X-Bapi-Limit-Status" !in header) {
        rest_private_post(end_point, json, flag, true);
        return;
      }

      rest_state_of_endpoint[end_point].limit = header["X-Bapi-Limit-Status"].to!int;
      rest_state_of_endpoint[end_point].reset_time = header["X-Bapi-Limit-Reset-Timestamp"].to!long + 50;

      import std.uuid;
      import std.file;

      if (!exists("./db/cb"))
      {
        mkdir("./db/cb");
      }
      if (!exists(format("./db/cb/%s", end_point.split("/")[0])))
      {
        mkdir(format("./db/cb/%s", end_point.split("/")[0]));
      }
      if (!exists(format("./db/cb/%s", end_point)))
      {
        mkdir(format("./db/cb/%s", end_point));
      }

      format("./db/cb/%s%s.txt", end_point, randomUUID).write(str);
      if(str.parseJSON()["retMsg"].str() != "OK") {
        if(end_point !in no_log) logInfo("[bybit rest api] header: \"%s\" %s", end_point, header);
        if(end_point !in no_log) logInfo("[bybit rest api] response: \"%s\" %s", end_point, str);
      }
    });
}