module bybit_url;

version (bybit_main_server)
{
  immutable url_wss_public_linear = "wss://stream.bybit.com/v5/public/linear";
  immutable url_rest = "https://api.bybit.com/v5/";
}
version (bybit_test_server)
{
  immutable url_wss_public_linear = "wss://stream-testnet.bybit.com/v5/public/linear";
  immutable url_rest = "https://api-testnet.bybit.com/v5/";
}
