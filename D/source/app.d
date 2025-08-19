import std.typecons;
import std.json;
import vibe.d;
import vibe.core.log;

static import trade;
static import data;
static import bybit;
static import bybit_url;
static import webserver;
static import trade_helper;
import core.stdcpp.exception;

int main(string[] args)
{
  import std.conv;
  import std.format;

  bybit.wss_topic_handler["kline.1.BTCUSDT"] = (json) {
    data.cur_price = json["data"][0]["close"].str().to!double;
  };

  webserver.command_map["/c"] = (parms) {
    switch(parms[0])
    {
      case "order_limit":
        data.config.order_limit = parms[1].to!int;
        break;
      case "order_rate":
        data.config.order_rate = parms[1].to!long;
        break;
      case "qty":
        data.config.target_qty = parms[1].to!double;
        break;
      case "offset":
        data.config.open_order_offset = parms[1].to!double;
        break;
      case "shutdown":
        import core.stdc.stdlib;
        exit(0);
        break;
      default:
        return "Invalid command";
    }
    data.update_config();
    return "OK";
  };

  const serverTaskHandle = runTask(&webserver.start_server);
  const clientTaskHandle = runTask(() {
    bybit.wss_loop(bybit_url.url_wss_public_linear, (recv_text) {});
  });

  //실행후 한 시간 뒤 종료
  import core.stdc.stdlib;
  
  const exitTaskHandle = runTask(() { 
    try { 
      sleep(dur!("hours")(1)); 
      exit(0);
    } catch (Exception e) {
      logInfo("init error: %s", e.msg);
    }
  });
  
  while(1) {
    trade.run();
    bybit.run_rest_order_queue();
  }

  const appResult = runApplication(&args);
  return appResult;
}
