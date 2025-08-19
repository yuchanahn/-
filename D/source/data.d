module data;

import trade_type;
import std.container;
import core.sys.posix.unistd;
import vibe.d;

auto private_key = "";
auto public_key = "";

auto cur_price = 0.0;

auto cancel_order_delta = 0.05; //@
const close_order_rate = 0.01; //@
const close_order_time_delta = 0.00001; // 1초당, 1%씩 //@
const close_order_rate_limit = 0.003; //@

const limit_fee = 0.0002;
const market_fee = 0.00055;


auto leverage = 25.0; //계속 보여주기

long last_order_time = 0;
long last_check_position_time = 0;
long wait_filled_order_count = 0;

long cancel_order_time = 1000 * 60 * 5; // 5분
double close_order_amend_price_limit = 10.0;
long amend_order_update_rate = 1000 * 5; // 5초


// config data
struct TradeConfig {
  double target_qty = 0.005;
  int order_limit = 30;
  long order_rate = 1000 * 60 / 4;
  double open_order_offset = 1.5;
}
TradeConfig config = TradeConfig();

void load_config()
{
  import std.conv;
  import std.string;
  import std.file;
  
  auto r = ("./db/trade_config.txt".readText().strip().to!string).split("\n");

  config.target_qty = r[0].to!double;
  config.order_limit = r[1].to!int;
  config.order_rate = r[2].to!long;
  config.open_order_offset = r[3].to!double;
}

void update_config()
{
  import std.conv;
  import std.string;
  import std.file;

  "./db/trade_config.txt".write(
    format("%s\n%s\n%s\n%s",
      config.target_qty,
      config.order_limit,
      config.order_rate,
      config.open_order_offset
    )
  );
}

static this()
{
  import std.conv;
  import std.string;
  import std.file;
  import std.datetime;
  import std.datetime.timezone;
  import vibe.d;

  if (!exists("./db/"))
  {
    mkdir("./db/");
    "./db/cancel_orders.txt".write("");
    "./db/last_poo_time.txt".write("0");
  }

  if (!exists("./logs/"))
  {
    mkdir("./logs/");
  }

  immutable korTimeZone = new SimpleTimeZone(9.hours(), "KST");
  immutable currentTime = Clock.currTime(korTimeZone);
  immutable currentTimeStr = format(
    "logs/[%s-%02s-%02s][%02sh_%02sm_%02ss].log",
    currentTime.year,
    currentTime.month.to!int,
    currentTime.day,
    currentTime.hour,
    currentTime.minute,
    currentTime.second
  );

  setLogFile(currentTimeStr, LogLevel.diagnostic);
  setLogFormat(FileLogger.format.threadTime, FileLogger.format.threadTime);

  if (exists("./key/"))
  {
    data.private_key = "./key/private.txt".readText().strip();
    data.public_key = "./key/public.txt".readText().strip();
  }

  data.last_order_time = "./db/last_poo_time.txt".readText().strip().to!long;
  data.last_check_position_time = "./db/last_check_position_time.txt".readText().strip().to!long;

  if(exists("./db/trade_config.txt")){
    load_config();
  } else{
    update_config();
  }
}