module webserver;

string function(string[])[string] command_map;

static this()
{
  import std.conv;
  import std.format;
  import std.json;
  import vibe.d;
  static import bybit;
  static import data;

  webserver.command_map["p"] = (parms) {
    return "BTCUSDT price: $" ~ data.cur_price.to!string;
  };
}

void start_server() nothrow
{
  import std.algorithm;
  import std.array;
  import std.file;
  import std.json;
  import vibe.d;
  import vibe.http.router;
  static import data;

  try
  {
    HTTPListener http_listener;

    auto settings = new HTTPServerSettings();
    settings.port = 80;

    auto router = new URLRouter();

    router.get("/", (HTTPServerRequest req, HTTPServerResponse res) {
      res.contentType = "text/html; charset=utf-8";
      res.writeBody(readText("frontend/index.html"));
    });

    router.get("/getlog", (HTTPServerRequest req, HTTPServerResponse res) {
      auto log_files = dirEntries("./logs/", SpanMode.shallow).array();
      if(log_files.length == 0) return;
      log_files.sort!((a, b) => a.timeCreated > b.timeCreated);
      auto file = log_files[0];
      res.contentType = "application/octet-stream";
      res.headers["Content-Disposition"] = format("attachment; filename=\"%s\"", file.name.split("/").back);
      res.writeBody(file.name.readText());
    });

    router.post("/cmd", (HTTPServerRequest req, HTTPServerResponse res) {
      auto body_str = req.bodyReader.readAllUTF8();
      auto cmd = body_str.split(" ")[0];
      if(cmd in command_map)
      {
        auto parms = body_str.split(" ")[1..$];
        auto cmd_result = command_map[cmd](parms);
        res.writeBody(cmd_result);
      }
      else
      {
        res.writeBody(format("unknown command: %s", body_str));
      }
    });

    router.get("/ws", handleWebSockets((scope WebSocket sock) nothrow {
      try {
        logInfo("[webserver] websocket connected");

        auto run_timer = true;
        auto timer_handle = setTimer(500.msecs, () {
          sock.send(JSONValue([
            "cur_price": data.cur_price,
            "target_qty": data.config.target_qty,
            "order_limit": data.config.order_limit,
            "order_rate": data.config.order_rate,
            "open_order_offset": data.config.open_order_offset,
            "leverage": data.leverage,
          ]).to!string);
        }, true);
        scope(exit) timer_handle.stop();

        while (sock.connected) {
          if (!run_timer) {
            timer_handle.stop();
          }
          sleep(1.seconds);
        }

        logInfo("[webserver] websocket disconnected");
      } catch (Exception e) {
        logFatal("[webserver] handleWebSockets exception: %s", e);
      }
    }));

    http_listener = listenHTTP(settings, router);
  }
  catch (Exception e)
  {
    logFatal("[webserver] webserver exception: %s", e);
  }
}
